#include "Player.h"
#include "cards.h"
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

//  Constructors 
Player::Player()
    : playerName("Player"), balance(1000)
{
    for (int i = 0; i < 5; ++i)
        community[i] = { Suit::DIAMONDS, value::ACE };
}

Player::Player(const std::string& name)
    : playerName(name), balance(1000)
{
}

//  Community Cards 
void Player::communityCards()
{
    std::vector<Card> deck;

    for (int s = 0; s < static_cast<int>(Suit::SUIT_COUNT); ++s)
        for (int v = 2; v <= 14; ++v)
            deck.push_back(Card(static_cast<Suit>(s), static_cast<value>(v)));

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);

    for (int j = 0; j < 5; ++j)
        community[j] = deck[j];
}

void Player::displayCommunityCards(int count) const
{
    std::cout << "Community cards: ";
    for (int j = 0; j < count; ++j)
    {
        int valueIndex = static_cast<int>(community[j].val) - 2;
        int suitIndex = static_cast<int>(community[j].suit);

        if (valueIndex >= 0 && valueIndex < static_cast<int>(valueNum.size()) &&
            suitIndex >= 0 && suitIndex < static_cast<int>(suitNames.size()))
        {
            std::cout << valueNum[valueIndex] << suitNames[suitIndex] << " ";
        }
        else
            std::cout << "[Invalid] ";
    }
    std::cout << std::endl;
}

//  Player Cards 
std::string Player::playerCards()
{
    for (int i = 0; i < 2; ++i) {
        Suit randomSuit = static_cast<Suit>(rand() % 4);
        value randomValue = static_cast<value>((rand() % 13) + 2);
        cards[i] = { randomSuit, randomValue };
    }
    return "Cards assigned";
}

void Player::displayCards() const
{
    std::cout << "Your hole cards: ";
    for (int i = 0; i < 2; ++i) {
        int valueIndex = static_cast<int>(cards[i].val) - 2;
        std::cout << valueNum[valueIndex] << suitNames[static_cast<int>(cards[i].suit)] << " ";
    }
    std::cout << std::endl;
}

//  Balance / Pot 
void Player::pot(int initialAmount) { balance = initialAmount; }

void Player::addPot(int amount) { balance += amount; }

void Player::subtractPot(int amount) { balance -= amount; }

int Player::getBalance() const { return balance; }

void Player::displayBalance() const {
    std::cout << playerName << "'s current balance: $" << balance << std::endl;
}

//  Hand Evaluation 
void Player::evaluateHand() const
{
    std::vector<Card> allCards(cards, cards + 2);
    allCards.insert(allCards.end(), community, community + 5);

    int valueCount[15] = { 0 };
    int suitCount[4] = { 0 };

    for (const auto& c : allCards) {
        valueCount[static_cast<int>(c.val)]++;
        suitCount[static_cast<int>(c.suit)]++;
    }

    int flushSuit = -1;
    for (int i = 0; i < 4; ++i)
        if (suitCount[i] >= 5) { flushSuit = i; break; }

    // Ace can be low for straights
    if (valueCount[14] > 0) valueCount[1] = valueCount[14];

    // Straight detection
    int straightHigh = -1, consecutive = 0;
    for (int val = 1; val <= 14; ++val) {
        if (valueCount[val] > 0) { consecutive++; if (consecutive >= 5) straightHigh = val; }
        else consecutive = 0;
    }

    // Straight flush detection
    std::vector<int> flushValues;
    if (flushSuit != -1)
        for (const auto& c : allCards)
            if (static_cast<int>(c.suit) == flushSuit) flushValues.push_back(static_cast<int>(c.val));

    bool hasStraightFlush = false;
    std::sort(flushValues.begin(), flushValues.end());
    flushValues.erase(std::unique(flushValues.begin(), flushValues.end()), flushValues.end());

    int sfConsec = 1;
    for (size_t i = 1; i < flushValues.size(); ++i) {
        if (flushValues[i] == flushValues[i - 1] + 1) { sfConsec++; if (sfConsec >= 5) { hasStraightFlush = true; break; } }
        else if (flushValues[i] != flushValues[i - 1]) sfConsec = 1;
    }

    // Count multiples
    bool hasFour = false, hasThree = false; int pairCount = 0;
    for (int val = 2; val <= 14; ++val) {
        if (valueCount[val] == 4) hasFour = true;
        if (valueCount[val] == 3) hasThree = true;
        if (valueCount[val] == 2) pairCount++;
    }

    // Ranking output
    if (hasStraightFlush) std::cout << "Straight Flush\n";
    else if (hasFour) std::cout << "Four of a Kind\n";
    else if (hasThree && pairCount >= 1) std::cout << "Full House\n";
    else if (flushSuit != -1) std::cout << "Flush\n";
    else if (straightHigh != -1) std::cout << "Straight\n";
    else if (hasThree) std::cout << "Three of a Kind\n";
    else if (pairCount >= 2) std::cout << "Two Pair\n";
    else if (pairCount == 1) std::cout << "One Pair\n";
    else {
        for (int val = 14; val >= 2; --val) if (valueCount[val] > 0) { std::cout << "High Card: " << valueNum[val - 2] << "\n"; break; }
    }
}

