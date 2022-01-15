#pragma once
#include "cards.h"
#include <conio.h>
#define pokerHandSize 5
#define startingMoney 500
#define call 0
#define fold -1
#define startingBet 5
#define DEBUG 1

//enum handTypes { RoyalFlush, StraightFlush, FourKind, FullHouse, Flush, Straight, ThreeKind, TwoPair, TwoKind };
int qsortCompDesecndingFirstByte(const void* a, const void* b);
int qsortCompDesecndingCardsLevel(const void* a, const void* b);


struct handpotential {
	char difficulty;//0-20 on how hard it is to achieve the new hand (Zero means we HAVE it)
	char neededToSwitch[5];//If 1, is a card to drop
	char potentialHandValue;//If we were to get this new hand, how good would it be?
	char failureBad;//Does failing to get the new hand cause us to lose what we've already got?
	char highestInHand;//For use if whole
};
typedef struct handpotential handPotential;

struct handvalue {
	char highestCard;//General highcard of the hand
	char handType;
	char handTypeHighest;//Highcard within the type, AKA a pair of kings is a king here,
	//a straight starting at 9 and going down would be a 9.
	char necessaryCards[5];
	handPotential* potentialHands;
};
typedef struct handvalue handValue;

struct checkerreturn {
	char uselessCards[5];
	char handValue;
	char highestInHand;
	char isPairKind;//used for the pairs to specify if it is in fact
};
typedef struct checkerreturn checkReturn;

struct carddiscard {
	char uselessCards[5];
};
typedef struct carddiscard cardDiscards;

class pokerPlayer : public player {
	public: 
		pokerPlayer(Cards* givenHand, int givenmoney);
		pokerPlayer(Cards* givenHand, int givenmoney, int sorted);
		virtual int taketurn(int curBet, int stage) { return(-1); }
		virtual int takeNormalTurn(int curBet, int someoneCalled) { return(-1); }
		virtual cardDiscards takeDiscardTurn() { return(*(cardDiscards*)&"-1"); }
		void evaluateHand();
		void sortOurHand();
		int ourBet;
		int money;
		handValue ourHandValue;
};

class humanPlayer : public pokerPlayer {
	public:
		humanPlayer(Cards* givenHand, int givenmoney);

		int taketurn(int curBet, int stage) override;
		int takeNormalTurn(int curBet, int someoneCalled) override;
		cardDiscards takeDiscardTurn() override;

	private:
		std::string userInput;
};

class aiPlayer : public pokerPlayer {
	public:
		
		aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName);

		void initializeMaxBet();

		int taketurn(int curBet, int stage) override;

		int takeNormalTurn(int curBet, int someoneCalled) override;
		cardDiscards takeDiscardTurn() override;

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










handValue findHandValue(Cards* hand);

int setPotential(char* data, handPotential* potentialHands, int* handcounts, handValue* actualHand, int difficulties[5], int marginOfError);
handValue findValue(Cards* sortedHand);
checkReturn checkStraight(int* levels);
checkReturn checkFlush(int* suits);
checkReturn checkPairs(int* levels);