#pragma once
#include "cards.h"
#include <conio.h>
#define pokerHandSize 5
#define startingMoney 500
#define call 0
#define fold -1
#define startingBet 5

double calcHandValue(card* hand, int cardCount);

class pokerPlayer : public player {
	public: 
		pokerPlayer(Cards* givenHand, int givenmoney);
		virtual int taketurn(int curBet, int stage) = 0;
		int ourBet;
		int money;
};

//note - demonstates polymorphism because both have a 'take turn' function
//and are all treated as a 'player' object later on when driving code.
class humanPlayer : public pokerPlayer {
	public:
		humanPlayer(Cards* givenHand, int givenmoney);

		int taketurn(int curBet, int stage) override;

	private:
		std::string userInput;
};

class aiPlayer : public pokerPlayer {
	public:
		
		aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName);

		void initializeMaxBet();

		int taketurn(int curBet, int stage);

	private:
		int maxBet;
		int aggressiveness;//0-100	
};

class poker {
	public:

		poker();

		int checkContinuePlay(int flag);

		void endOfRound(int winner);

		void playRound();

		void playGame();


	private:
		deck* ourDeck;

		int currentBet;
		int stageOfGame;

		int currentPlayer;
		int playercount;
		pokerPlayer** players;
};






struct handvalue {
	int highestCard;
	int handType;
	int handTypeHighest;
};
typedef struct handvalue handValue;

handValue findHandValue(Cards* hand);

int sameSuit(card* hand, int* cardCount);
int inlineCards(card* hand, int* cardCount);
int sameLevel(card* hand, int* cardCount);
int royalFlush(card* hand, int* cardCount);
int royalFlush(card* hand, int* cardCount, int alreadySet);
int straightFlush(card* hand, int* cardCount);
int fourkind(card* hand, int* cardCount);
int fullHouse(card* hand, int* cardCount);
int flush(card* hand, int* cardCount);
int straight(card* hand, int* cardCount);
int threekind(card* hand, int* cardCount);
int twopair(card* hand, int* cardCount);
int onepair(card* hand, int* cardCount);