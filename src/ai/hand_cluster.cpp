#include "hand_cluster.hpp"

#include <random>
#include <array>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <set>
#include <cstring>

namespace thai_poker {

HandCluster::HandCluster(const std::string& filename)
    : prob_table(ProbabilityTable::instance()), hand_table(HandTable::instance()), rng(2137) {
    if (load(filename)) {
        std::cerr << "HandClusters loaded" << std::endl;
    }
    else {
        std::cerr << "Building HandClusters" << std::endl;
        build_kmeans();
        build_clusters_ds();
        save(filename);
    }
}

HandCluster& HandCluster::instance() {
    static HandCluster singleton(std::string(DATA_DIR) + "/HCL0.bin");
    return singleton;
}

double HandCluster::Point::distance(HandCluster::Point const& other) const {
    double dist = 0;
    for (int bet = 0; bet < BET_NB; bet++) {
        const double diff = p[bet] - other.p[bet];
        dist += std::abs(diff);
    }
    return dist;
    // return std::sqrt(std::max<double>(0.0, dist));
}

std::pair<int, Hand> HandCluster::sample_hand(Cluster const& cluster) {
    int which = std::uniform_int_distribution<int>(0, cluster.blocks_prefix_sum.back() - 1)(rng);

    auto const& prefix_sums = cluster.blocks_prefix_sum;
    int block = static_cast<int>(std::upper_bound(prefix_sums.begin(), prefix_sums.end(), which)
                                - prefix_sums.begin());

    if (block > 0) {
        which -= prefix_sums[block - 1];
    }

    return std::make_pair(
        block,
        hand_table.from_index(cluster.blocks[block][which].hand_index)
    );
}

void HandCluster::build_kmeans() {
    long long sum_all = 0;
    long long sum_kmeans = 0;
    for (int hand_size = 0; hand_size <= HAND_SZ; hand_size++) {
        for (int opp_size = 0; hand_size + opp_size <= CARD_NB; opp_size++) {
            std::vector<Point> data;
            std::set<std::array<double, BET_NB>> unique_check;

            for (int hand_index = 0; hand_index < HAND_NB; hand_index++) {
                Hand hand = hand_table.from_index(hand_index);
                if (popcount(hand) == hand_size) {
                    std::array<double, BET_NB> p{}, c{};
                    for (int bet = 0; bet < BET_NB; bet++) {
                        p[bet] = prob_table.get_prob(static_cast<Bet>(bet), opp_size + hand_size, hand);
                        c[bet] = prob_table.get_comp(static_cast<Bet>(bet), opp_size + hand_size, hand);
                    }
                    data.push_back(Point{c, hand_index, opp_size});
                    unique_check.insert(c);
                }
            }

            std::cerr << "hand_size: " << hand_size << " opp_size: " << opp_size << " data.size(): " << data.size() << std::endl;
            std::cerr << "unique_check.size(): " << unique_check.size() << std::endl;

            std::vector<Point> kmeans;
            int kmeans_size = KMEANS_K;
            if (unique_check.size() < KMEANS_K) {
                for (auto c : unique_check) {
                    kmeans.push_back(Point{c, -1, -1});
                }
                kmeans_size = static_cast<int>(kmeans.size());
            }
            else {
                for (int i = 0; i < kmeans_size; i++) {
                    Point center{};
                    for (int bet = 0; bet < BET_NB; bet++) {
                        center.p[bet] = std::uniform_real_distribution<double>(0, 1)(rng);
                    }
                    kmeans.push_back(center);
                }
            }

            sum_all += unique_check.size();
            sum_kmeans += kmeans_size;
            std::cerr << "sum_all: " << sum_all << std::endl;
            std::cerr << "sum_kmeans: " << sum_kmeans << std::endl;

            assert(kmeans_size == static_cast<int>(kmeans.size()));
            for (int iter = 0; iter < KMEANS_ITER; iter++) {
                double cum_error = 0;
                std::vector<Point> center_sum(kmeans_size);
                std::vector<int> center_cnt(kmeans_size);

                for (Point const& point : data) {
                    int best_center = -1;
                    double best_error = std::numeric_limits<double>::max();
                    for (int center = 0; center < kmeans_size; center++) {
                        double error = point.distance(kmeans[center]);
                        if (best_error > error) {
                            best_error = error;
                            best_center = center;
                        }
                    }

                    cum_error += best_error;
                    center_cnt[best_center]++;
                    for (int bet = 0; bet < BET_NB; bet++) {
                        center_sum[best_center].p[bet] += point.p[bet];
                    }
                }

                std::cerr << "KMEANS(iter: " << iter << "): cum_error => " << cum_error << std::endl;

                for (int center = 0; center < kmeans_size; center++) {
                    for (int bet = 0; bet < BET_NB; bet++) {
                        kmeans[center].p[bet] = center_sum[center].p[bet] / center_cnt[center];
                    }
                }

                if (cum_error < 1e-7)
                    break;
            }

            Cluster& c = clusters[hand_size][opp_size];
            c.centers = kmeans;
            c.blocks.resize(kmeans_size, {});

            double final_error = 0;
            for (Point const& point : data) {
                int best_center = -1;
                double best_error = std::numeric_limits<double>::max();
                for (int center = 0; center < kmeans_size; center++) {
                    double error = point.distance(kmeans[center]);
                    if (best_error > error) {
                        best_error = error;
                        best_center = center;
                    }
                }

                final_error += best_error;

                c.blocks[best_center].push_back(point);
            }

            std::cerr << "final_error => " << final_error << std::endl;
        }
    }

    std::cerr << "sum_all: " << sum_all << std::endl;
    std::cerr << "sum_kmeans: " << sum_kmeans << std::endl;
    std::cerr << "DONE" << std::endl;
}

void HandCluster::build_clusters_ds() {
    for (int hand_size = 0; hand_size <= HAND_SZ; hand_size++) {
        for (int opp_size = 0; opp_size <= CARD_NB; opp_size++) {
            Cluster& cluster = clusters[hand_size][opp_size];
            const int n = static_cast<int>(cluster.blocks.size());
            cluster.blocks_prefix_sum.resize(n, 0);
            for (int i = 0; i < n; i++) {
                cluster.blocks_prefix_sum[i] = static_cast<int>(cluster.blocks[i].size());
                if (i > 0) {
                    cluster.blocks_prefix_sum[i] += cluster.blocks_prefix_sum[i - 1];
                }
            }
        }
    }
}

GameSample HandCluster::sample(int h1_size, int h2_size) {
    Cluster const& c1 = clusters[h1_size][h2_size];
    Cluster const& c2 = clusters[h2_size][h1_size];

    assert(!c1.blocks.empty());
    assert(!c2.blocks.empty());

    int h1_block, h2_block;
    Hand h1_hand, h2_hand;

    do {
        std::tie(h1_block, h1_hand) = sample_hand(c1);
        std::tie(h2_block, h2_hand) = sample_hand(c2);
    } while ((h1_hand & h2_hand) != 0);

    return GameSample {
        h1_hand, h1_block,
        h2_hand, h2_block
    };
}

void HandCluster::save(const std::string& path) const {
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) throw std::runtime_error("Cannot open " + path);