// ==================== Hand Strength ====================
double Player::calculateHandStrength(const std::vector<Card>& communityCards) const
{
    std::vector<Card> allCards(cards, cards + 2);
    allCards.insert(allCards.end(), communityCards.begin(), communityCards.end());

    int bestRank = 0;

    for (size_t i = 0; i < allCards.size(); ++i)
        for (size_t j = i + 1; j < allCards.size(); ++j)
            for (size_t k = j + 1; k < allCards.size(); ++k)
                for (size_t l = k + 1; l < allCards.size(); ++l)
                    for (size_t m = l + 1; m < allCards.size(); ++m) {
                        std::vector<Card> hand = { allCards[i], allCards[j], allCards[k], allCards[l], allCards[m] };
                        int rank = evaluateHand(hand);
                        if (rank > bestRank) bestRank = rank;
                    }

    return static_cast<double>(bestRank) / 8000.0; 
}

int Player::evaluateHand(const std::vector<Card>& hand) const
{
    std::map<value, int> valueCount;
    std::map<Suit, int> suitCount;
    std::set<int> uniqueValues;

    for (const Card& c : hand) {
        valueCount[c.val]++;
        suitCount[c.suit]++;
        uniqueValues.insert(static_cast<int>(c.val));
    }

    std::vector<int> sortedValues(uniqueValues.begin(), uniqueValues.end());
    std::sort(sortedValues.begin(), sortedValues.end());

    // Flush
    bool isFlush = false; Suit flushSuit;
    for (auto& p : suitCount) if (p.second >= 5) { isFlush = true; flushSuit = p.first; break; }

    // Straight
    bool isStraight = false; int highStraight = 0; int consec = 1;
    for (size_t i = 1; i < sortedValues.size(); ++i) {
        if (sortedValues[i] == sortedValues[i - 1] + 1) { consec++; if (consec >= 5) { isStraight = true; highStraight = sortedValues[i]; } }
        else consec = 1;
    }

    // Straight flush
    if (isFlush) {
        std::vector<int> flushValues;
        for (const Card& c : hand) if (c.suit == flushSuit) flushValues.push_back(static_cast<int>(c.val));
        std::sort(flushValues.begin(), flushValues.end());
        flushValues.erase(std::unique(flushValues.begin(), flushValues.end()), flushValues.end());

        int fsCount = 1;
        for (size_t i = 1; i < flushValues.size(); ++i) {
            if (flushValues[i] == flushValues[i - 1] + 1) { fsCount++; if (fsCount >= 5) return 8000 + flushValues[i]; }
            else fsCount = 1;
        }
    }

    // Four of a Kind
    for (auto& p : valueCount) if (p.second == 4) return 7000 + static_cast<int>(p.first);

    // Full House
    int three = -1, two = -1;
    for (auto& p : valueCount) if (p.second == 3 && (three == -1 || p.first > three)) three = static_cast<int>(p.first);
    for (auto& p : valueCount) if (p.second >= 2 && static_cast<int>(p.first) != three && (two == -1 || p.first > two)) two = static_cast<int>(p.first);
    if (three != -1 && two != -1) return 6000 + three * 100 + two;

    // Flush
    if (isFlush) { int highest = 0; for (auto& c : hand) if (c.suit == flushSuit) highest = std::max(highest, static_cast<int>(c.val)); return 5000 + highest; }

    // Straight
    if (isStraight) return 4000 + highStraight;

    // Three of a Kind
    for (auto& p : valueCount) if (p.second == 3) return 3000 + static_cast<int>(p.first);

    // Two Pair
    std::vector<int> pairs;
    for (auto& p : valueCount) if (p.second == 2) pairs.push_back(static_cast<int>(p.first));
    if (pairs.size() >= 2) { std::sort(pairs.begin(), pairs.end(), std::greater<int>()); return 2000 + pairs[0] * 100 + pairs[1]; }

    // One Pair
    if (pairs.size() == 1) return 1000 + pairs[0];

    // High Card
    return *std::max_element(sortedValues.begin(), sortedValues.end());
}

// ==================== Name Access ====================
std::string Player::getName() const { return playerName; }

// ==================== Output Overload ====================
std::ostream& operator<<(std::ostream& os, Player& player) {
    os << player.playerCards();
    return os;
}
