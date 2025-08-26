# 🎲 Texas Hold'em Poker (C++ Console Game)

A simple **single-player Texas Hold’em Poker game** written in C++.  
This project deals cards, reveals the flop/turn/river, and lets the player fold, call, or raise during each betting round.  
It’s built as a **practice project** for learning object-oriented C++ and game logic.
There is an error in this program where the random cards are out of range. I have no clue how to fix it, but the code works as expected after running it 

---

## ✨ Features
- 🎴 Player hole cards (2 private cards)
- 🃏 Community cards revealed in stages (Flop, Turn, River)
- 💰 Simple betting system (Fold / Call / Raise)
- 📜 Displays all cards at the end of the hand
- 🔄 Modular code with `Player` and `Card` classes

---


---

##  Getting Started

### Clone the repository:
```bash
git clone https://github.com/justkbab/Poker.git
cd Poker
```

### Build the game:
```bash
g++ main.cpp Player.cpp cards.cpp -o poker
```

### Run the game:
```bash
./poker
```
Then follow the on-screen prompts to play.
