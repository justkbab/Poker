#include "cards.h"
#include <iostream>
#include <string>

static std::ostream& operator<<(std::ostream& os, Suit res) {
	switch (res) {
	case Suit::CLUBS: return os << "C";
	case Suit::DIAMONDS: return os << "D";
	case Suit::SPADES: return os << "S";
	case Suit::HEART: return os << "H";
	default: return  os << "Unknown Suit";
	}
}

static std::ostream& operator<<(std::ostream& os, value res) {
	switch (res) {
	case ACE: return os << "A";
	case TWO: return os << "2";
	case THREE: return os << "3";
	case FOUR: return os << "4";
	case FIVE: return os << "5";
	case SIX: return os << "6";
	case SEVEN: return os << "7";
	case EIGHT: return os << "8";
	case NINE: return os << "9";
	case TEN: return os << "10";
	case JACK: return os << "J";
	case QUEEN: return os << "Q";
	case KING: return os << "K";
	default: return  os << "Unknown VALUE";
	}
}