    u32 hands = HAND_SZ + 1;
    u32 cards = CARD_NB + 1;
    std::fwrite("HCL0", 1, 4, f);
    std::fwrite(&VERSION, sizeof(VERSION), 1, f);

    std::fwrite(&hands, sizeof(hands), 1, f);
    std::fwrite(&cards, sizeof(cards), 1, f);

    for (int hand_size = 0; hand_size <= HAND_SZ; hand_size++) {
        for (int opp_size = 0; opp_size <= CARD_NB; opp_size++) {
            Cluster const& c = clusters[hand_size][opp_size];

            u32 n_blocks = static_cast<u32>(c.blocks.size());
            std::fwrite(&n_blocks, sizeof(n_blocks), 1, f);
            for (auto const& block : c.blocks) {
                u32 n_points = static_cast<u32>(block.size());
                std::fwrite(&n_points, sizeof(n_points), 1, f);
                for (auto const& p : block) {
                    std::fwrite(p.p.data(), sizeof(double), BET_NB, f);
                    std::fwrite(&p.hand_index, sizeof(int), 1, f);
                    std::fwrite(&p.opp_size, sizeof(int), 1, f);
                }
            }

            u32 n_prefix = static_cast<u32>(c.blocks_prefix_sum.size());
            std::fwrite(&n_prefix, sizeof(n_prefix), 1, f);
            std::fwrite(c.blocks_prefix_sum.data(), sizeof(int), n_prefix, f);

            u32 n_centers = static_cast<u32>(c.centers.size());
            std::fwrite(&n_centers, sizeof(n_centers), 1, f);
            for (auto const& p : c.centers) {
                std::fwrite(p.p.data(), sizeof(double), BET_NB, f);
                std::fwrite(&p.hand_index, sizeof(int), 1, f);
                std::fwrite(&p.opp_size, sizeof(int), 1, f);
            }
        }
    }

    std::fclose(f);
}

bool HandCluster::load(const std::string& path) {
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;

    char magic[4];
    if (std::fread(magic, 1, 4, f) != 4 || std::memcmp(magic, "HCL0", 4) != 0) {
        std::fclose(f);
        return false;
    }

    u32 version=0, hands=0, cards=0;
    std::fread(&version, 1, 4, f);
    std::fread(&hands, 1, 4, f);
    std::fread(&cards, 1, 4, f);

    if (version != VERSION || hands != HAND_SZ+1 || cards != CARD_NB+1) {
        std::fclose(f);
        throw std::runtime_error("HandCluster::load: version/dim mismatch");
    }

    for (int hand_size = 0; hand_size <= HAND_SZ; hand_size++) {
        for (int opp_size = 0; opp_size <= CARD_NB; opp_size++) {
            Cluster& c = clusters[hand_size][opp_size];

            u32 n_blocks;
            std::fread(&n_blocks, sizeof(n_blocks), 1, f);
            c.blocks.resize(n_blocks);
            for (u32 b = 0; b < n_blocks; ++b) {
                u32 n_points;
                std::fread(&n_points, sizeof(n_points), 1, f);
                c.blocks[b].resize(n_points);
                for (u32 i = 0; i < n_points; ++i) {
                    Point& p = c.blocks[b][i];
                    std::fread(p.p.data(), sizeof(double), BET_NB, f);
                    std::fread(&p.hand_index, sizeof(int), 1, f);
                    std::fread(&p.opp_size, sizeof(int), 1, f);
                }
            }

            u32 n_prefix;
            std::fread(&n_prefix, sizeof(n_prefix), 1, f);
            c.blocks_prefix_sum.resize(n_prefix);
            std::fread(c.blocks_prefix_sum.data(), sizeof(int), n_prefix, f);

            u32 n_centers;
            std::fread(&n_centers, sizeof(n_centers), 1, f);
            c.centers.resize(n_centers);
            for (u32 i = 0; i < n_centers; ++i) {
                Point& p = c.centers[i];
                std::fread(p.p.data(), sizeof(double), BET_NB, f);
                std::fread(&p.hand_index, sizeof(int), 1, f);
                std::fread(&p.opp_size, sizeof(int), 1, f);
            }
        }
    }

    std::fclose(f);
    return true;
}

} // namespace thai_poker