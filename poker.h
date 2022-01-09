#pragma once
#include "cards.h"
#include <conio.h>
#define pokerHandSize 5
#define startingMoney 500
#define call 0
#define fold -1
#define startingBet 5


int qsortCompDesecndingFirstByte(const void* a, const void* b);
int qsortCompDesecndingCardsLevel(const void* a, const void* b);


class pokerPlayer : public player {
	public: 
		pokerPlayer(Cards* givenHand, int givenmoney);
		pokerPlayer(Cards* givenHand, int givenmoney, int sorted);
		virtual int taketurn(int curBet, int stage) = 0;
		int ourBet;
		int money;
};

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

		int taketurn(int curBet, int stage) override;

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








struct handpotential {
	unsigned char difficulty;//0-20 on how hard it is to achieve the new hand (Zero means we HAVE it)
	unsigned char neededToSwitch[5];//If 1, is a card to drop
	unsigned char potentialHandValue;//If we were to get this new hand, how good would it be?
	unsigned char failureBad;//Does failing to get the new hand cause us to lose what we've already got?
	unsigned char highestInHand;//For use if whole
};
typedef struct handpotential handPotential;

struct handvalue {
	int highestCard;//General highcard of the hand
	int handType;
	int handTypeHighest;//Highcard within the type, AKA a pair of kings is a king here,
	//a straight starting at 9 and going down would be a 9.
	handPotential* potentialHands;
};
typedef struct handvalue handValue;

struct checkerreturn {
	char uselessCards[5];
	char handValue;
	char highestInHand;
};
typedef struct checkerreturn checkReturn;

handValue findHandValue(Cards* hand);

handValue findValue(Cards* sortedHand);
checkReturn checkStraight(int* levels, int* suits);
checkReturn checkFlush(int* suits);