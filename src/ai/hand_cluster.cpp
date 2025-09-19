#include "hand_cluster.hpp"

#include <random>
#include <array>
#include <iostream>

namespace thai_poker {

double HandCluster::Point::distance(HandCluster::Point const& other) const {
    double dist = 0;
    for (int bet = 0; bet < BET_NB; bet++) {
        const double diff = p[bet] - other.p[bet];
        dist += diff * diff;
    }
    return std::sqrt(dist);
}

std::pair<int, Hand> HandCluster::Cluster::sample_hand() const {
    int which = std::uniform_int_distribution<int>(0, blocks_prefix_sum.back())(rng);

    int block = static_cast<int>(std::lower_bound(blocks_prefix_sum.begin(), blocks_prefix_sum.end(), which)
                                - blocks_prefix_sum.begin());

    if (block > 0) {
        which -= blocks_prefix_sum[block - 1];
    }

    return std::make_pair(
        block,
        hand_table.from_index(blocks[block][which].hand_index)
    );
}

HandCluster::HandCluster() : prob_table(), hand_table(), rng(2137) {
    build_kmeans();
    build_clusters_ds();
}

void HandCluster::build_kmeans() {
    for (int hand_size = MIN_CLUSTER_SIZE; hand_size <= HAND_SZ; hand_size++) {
        std::vector<Point> data;
        for (int opp_size = 1; opp_size <= HAND_SZ; opp_size++) {
            for (int hand_index = 0; hand_index < HAND_NB; hand_index++) {
                Hand hand = hand_table.from_index(hand_index);
                if (popcount(hand) == hand_size) {
                    std::array<double, BET_NB> p{};
                    for (int bet = 0; bet < BET_NB; bet++) {
                        p[bet] = prob_table.get_prob(static_cast<Bet>(bet), opp_size + hand_size, hand);
                    }
                    data.push_back(Point{p, hand_index, opp_size});
                }
            }
        }

        std::cerr << "hand_size: " << hand_size << " data.size(): " << data.size() << std::endl;

        std::vector<Point> kmeans;
        int kmeans_size = KMEANS_K;
        if (data.size() < KMEANS_K) {
            kmeans_size = static_cast<int>(kmeans.size());
            kmeans = data;
        }

        for (int i = 0; i < kmeans_size; i++) {
            Point center{};
            for (int bet = 0; bet < BET_NB; bet++) {
                center.p[bet] = std::uniform_real_distribution<double>(0, 1)(rng);
            }
            kmeans.push_back(center);
        }

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
        }

        Cluster c;
        c.centers = kmeans;
        c.blocks.resize(kmeans_size, {});

        double final_error = 0;
        for (Point const& point : data) {
            int best_center = -1;
            double best_error = std::numeric_limits<double>::max();
            for (int center = 0; center < KMEANS_K; center++) {
                double error = point.distance(kmeans[center]);
                if (best_error > error) {
                    best_error = error;
                    best_center = center;
                }
            }

            final_error += best_error;

            c.blocks[best_center].push_back(point);
        }

        std::cerr << "hand_size: " << hand_size << " final_error => " << final_error << std::endl;
    }
}

void HandCluster::build_clusters_ds() {
    for (Cluster& cluster : clusters) {
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

GameSample HandCluster::sample(int h1_size, int h2_size) const {
    Cluster const& c1 = clusters.at(h1_size);
    Cluster const& c2 = clusters.at(h2_size);

    int h1_block, h2_block;
    Hand h1_hand, h2_hand;

    do {
        std::tie(h1_block, h1_hand) = c1.sample_hand();
        std::tie(h2_block, h2_hand) = c2.sample_hand();
    }
    while ((h1_hand & h2_hand) != 0);

    return GameSample {
        h1_hand, h1_block,
        h2_hand, h2_block
    };
}

} // namespace thai_poker