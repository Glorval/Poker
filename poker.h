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

double calcHandValue(card* hand, int cardCount);

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
	int difficulty;//0-20 on how hard it is to achieve the new hand (Zero means we HAVE it)
	char neededToSwitch[5];//If 1, is a card to drop
	char necessaryCards[5];//Used for returning a whole hand. If 1, we need this card to keep the hand as is.
	int potentialHandValue;//If we were to get this new hand, how good would it be?
	char failureBad;//Does failing to get the new hand cause us to lose what we've already got?
	int currentHighcard;//The highest card we have, for use if this hand is whole.
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
	char neededCards[5];
};
typedef struct checkerreturn checkReturn;

handValue findHandValue(Cards* hand);

handValue findValue(Cards* sortedHand);
checkReturn checkStraight(int* distances, int* suits);











//The standard procedure here is to return the 
//0 for failure, 1 for having found something.
//They won't set 'failureBad' internally ever.
//They all need to set 'difficulty' and 'potential hand value', 'needed to switch'is only for if the hand is not whole. Necessary Cards and current highcard are only for a whole hand.
int straight(Cards* sortedHand, handPotential* handpotential, int handsize);
int flush(Cards* sortedHand, handPotential* handpotential, int handsize);






//old
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