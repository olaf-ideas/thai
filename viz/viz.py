import numpy as np
import struct

def load_ttp0(path):
    with open(path, "rb") as f:
        magic = f.read(4)
        if magic != b"TTP0":
            raise ValueError("bad magic")
        version, bets, cards, hands = struct.unpack("<4I", f.read(16))
        print("version", version, "bets", bets, "cards", cards, "hands", hands)
        data = np.fromfile(f, dtype=np.int32, count=bets*cards*hands)
    return data.reshape((bets, cards, hands))

P = load_ttp0("data/TTP0.bin")

from sklearn.decomposition import PCA

card_nb = 5
sample_idx = np.random.choice(P.shape[2], size=5000, replace=False)
X = P[:68, card_nb, sample_idx].T   # shape (n_samples, 68)

X = X / (X.sum(axis=1, keepdims=True) + 1e-9)  # normalize

pca = PCA(n_components=2)
coords = pca.fit_transform(X)

plt.figure(figsize=(6,6))
plt.scatter(coords[:,0], coords[:,1], s=2, alpha=0.5)
plt.xlabel("PC1")
plt.ylabel("PC2")
plt.title("PCA projection of 68-D bet vectors")
plt.show()