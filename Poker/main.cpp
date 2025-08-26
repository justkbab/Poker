#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include "cards.h"
#include "Player.h"
#include "botPlayer.h"

// Function to print community cards
void displayCommunity(const std::vector<Card>& community, int count) {
    for (int i = 0; i < count; ++i)
        std::cout << cardTostring(community[i]) << " ";
    std::cout << "\n";
}

// Betting round helper
bool bettingRound(Player& human, botPlayer& bot, int& pot, const std::string& stage) {
    std::string action;
    std::cout << "\n" << stage << ": Fold(1), Call(2), Raise(3): ";
    std::cin >> action;

    if (action == "1") {
        std::cout << "You folded. Bot wins.\n";
        return false;
    }

    int amount = 20; // default call amount
    if (action == "2") {
        human.subtractPot(amount);
        pot += amount * 2; // assume bot calls
        bot.removeChips(amount);
        std::cout << "You called " << amount << " chips.\n";
    }
    else if (action == "3") {
        int raiseAmount;
        std::cout << "Enter raise amount: ";
        std::cin >> raiseAmount;
        human.subtractPot(raiseAmount);
        pot += raiseAmount * 2; // assume bot matches raise
        bot.removeChips(raiseAmount);
        std::cout << "You raised " << raiseAmount << " chips.\n";
    }

    std::cout << "Pot size: " << pot << "\n";
    return true;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    std::cout << "=================================\n";
    std::cout << "         TEXAS HOLD'EM\n";
    std::cout << "=================================\n\n";

    std::string playerName;
    std::cout << "Enter your player name: ";
    std::getline(std::cin >> std::ws, playerName);

    Player human(playerName);
    botPlayer bot;

    int pot = 0;
    human.pot(1000);
    bot.addChips(1000);

    std::cout << human.getName() << " starts with ";
    human.displayBalance();
    std::cout << "Bot starts with 1000 chips.\n\n";

    // Create deck and shuffle
    std::vector<Card> deck;
    for (int s = 0; s < static_cast<int>(Suit::SUIT_COUNT); ++s)
        for (int v = 2; v <= 14; ++v)
            deck.emplace_back(static_cast<Suit>(s), static_cast<value>(v));
    std::random_shuffle(deck.begin(), deck.end());

    // Deal hole cards
    human.playerCards();
    bot.receiveCards(deck[2], deck[3]);
    deck.erase(deck.begin(), deck.begin() + 4);

    // Deal community cards
    std::vector<Card> community(deck.begin(), deck.begin() + 5);

    // Show player's cards
    std::cout << "Your hole cards: ";
    human.displayCards();

    // Optional: show bot cards for testing
    std::cout << "Bot's hole cards: ";
    bot.getHand();

    // Betting rounds
    if (!bettingRound(human, bot, pot, "Pre-flop")) return 0;

    std::cout << "\n=== FLOP ===\nCommunity Cards: ";
    displayCommunity(community, 3);
    if (!bettingRound(human, bot, pot, "Flop")) return 0;

    std::cout << "\n=== TURN ===\nCommunity Cards: ";
    displayCommunity(community, 4);
    if (!bettingRound(human, bot, pot, "Turn")) return 0;

    std::cout << "\n=== RIVER ===\nCommunity Cards: ";
    displayCommunity(community, 5);
    if (!bettingRound(human, bot, pot, "River")) return 0;

    // Evaluate hands
    double humanStrength = human.calculateHandStrength(community);
    double botStrength = bot.calculateHandStrength(community);

    std::cout << "\nYour hand strength: " << humanStrength << "\n";
    std::cout << "Bot hand strength: " << botStrength << "\n";

    // Determine winner
    if (humanStrength > botStrength) {
        std::cout << "You win the pot of " << pot << " chips!\n";
        human.addPot(pot);
    }
    else if (botStrength > humanStrength) {
        std::cout << "Bot wins the pot of " << pot << " chips.\n";
        bot.addChips(pot);
    }
    else {
        std::cout << "It's a tie! Pot is split.\n";
        human.addPot(pot / 2);
        bot.addChips(pot / 2);
    }

    human.displayBalance();
    std::cout << "Game over. Thanks for playing!\n";
    return 0;
}
