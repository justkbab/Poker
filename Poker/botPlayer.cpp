#include "botPlayer.h"
#include "cards.h"
#include "Player.h"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

enum class HandRank {
    HIGH_CARD = 0,
    ONE_PAIR = 1000,
    TWO_PAIR = 2000,
    THREE_OF_A_KIND = 3000,
    STRAIGHT = 4000,
    FLUSH = 5000,
    FULL_HOUSE = 6000,
    FOUR_OF_A_KIND = 7000,
	STRAIGHT_FLUSH = 8000

};


struct HandStrength {
    HandRank rank;
    std::vector<int> kickers; // sorted descending for tie-breakers
};



// Constructors
botPlayer::botPlayer() : Player() {}

botPlayer::botPlayer(const std::string& name) : Player(name) {}

// Calculate hand strength (0.0 - 1.0)
double botPlayer::calculateHandStrength(const std::vector<Card>& communityCards) const {
    // Combine hole cards and community cards
    std::vector<Card> allCards = communityCards;
    allCards.insert(allCards.end(), cards, cards + 2);

    int bestRank = 0;

    // Check all 5-card combinations
    for (size_t i = 0; i < allCards.size(); ++i)
        for (size_t j = i + 1; j < allCards.size(); ++j)
            for (size_t k = j + 1; k < allCards.size(); ++k)
                for (size_t l = k + 1; l < allCards.size(); ++l)
                    for (size_t m = l + 1; m < allCards.size(); ++m) {
                        std::vector<Card> hand = { allCards[i], allCards[j], allCards[k], allCards[l], allCards[m] };
                        int rank = evaluateHand(hand);
                        if (rank > bestRank) bestRank = rank;
                    }

    return static_cast<double>(bestRank) / 8000.0; // Normalize
}



// Evaluate hand (returns integer rank)
int botPlayer::evaluateHand(const std::vector<Card>& hand) const {
    std::map<value, int> valueCount;
    std::map<Suit, int> suitCount;
    std::vector<int> values;

    for (const Card& card : hand) {
        valueCount[card.val]++;
        suitCount[card.suit]++;
        values.push_back(static_cast<int>(card.val));
    }

    std::sort(values.begin(), values.end(), std::greater<int>());

    // Flush detection
    Suit flushSuit;
    bool isFlush = false;
    for (auto& p : suitCount) {
        if (p.second >= 5) { flushSuit = p.first; isFlush = true; break; }
    }

    // Collect flush values
    std::vector<int> flushValues;
    if (isFlush) {
        for (const Card& c : hand) {
            if (c.suit == flushSuit)
                flushValues.push_back(static_cast<int>(c.val));
        }
        std::sort(flushValues.begin(), flushValues.end(), std::greater<int>());
        flushValues.erase(std::unique(flushValues.begin(), flushValues.end()), flushValues.end());
    }

    // Straight detection (with A-2-3-4-5)
    std::vector<int> uniqueValues = values;
    uniqueValues.erase(std::unique(uniqueValues.begin(), uniqueValues.end()), uniqueValues.end());

    bool isStraight = false;
    int straightHigh = 0;

    // Add Ace-low option (Ace counts as 1)
    if (std::find(uniqueValues.begin(), uniqueValues.end(), 14) != uniqueValues.end())
        uniqueValues.push_back(1);

    std::sort(uniqueValues.begin(), uniqueValues.end());

    int consecutive = 1;
    for (size_t i = 1; i < uniqueValues.size(); i++) {
        if (uniqueValues[i] == uniqueValues[i - 1] + 1) {
            consecutive++;
            if (consecutive >= 5) {
                isStraight = true;
                straightHigh = uniqueValues[i];
            }
        }
        else {
            consecutive = 1;
        }
    }

    // Straight Flush
    if (isFlush && flushValues.size() >= 5) {
        int consec = 1;
        for (size_t i = 1; i < flushValues.size(); i++) {
            if (flushValues[i - 1] == flushValues[i] + 1) {
                consec++;
                if (consec >= 5) {
                    return static_cast<int>(HandRank::STRAIGHT_FLUSH) + flushValues[i - 4];
                }
            }
            else {
                consec = 1;
            }
        }
    }

    // Four of a Kind
    for (auto& p : valueCount) {
        if (p.second == 4) {
            return static_cast<int>(HandRank::FOUR_OF_A_KIND) + static_cast<int>(p.first);
        }
    }

    // Full House
    int three = -1, two = -1;
    for (auto& p : valueCount) {
        if (p.second >= 3 && p.first > three) three = static_cast<int>(p.first);
    }
    for (auto& p : valueCount) {
        if (p.second >= 2 && static_cast<int>(p.first) != three && p.first > two) two = static_cast<int>(p.first);
    }
    if (three != -1 && two != -1) {
        return static_cast<int>(HandRank::FULL_HOUSE) + three * 15 + two;
    }

    // Flush
    if (isFlush) {
        return static_cast<int>(HandRank::FLUSH) + flushValues[0];
    }

    // Straight
    if (isStraight) {
        return static_cast<int>(HandRank::STRAIGHT) + straightHigh;
    }

    // Three of a Kind
    for (auto& p : valueCount) {
        if (p.second == 3) {
            return static_cast<int>(HandRank::THREE_OF_A_KIND) + static_cast<int>(p.first);
        }
    }

    // Two Pair
    std::vector<int> pairs;
    for (auto& p : valueCount) {
        if (p.second == 2) pairs.push_back(static_cast<int>(p.first));
    }
    if (pairs.size() >= 2) {
        std::sort(pairs.begin(), pairs.end(), std::greater<int>());
        return static_cast<int>(HandRank::TWO_PAIR) + pairs[0] * 15 + pairs[1];
    }

    // ==========================
    // One Pair
    // ==========================
    if (pairs.size() == 1) {
        return static_cast<int>(HandRank::ONE_PAIR) + pairs[0];
    }

    // ==========================
    // High Card
    // ==========================
    return static_cast<int>(HandRank::HIGH_CARD) + values[0];
}

