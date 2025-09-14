#!/usr/bin/env python3
"""
K-Means bucketing for Thai Poker hands using your precomputed probability table.

- Loads TTP0 probability table (counts) and hand sizes.
- Builds feature vectors per hand size n_me by concatenating p(bid | n_opp) over n_opp=1..6 and 68 bids (excluding CHECK).
- Applies sqrt (Hellinger) + per-column z-score normalization.
- Runs Mini-Batch K-Means on GPU (PyTorch) with CPU fallback.
- Saves:
    bucket_of_hand_index.npy   # length HAND_NB, -1 if not clustered for that index
    labels_n{n}.npy            # per n_me labels (cluster id in [0..K_n-1])
    centers_n{n}.npy           # cluster centers (normalized feature space)
    norm_n{n}.npz              # mean/std used for z-score (per column)
    config.json

Author: (you)
"""

import argparse
import json
import math
import os
import struct
import sys
import time
from pathlib import Path

import numpy as np

try:
    import torch
except Exception as e:
    torch = None

try:
    from tqdm import tqdm
except Exception:
    # Minimal fallback
    def tqdm(x, **kwargs):
        return x


# ----------------------------
# Binary reader for TTP0 table
# ----------------------------

class TTP0:
    """
    Reader for your P-table binary format:
    Header:
      magic: b'TTP0'
      u32: version
      u32: BET_NB
      u32: CARD_NB_plus_1   (should be 25)
      u32: HAND_NB
    Payload:
      int32 counts, shape (BET_NB, CARD_NB_plus_1, HAND_NB)
    """
    def __init__(self, path: str):
        self.path = str(path)
        with open(self.path, "rb") as f:
            header = f.read(4)
            if header != b"TTP0":
                raise ValueError(f"Bad magic in {self.path}: {header!r}")
            hdr = f.read(16)
            if len(hdr) != 16:
                raise ValueError("Short header")
            self.version, self.BET_NB, self.CARD_NB_plus_1, self.HAND_NB = struct.unpack("<4I", hdr)
            # read int32 array
            need = self.BET_NB * self.CARD_NB_plus_1 * self.HAND_NB
            data = np.fromfile(f, dtype="<i4", count=need)
            if data.size != need:
                raise ValueError(f"Expected {need} int32, got {data.size}")
            self.counts = data.reshape(self.BET_NB, self.CARD_NB_plus_1, self.HAND_NB)
        # Basic checks
        if not (1 <= self.version <= 10):
            print(f"[warn] unusual version {self.version}", file=sys.stderr)
        if self.CARD_NB_plus_1 != 25:
            print(f"[warn] CARD_NB_plus_1={self.CARD_NB_plus_1} (expected 25 for 24-card deck)", file=sys.stderr)

    def detect_check_index(self) -> int:
        """Heuristic: the CHECK 'bet' row is all zeros in counts."""
        # Sum across all card_nb and hands
        row_sums = self.counts.reshape(self.BET_NB, -1).sum(axis=1)
        zeros = np.where(row_sums == 0)[0]
        if zeros.size == 1:
            return int(zeros[0])
        # Fallback: pick argmin
        idx = int(np.argmin(row_sums))
        print(f"[warn] unable to uniquely detect CHECK; choosing index {idx} with smallest sum", file=sys.stderr)
        return idx


# ----------------------------
# Combinatorics helper
# ----------------------------

def precompute_nCk(max_n=24) -> np.ndarray:
    """Return array comb[n,k] for 0<=n<=max_n, 0<=k<=max_n."""
    comb = np.zeros((max_n + 1, max_n + 1), dtype=np.float64)
    for n in range(max_n + 1):
        comb[n, 0] = 1.0
        comb[n, n] = 1.0
        for k in range(1, n):
            comb[n, k] = comb[n - 1, k - 1] + comb[n - 1, k]
    return comb


# ----------------------------
# Feature construction
# ----------------------------

