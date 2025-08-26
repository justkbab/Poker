#pragma once
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <random>    
#include <algorithm> 

enum Suit { DIAMONDS, SPADES, CLUBS, HEART, SUIT_COUNT };
enum value { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, VALUE_COUNT
};

const std::vector<std::string> suitNames = {
	"D ", "S ", "C ", "H ",
};

const std::vector<std::string> valueNum = {
	"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

struct Card {
	Suit suit;
	value val;

	Card() : suit(Suit::DIAMONDS), val(value::ACE) {} 

	Card(Suit s, value v) : suit(s), val(v) {}
};

inline std::string cardTostring(const Card& card) {
	size_t valueIndex = static_cast<size_t>(card.val) - 1; 
	size_t suitIndex = static_cast<size_t>(card.suit);

	return valueNum[valueIndex] + suitNames[suitIndex];
}
