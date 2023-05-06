// Olaf Surgut 06.07.2022 23:54:34
#include <bits/stdc++.h>

using namespace std;

mt19937 rng(2137);

int order_rank(char rank) {
	if (rank == '9')
		return 9;
	if (rank == 'T')
		return 10;
	if (rank == 'J')
		return 11;
	if (rank == 'Q')
		return 12;
	if (rank == 'K')
		return 13;
	if (rank == 'A')
		return 14;

	assert(false);
	return -1;
}

int order_suit(char suit) {
	if (suit == 'C')
		return 1;
	if (suit == 'D')
		return 2;
	if (suit == 'H')
		return 3;
	if (suit == 'S')
		return 4;

	assert(false);
	return -1;
}

struct Card {
	char suit;
	char rank;

	bool operator< (const Card &other) const {
		if (rank != other.rank)
			return order_rank(rank) < order_rank(other.rank);
		return order_suit(suit) < order_suit(other.suit);
	}
};

typedef vector<Card> Deck;

Deck all_cards;

void init_all_cards() {
	for (char suit : {'C', 'D', 'H', 'S'}) {
		for (char rank : {'9', 'T', 'J', 'Q', 'K', 'A'}) {
			all_cards.push_back(Card{suit, rank});
		}
	}
}

bool check_for_pair(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			return order_rank(a.rank) < order_rank(b.rank);	
		}
	);

	for (int i = 1; i < (int) cards.size(); i++) {
		if (cards[i - 1].rank == cards[i].rank)
			return true;
	}

	return false;
}

bool check_for_low_straight(const Deck &cards) {
	set<Card> occ(cards.begin(), cards.end());

	for (char rank : {'9', 'T', 'J', 'Q', 'K'}) {
		bool ok = false;

		for (char suit : {'C', 'D', 'H', 'S'}) {
			if (occ.count(Card{suit, rank}) > 0) {
				ok = true;
				break;
			}
		}

		if (ok == false) {
			return false;
		}
	}

	return true;
}

bool check_for_high_straight(const Deck &cards) {
	set<Card> occ(cards.begin(), cards.end());

	for (char rank : {'T', 'J', 'Q', 'K', 'A'}) {
		bool ok = false;

		for (char suit : {'C', 'D', 'H', 'S'}) {
			if (occ.count(Card{suit, rank}) > 0) {
				ok = true;
				break;
			}
		}

		if (ok == false) {
			return false;
		}
	}

	return true;
}

bool check_for_three(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			return order_rank(a.rank) < order_rank(b.rank);
		}
	);

	for (int i = 1; i + 1 < (int) cards.size(); i++) {
		if (cards[i - 1].rank == cards[i + 1].rank) {
			return true;
		}
	}

	return false;
}

bool check_for_full(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			return order_rank(a.rank) < order_rank(b.rank);
		}
	);

	char three_rank = -1;
	for (int i = 1; i + 1 < (int) cards.size(); i++) {
		if (cards[i - 1].rank == cards[i + 1].rank) {
			three_rank = cards[i].rank;
		}
	}

	if (three_rank == -1)
		return false;
	
	for (int i = 0; i + 1 < (int) cards.size(); i++) {
		if (cards[i].rank == cards[i + 1].rank &&
			cards[i].rank != three_rank) {
			return true;
		}
	}

	return false;
}

bool check_for_flush(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			return order_suit(a.suit) < order_suit(b.suit);
		}
	);

	for (int i = 2; i + 2 < (int) cards.size(); i++) {
		if (cards[i - 2].suit == cards[i + 2].suit) {
			return true;
		}
	}

	return false;
}

bool check_for_quads(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			return order_rank(a.rank) < order_rank(b.rank);
		}
	);

	for (int i = 2; i + 1 < (int) cards.size(); i++) {
		if (cards[i - 2].rank == cards[i + 1].rank) {
			return true;
		}
	}

	return false;
}

bool check_for_straight_flush(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			if (a.suit != b.suit)
				return order_suit(a.suit) < order_suit(b.suit);
			return order_rank(a.rank) < order_rank(b.rank);
		}
	);

	for (int i = 2; i + 2 < (int) cards.size(); i++) {
		if (cards[i - 2].suit == cards[i + 2].suit &&
			cards[i + 2].rank - cards[i - 2].rank == 4) {
			return true;
		}
	}

	return false;
}


bool check_for_royal_flush(Deck cards) {
	sort(cards.begin(), cards.end(),
		[](const Card &a, const Card &b) {
			if (a.suit != b.suit)
				return order_suit(a.suit) < order_suit(b.suit);
			return order_rank(a.rank) < order_rank(b.rank);
		}
	);

	for (int i = 2; i + 2 < (int) cards.size(); i++) {
		if (cards[i - 2].suit == cards[i + 2].suit &&
			cards[i - 2].rank == 'T') {
			return true;
		}
	}

	return false;
}

int main() {
	ios::sync_with_stdio(false), cin.tie(nullptr);

	init_all_cards();

	const int total = 1e5;

	map<int, int> pair_count;
	map<int, int> low_straight_count;
	map<int, int> high_straight_count;
	map<int, int> three_count;
	map<int, int> full_count;
	map<int, int> flush_count;
	map<int, int> quads_count;
	map<int, int> straight_flush_count;
	map<int, int> royal_flush_count;

	for (int rep = 0; rep < total; rep++) {
		shuffle(all_cards.begin(), all_cards.end(), rng);

		Deck current_cards;

		for (int drawn = 0; drawn < (int) all_cards.size(); drawn++) {
			current_cards.push_back(all_cards[drawn]);

			#define concat2(x, y) x ## y
			#define concat(x, y) concat2(x, y)
	
			#define macro_count(x) \
			({  \
				if (concat(check_for_, x)(current_cards))\
					concat(x, _count)[drawn + 1]++;\
			})

			macro_count(pair);
			macro_count(low_straight);
			macro_count(high_straight);
			macro_count(three);
			macro_count(full);
			macro_count(flush);
			macro_count(quads);
			macro_count(straight_flush);
			macro_count(royal_flush);
		}
	}

	auto print_stats = [&](string name, map<int, int> cnt) {
		cerr << "hand: " << name << '\n';
		for (int drawn = 1; drawn <= (int) all_cards.size(); drawn++) {
			cerr << " #cards: " << drawn << ": " << cnt[drawn] / (double) total << '\n';
		}
		cerr << '\n';
	};

	print_stats("pair", pair_count);
	print_stats("low straight", low_straight_count);
	print_stats("high straght", high_straight_count);
	print_stats("three", three_count);
	print_stats("full", full_count);
	print_stats("flush", flush_count);
	print_stats("quads", quads_count);
	print_stats("straight flush", straight_flush_count);
	print_stats("royal flush", royal_flush_count);
}