def build_features_for_n_me(ttp: TTP0,
                            hand_sizes: np.ndarray,
                            n_me: int,
                            check_idx: int,
                            comb: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Build features X for all hands with size == n_me.
    X shape: (N_hands_of_n_me, 68*6) – 68 bids (excluding CHECK) × 6 opponent sizes
    Returns (X_float32, hand_indices) where hand_indices are the hand_index positions into TTP0.
    """
    assert 1 <= n_me <= 6
    # Find hands of this size
    idxs = np.where(hand_sizes == n_me)[0]
    if idxs.size == 0:
        raise ValueError(f"No hands found with size {n_me}. Check your hand_sizes.npy.")
    BET_NB, CARD_NB_plus_1, HAND_NB = ttp.BET_NB, ttp.CARD_NB_plus_1, ttp.HAND_NB
    # Bids excluding CHECK
    bids = [b for b in range(BET_NB) if b != check_idx]
    B_no_check = len(bids)  # should be 68

    # Precompute denominators: C(24 - n_me, n_opp) for n_opp=1..6
    denom = np.array([comb[24 - n_me, n_opp] for n_opp in range(1, 7)], dtype=np.float64)  # shape (6,)

    # Allocate
    X = np.empty((idxs.size, B_no_check * 6), dtype=np.float32)

    # Fill
    # For each n_opp in 1..6, card_nb = n_me + n_opp
    # p = counts / C(24 - n_me, n_opp)
    # counts shape slice: (B_no_check, HAND_NB) at card_nb row.
    col = 0
    for n_opp in range(1, 7):
        card_nb = n_me + n_opp
        # Sanity
        if not (0 <= card_nb < CARD_NB_plus_1):
            raise ValueError(f"card_nb out of bounds: {card_nb}")
        counts_slice = ttp.counts[bids, card_nb, :][:, idxs].astype(np.float64)  # (B_no_check, N_idx)
        probs = (counts_slice / denom[n_opp - 1]).T.astype(np.float32)  # (N_idx, B_no_check)
        X[:, col:col + B_no_check] = probs
        col += B_no_check

    return X, idxs


def normalize_features_inplace(X: np.ndarray, eps: float = 1e-12) -> tuple[np.ndarray, np.ndarray]:
    """
    Apply recommended transform: sqrt(p), then per-column z-score.
    Returns (mean, std) used.
    """
    # Hellinger transform
    np.sqrt(X, out=X)
    # Per-column z-score
    mean = X.mean(axis=0, dtype=np.float64)
    std = X.std(axis=0, dtype=np.float64)
    std_safe = np.where(std < eps, 1.0, std)
    X -= mean
    X /= std_safe
    return mean.astype(np.float32), std_safe.astype(np.float32)


# ----------------------------
# Mini-Batch K-Means (Torch)
# ----------------------------

class MiniBatchKMeansTorch:
    """
    Mini-Batch K-Means in PyTorch (GPU-accelerated) with CPU fallback.
    - Centers kept on device.
    - Data can stay on CPU; streamed in batches.
    - Uses exact nearest-center assignment per batch (chunked over centers if needed).
    """
    def __init__(self, n_clusters: int, iters: int = 100, batch_size: int = 8192,
                 center_chunk: int = 1024, device: str | None = None, seed: int = 42,
                 reinit_empty: bool = True):
        self.K = n_clusters
        self.iters = iters
        self.batch_size = batch_size
        self.center_chunk = center_chunk
        self.device = device or ("cuda" if (torch is not None and torch.cuda.is_available()) else "cpu")
        self.seed = seed
        self.reinit_empty = reinit_empty
        self.centers = None

    def _to_device(self, x: np.ndarray) -> torch.Tensor:
        return torch.from_numpy(x).to(self.device)

    @torch.no_grad()
    def _assign_batch(self, Xb: torch.Tensor, centers: torch.Tensor) -> tuple[torch.Tensor, torch.Tensor]:
        """
        Assign each row in Xb to nearest center. Returns (indices, min_dist2).
        Computes distances in chunks over centers to reduce peak memory.
        """
        B, D = Xb.shape
        K = centers.shape[0]
        x_norm = (Xb * Xb).sum(dim=1)  # (B,)
        best_d2 = torch.full((B,), float("inf"), device=Xb.device)
        best_idx = torch.zeros(B, dtype=torch.long, device=Xb.device)
        for start in range(0, K, self.center_chunk):
            end = min(start + self.center_chunk, K)
            C = centers[start:end, :]  # (Ck, D)
            c_norm = (C * C).sum(dim=1)  # (Ck,)
            # d^2 = ||x||^2 + ||c||^2 - 2 x.c
            # (B, Ck)
            dot = Xb @ C.t()
            d2 = x_norm[:, None] + c_norm[None, :] - 2.0 * dot
            d2 = torch.clamp(d2, min=0.0)
            d2_min, idx_local = d2.min(dim=1)
            better = d2_min < best_d2
            best_idx[better] = idx_local[better] + start
            best_d2[better] = d2_min[better]
            # free memory
            del C, c_norm, dot, d2, d2_min, idx_local, better
            torch.cuda.empty_cache() if self.device.startswith("cuda") else None
        return best_idx, best_d2

    @torch.no_grad()
    def fit(self, X: np.ndarray) -> np.ndarray:
        """
        X: (N, D) float32 on CPU. Returns labels (N,) int32.
        Populates self.centers (K, D) on device and returns final labels numpy array.
        """
        if torch is None:
            raise RuntimeError("PyTorch is not available. Install torch to use GPU.")
        torch.manual_seed(self.seed)
        N, D = X.shape
        # Random init: pick K distinct points
        if self.K > N:
            raise ValueError(f"n_clusters K={self.K} > N={N}")
        init_idx = np.random.default_rng(self.seed).choice(N, size=self.K, replace=False)
        centers = self._to_device(X[init_idx].copy())  # (K, D)

        # Iterations
        for it in range(self.iters):
            # Fresh accumulators
            sums = torch.zeros_like(centers)             # (K,D)
            counts = torch.zeros(self.K, device=self.device, dtype=torch.long)  # (K,)

            # Iterate mini-batches
            order = np.random.permutation(N)
            for start in range(0, N, self.batch_size):
                end = min(start + self.batch_size, N)
                Xb_cpu = X[order[start:end]]  # (B,D) on CPU
                Xb = self._to_device(Xb_cpu)  # to device
                idx, _ = self._assign_batch(Xb, centers)
                # Accumulate
                counts += torch.bincount(idx, minlength=self.K)
                sums.index_add_(0, idx, Xb)
                # free
                del Xb, idx

            # Update centers
            nonempty = counts > 0
            if nonempty.any():
                centers[nonempty] = sums[nonempty] / counts[nonempty].unsqueeze(1)
            # Optionally reinit empties to random points
            if self.reinit_empty and (~nonempty).any():
                empties = torch.where(~nonempty)[0]
                repl_idx = np.random.default_rng(self.seed + it).choice(N, size=empties.numel(), replace=True)
                centers[empties] = self._to_device(X[repl_idx])

        # Final labeling pass
        labels = np.empty(N, dtype=np.int32)
        for start in tqdm(range(0, N, self.batch_size), desc="Final assign"):
            end = min(start + self.batch_size, N)
            Xb = self._to_device(X[start:end])
            idx, _ = self._assign_batch(Xb, centers)
            labels[start:end] = idx.cpu().numpy().astype(np.int32)
            del Xb, idx

        self.centers = centers  # keep on device
        return labels


# ----------------------------
# CLI / Main
# ----------------------------

def parse_args():
    ap = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    ap.add_argument("--P", required=True, help="Path to TTP0 P-table binary (written by your C++ save())")
    group = ap.add_mutually_exclusive_group(required=True)
    group.add_argument("--hand-sizes", help="Path to .npy (uint8) of length HAND_NB where entry = in_hand (0..6)")
    group.add_argument("--hand-masks", help="Path to .npy (uint32) of length HAND_NB with 24-bit masks (popcount to get size)")
    ap.add_argument("--out", default="kmeans_out", help="Output directory")
    ap.add_argument("--device", default=None, help="cuda / cpu (default: auto-detect)")
    ap.add_argument("--iters", type=int, default=100, help="Mini-batch kmeans iterations per n_me")
    ap.add_argument("--batch-size", type=int, default=8192, help="Mini-batch size")
    ap.add_argument("--center-chunk", type=int, default=1024, help="Centers chunk size for distance compute")
    ap.add_argument("--seed", type=int, default=42, help="Random seed")
    ap.add_argument("--K", default="24,276,2024,4000,4000,4000",
                    help="Comma-separated K for n_me=1..6. Use 'auto' to set K=#hands for n_me<=3 and 4000 for 4/5/6.")
    ap.add_argument("--detect-check", action="store_true",
                    help="Auto-detect CHECK bet index (row of all zeros). Otherwise pass --check-idx.")
    ap.add_argument("--check-idx", type=int, default=None, help="Explicit bet index for CHECK if known")
    return ap.parse_args()


def load_hand_sizes(npy_path: str | None, masks_npy_path: str | None) -> np.ndarray:
    if npy_path:
        hs = np.load(npy_path)
        if hs.dtype != np.uint8:
            hs = hs.astype(np.uint8)
        return hs
    else:
        masks = np.load(masks_npy_path)
        if masks.dtype != np.uint32:
            masks = masks.astype(np.uint32)
        # popcount of 24 bits
        hs = np.unpackbits(masks.view(np.uint8)).reshape(-1, 32)[:, :24].sum(axis=1).astype(np.uint8)
        return hs


def parse_K(argK: str, counts_by_size: dict[int, int]) -> dict[int, int]:
    if argK.strip().lower() == "auto":
        K = {}
        for n in range(1, 7):
            if n <= 3:
                K[n] = counts_by_size[n]  # use all hands (no clustering)
            else:
                K[n] = 4000
        return K
    parts = [int(x) for x in argK.split(",")]
    if len(parts) != 6:
        raise ValueError("--K must have 6 integers or be 'auto'")
    return {n: parts[n - 1] for n in range(1, 7)}


def main():
    args = parse_args()
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    print("[info] Loading P-table...")
    ttp = TTP0(args.P)
    print(f"[info] version={ttp.version}, BET_NB={ttp.BET_NB}, CARD_NB+1={ttp.CARD_NB_plus_1}, HAND_NB={ttp.HAND_NB}")

    # Determine CHECK bet index
    if args.check_idx is not None:
        check_idx = int(args.check_idx)
    elif args.detect_check:
        check_idx = ttp.detect_check_index()
        print(f"[info] Detected CHECK index: {check_idx}")
    else:
        raise SystemExit("Provide --check-idx or --detect-check to identify the CHECK bet row.")

    # Load hand sizes
    hand_sizes = load_hand_sizes(args.hand_sizes, args.hand_masks)
    if hand_sizes.shape[0] != ttp.HAND_NB:
        raise ValueError(f"hand_sizes length {hand_sizes.shape[0]} != HAND_NB {ttp.HAND_NB}")
    # Count by n_me (expect these: 24, 276, 2024, 10626, 42504, 134596)
    counts_by_size = {n: int((hand_sizes == n).sum()) for n in range(0, 7)}
    print("[info] hand size counts:", counts_by_size)

    # Parse K per n_me
    K_by_n = parse_K(args.K, counts_by_size)
    print("[info] K per n_me:", K_by_n)

    # Combinatorics
    comb = precompute_nCk(24)

    # Prepare global mapping (default -1)
    bucket_of_hand_index = np.full(ttp.HAND_NB, -1, dtype=np.int32)

    # K-means trainer
    if torch is None and (args.device is None or args.device.startswith("cuda")):
        print("[warn] PyTorch not available. Falling back to CPU; this will be slower.", file=sys.stderr)
    device = args.device or ("cuda" if (torch is not None and torch.cuda.is_available()) else "cpu")

    timings = {}
    for n_me in range(1, 7):
        N_expected = counts_by_size[n_me]
        if N_expected == 0:
            print(f"[warn] No hands for n_me={n_me}, skipping.", file=sys.stderr)
            continue
        K = K_by_n[n_me]
        # Fast path: if K equals number of hands, just assign unique buckets
        if K >= N_expected:
            # Identity mapping
            _, idxs = build_features_for_n_me(ttp, hand_sizes, n_me, check_idx, comb)
            labels = np.arange(N_expected, dtype=np.int32)
            centers = np.empty((0, 0), dtype=np.float32)
            mean = np.empty((0,), dtype=np.float32)
            std = np.empty((0,), dtype=np.float32)
            bucket_of_hand_index[idxs] = labels
            np.save(out_dir / f"labels_n{n_me}.npy", labels)
            np.save(out_dir / f"centers_n{n_me}.npy", centers)
            np.savez(out_dir / f"norm_n{n_me}.npz", mean=mean, std=std)
            print(f"[info] n_me={n_me}: K={K} (identity), assigned {N_expected} buckets.")
            timings[f"n{n_me}"] = 0.0
            continue

        # Build features
        t0 = time.time()
        X, idxs = build_features_for_n_me(ttp, hand_sizes, n_me, check_idx, comb)
        # Transform
        mean, std = normalize_features_inplace(X)
        t1 = time.time()

        # Fit
        print(f"[info] n_me={n_me}: X shape={X.shape}, running Mini-Batch K-Means on {device}...")
        kmeans = MiniBatchKMeansTorch(n_clusters=K, iters=args.iters, batch_size=args.batch_size,
                                      center_chunk=args.center_chunk, device=device, seed=args.seed)
        labels = kmeans.fit(X)  # returns (N,) int32
        t2 = time.time()

        # Save per-group
        np.save(out_dir / f"labels_n{n_me}.npy", labels)
        # Save centers (pull to CPU)
        centers = kmeans.centers.detach().cpu().numpy().astype(np.float32)
        np.save(out_dir / f"centers_n{n_me}.npy", centers)
        np.savez(out_dir / f"norm_n{n_me}.npz", mean=mean, std=std)

        # Fill global mapping
        bucket_of_hand_index[idxs] = labels

        print(f"[info] n_me={n_me}: featurize+norm {t1 - t0:.2f}s, kmeans {t2 - t1:.2f}s")
        timings[f"n{n_me}"] = (t1 - t0, t2 - t1)

        # Free GPU mem between groups
        if torch is not None and device.startswith("cuda"):
            torch.cuda.empty_cache()

    # Save global mapping
    np.save(out_dir / "bucket_of_hand_index.npy", bucket_of_hand_index)

    # Save config
    cfg = {
        "P": args.P,
        "hand_sizes": args.hand_sizes,
        "hand_masks": args.hand_masks,
        "device": device,
        "iters": args.iters,
        "batch_size": args.batch_size,
        "center_chunk": args.center_chunk,
        "seed": args.seed,
        "K_per_n_me": K_by_n,
        "check_idx": int(check_idx),
        "timings": {k: (float(v) if isinstance(v, (int, float)) else [float(x) for x in v])
                    for k, v in timings.items()},
    }
    with open(out_dir / "config.json", "w") as f:
        json.dump(cfg, f, indent=2)
    print(f"[done] Saved bucket mapping and artifacts to {out_dir.resolve()}")


if __name__ == "__main__":
    main()
