## Table of Contents
1. [Objective of the Game](#objective-of-the-game)
2. [Gameplay](#gameplay)
   - [Dealing Cards](#dealing-cards)
   - [Bidding](#bidding)
     - [Proposing a Hand](#proposing-a-hand)
     - [Checking](#checking)
3. [Example Round](#example-round)
4. [Tables](#tables)
   - [Initial Number of Cards](#initial-number-of-cards)
   - [Maximum Number of Cards in Hand](#maximum-number-of-cards-in-hand)
   - [Hand Rankings in Thai Poker](#hand-rankings-in-thai-poker)
   - [Suits Order](#suits-order)
5. [Probability Tool for Thai Poker Hands](#probability-tool-for-thai-poker-hands)

#### Official rules created by: Jeremiasz Preiss, December 5, 2023

## Objective of the Game
Thai Poker (also known as Liar's Poker) is a card game for 2 to 10 players. The main goal is to be the last person remaining in the game. Each player has a certain number of cards in hand, and through a bidding process, one player loses the round and receives an additional card. Exceeding the card limit results in elimination from the game.

## Gameplay
The game is played in rounds, each consisting of two phases. Rounds continue until only one player remains — the winner.

### Dealing Cards
Each player receives cards from a deck containing A, K, Q, J, 10, and 9 (total of 24 cards):

1. At the beginning of the game, each player receives a set number of cards based on [Table 1](#initial-number-of-cards).
2. After each round that they do not lose, players receive the same number of cards as in the previous round.
3. After each round they lose, players receive one more card than in the previous round, unless this exceeds the maximum card limit (see [Table 2](#maximum-number-of-cards-in-hand)); in that case, the player is eliminated.

### Bidding
Bidding proceeds clockwise, starting with:

1. At the beginning of the game, traditionally the winner of the previous game, or anyone chosen otherwise.
2. After each subsequent round, the player who lost the previous round, unless that player has been eliminated.
3. If the player who was supposed to start has been eliminated, the bidding starts with the next player to their left.

During their turn, each player can choose one of the following two options. The only exception is the first player to bid — they must choose the first option (the second would be undefined).

#### Proposing a Hand
The player declares one of the hands from [Table 3](#hand-rankings-in-thai-poker). The first proposed hand can be anything, but each subsequent hand must be higher-ranked (e.g., three nines after a big straight) or a higher version of the same type (e.g., a pair of aces after a pair of kings). Since no hand is higher than a royal flush of spades, the next player must check.

#### Checking
The player says "check" and/or reveals their cards. All players show their cards, and they check whether the previously declared hand can be formed from the cards shown (i.e., if it's possible to pick cards that meet the requirements of the hand). If the declared hand exists, the player who checks loses. Otherwise, the player who declared the hand loses. The round ends, and the game returns to [Dealing Cards](#dealing-cards).

## Example Round

### Dealt Cards

| Player A | Player B | Player C |
|----------|----------|----------|
| ♠A       | ♠A       | ♥A       |
| ♥Q       | ♥Q       | ♦J       |
| ♣Q       | ♦9       | ♠J       |

Assume Player A lost the previous round. The order of play is Player A - Player B - Player C.

- **Player A:** Ace (high card ace)
- **Player B:** Two Queens (pair of queens, hoping someone else has a queen)
- **Player C:** Two Aces (in a tough spot, they assume Player B has at least one queen, so bluffing with a high card ace might discourage Player A from checking)
- **Player A:** Big Straight (opts for a straight to minimize risk)
- **Player B:** Check (not confident in higher hands)

All players reveal their cards. The Big Straight is incomplete (missing 10 and K). Player A loses, gaining additional cards. The round ends.

## Tables

### Initial Number of Cards

| Number of Players | Cards per Player |
|-------------------|------------------|
| 2                 | 3                |
| 3                 | 2                |
| 4                 | 2                |
| 5 - 10            | 1                |

### Maximum Number of Cards in Hand
Maximum values can be calculated using the formula: `min(6, ⌊ 23 / N ⌋)`, where `N` is the number of players. This prevents from situations where all cards all dealt and the biggest hand type is known.

| Players in Game | Max Cards in Hand |
|-----------------|-------------------|
| 2               | 6 (or optionally 7)|
| 3               | 6                 |
| 4               | 5                 |
| 5               | 4                 |
| 6               | 3                 |
| 7               | 3                 |
| 8 - 10          | 2                 |

### Hand Rankings in Thai Poker

Note the unusual ordering:
- Three of a Kind over Straight
- Flush over Full House

| Hand Type       | Hierarchy     | Example                  |
|------------------|---------------|--------------------------|
| Royal Flush      | Suit Order    | ♠A, ♠K, ♠Q, ♠J, ♠10      |
| Straight Flush   | Suit Order    | ♥K, ♥Q, ♥J, ♥10, ♥9      |
| Four of a Kind   | By Value      | ♣10, ♦10, ♥10, ♠10       |
| Flush            | Suit Order    | ♣A, ♣Q, ♣J, ♣10, ♣9      |
| Full House       | Triplet First | ♠K, ♣K, ♦K, ♥10, ♦10     |
| Three of a Kind  | By Value      | ♠Q, ♥Q, ♣Q               |
| Straight         | Big beats Small| ♣K, ♦Q, ♥J, ♠10, ♣9      |
| Pair             | By Value      | ♥Q, ♦Q                   |
| High Card        | By Value      | ♣9                       |

### Suits Order
| Suit |
|------|
| Spades (♠) |
| Hearts (♥) |
| Diamonds (♦) |
| Clubs (♣) |

# Probability Tool for Thai Poker Hands

## Problem Statement
A player may ask for the probability that a given hand type appears on the board (with a specific suit or value), given their hand and the number of dealt cards. 

The objective is to create a black-box solution that can have a relatively long precomputation phase, but should be able to respond to queries in **O(1)** time.

## Solution Overview

To solve this problem, we proceed as follows:

1. **Precompute the Probabilities**:
   - We solve the problem separately for each hand type and hand size, storing the results in a large array.
   - Each hand is represented as a **24-bit vector**, where each bit is set if the corresponding card is present.
   - We determine which sets of cards form the required hand type by simply checking the bit vector (see the `check.h` file for details).

2. **Understanding the Bitmask**:
   - Given a bitmask `hand` and `card_nb` representing the total number of cards in play, we need to determine how many valid bitmasks (supersets of `hand`) have exactly `card_nb` bits set and also contain the bits set in `hand`.
   - The solution leverages **OR convolution** (also known as **Sum Over Subsets DP**).

3. **Mathematical Formulation**:
   - Define a function `H[cards]`:
     - `H[cards] = 1` if `cards` represents a valid hand type and contains exactly `card_nb` set bits.
     - `H[cards] = 0` otherwise.
   - We want to compute `P[cards]`, defined as:
     - `P[cards] = sum of H[cards2]` where `cards2` is a superset of `cards` (i.e., `cards2 & cards = cards`). 
     - `P[cards]` tells us how many possible supersets of `cards` are valid.

4. **Calculating the Probability**:
   - Using the precomputed `P[hand]`, we can determine the probability by dividing `P[hand]` by the combination `C(24 - size(hand), card_nb - size(hand))`.
   - This approach gives us the desired answer efficiently in **O(1)** time.

5. **Complexity**:
   - The precomputation involves creating a full array of **190,051 possible hands** (calculated as `C(24, 6) + C(24, 5) + ... + C(24, 0)`) with **24 possible card number** and **69 hand types** , which can be accessed in **O(1)** (it's about 1.1GB)
   - The solution is implemented in the `luck.h` file (note that `H` is named `_P` because it's getting transformed into `P`).
   - The preprocessing involves about:
     ```
     2^24 * 24 (from computing P) * 69 (total hand types) * 24 (possible deck size)
     ```
     - This amounts to roughly **6 × 10^11 operations**, which completes in about **10 minutes** using the **SOS DP technique**.

## Files
- **`check.h`**: Contains functions to determine whether bit vector has given hand type.
- **`luck.h`**: Implements the core solution, precomputing probabilities and providing `P` table.
- **`demo.h`**: For testing `check.h` purposes
- **`utils.h`**: Just `pop_count` function to find the number of set bits.
- **`main.cpp`**: Testing `P` values with the bruteforce approach.