// Decision logging
void botPlayer::logDecision(Action action, double handStrength, const std::vector<Card>& communityCards, int potSize, int betAmount) const {
    std::cout << "== BOT DECISION ==" << std::endl;
    std::cout << "Action: ";
    switch (action) {
    case Action::FOLD: std::cout << "Fold"; break;
    case Action::CHECK: std::cout << "Check"; break;
    case Action::CALL: std::cout << "Call"; break;
    case Action::BET: std::cout << "Bet"; break;
    case Action::RAISE: std::cout << "Raise"; break;
    case Action::ALL_IN: std::cout << "All In"; break;
    }
    std::cout << "\nHand Strength: " << handStrength << "\nHole Cards: ";
    for (int i = 0; i < 2; ++i) std::cout << cardTostring(cards[i]) << " ";
    std::cout << "\nCommunity Cards: ";
    for (const auto& c : communityCards) std::cout << cardTostring(c) << " ";
    std::cout << "\nPot Size: $" << potSize << "\nBet Amount: $" << betAmount << "\n====================" << std::endl;
}

// Decide action based on hand strength
Action botPlayer::decideAction(const std::vector<Card>& communityCards, int currentBet, int potSize, int minRaiseAmount, const std::vector<int>& opponentChips) {
    double handStrength = calculateHandStrength(communityCards);
    Action action;
    if (handStrength < 0.2) action = Action::FOLD;
    else if (handStrength < 0.5) action = (currentBet == 0) ? Action::CHECK : Action::CALL;
    else if (handStrength < 0.8) action = (currentBet == 0) ? Action::BET : Action::CALL;
    else action = (currentBet == 0) ? Action::BET : Action::RAISE;

    logDecision(action, handStrength, communityCards, potSize, currentBet);
    return action;
}

// Utility functions
void botPlayer::getHand() const {
    std::cout << "Bot's Hand: "; for (int i = 0; i < 2; ++i) std::cout << cardTostring(cards[i]) << " "; std::cout << std::endl;
}

void botPlayer::addChips(int amount) {
    if (amount > 0) { chips += amount; std::cout << "Bot added " << amount << " chips. Total: " << chips << std::endl; }
    else std::cout << "Cannot add negative or zero chips." << std::endl;
}

void botPlayer::removeChips(int amount) {
    if (amount > 0) { chips -= amount; std::cout << "Bot removed " << amount << " chips. Total: " << chips << std::endl; }
    else std::cout << "Cannot remove negative or zero chips." << std::endl;
}

void botPlayer::receiveCards(const Card& card1, const Card& card2) {
    cards[0] = card1; cards[1] = card2;
}
