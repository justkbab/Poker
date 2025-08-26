#pragma once

#include "Player.h"
#include "cards.h"
#include <iostream>
#include <vector>
#include <string>

enum class Action
{
    FOLD, CHECK, CALL, BET, RAISE, ALL_IN
};

class botPlayer : public Player {
private:

   
    int chips = 0;

    void logDecision(Action action, double handStrength, const std::vector<Card>& communityCards, int potSize, int betAmount) const;
    Card cards[2];
    std::string botName;

public:
    botPlayer();                 
    botPlayer(const std::string& name); 
    double calculateHandStrength(const std::vector<Card>& communityCards) const;
    int evaluateHand(const std::vector<Card>& hand) const;


    Action decideAction(const std::vector<Card>& communityCards, int currentBet, int potSize, int minRaiseAmount, const std::vector<int>& opponentChips);
    void getHand() const;
    void addChips(int amount);
    void removeChips(int amount);
    void receiveCards(const Card& card1, const Card& card2);
};
