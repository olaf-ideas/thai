#!/usr/bin/env python3

suit = ['C', 'D', 'H', 'S']
rank = ['9', 'T', 'J', 'Q', 'K', 'A']
CARDS = len(suit) * len(rank)

hand = int(input())

for i in range(CARDS):
    if (hand >> i) & 1:
        print(suit[i % len(suit)], rank[i // len(suit)])
