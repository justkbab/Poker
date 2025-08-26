#pragma once
#include <string>
#include <vector>
#include <utility>  
#include <iostream>
#include "cards.h"   



class Player  
{  
protected:  
	std::string playerName;  
	int balance;  
	Card cards[2]; 
	Card community[5];
	

public:  
	Player();  
	Player(const std::string& name);  
	void communityCards();
	void displayCommunityCards(int count) const;  
	void pot(int initalAmount);  
	void displayBalance() const;  
	std::string playerCards();  
	void displayCards() const;  
	void evaluateHand() const;
	double calculateHandStrength(const std::vector<Card>& communityCards) const;
	int evaluateHand(const std::vector<Card>& hand) const;

	void subtractPot(int subtractAmount);  
	void addPot(int addAmount);  
	int getBalance() const;  
	std::string getName() const;  
};