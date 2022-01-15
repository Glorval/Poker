#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "GlorwynMemory.h"
enum suits { diamonds, hearts, clubs, spades };
enum levels {two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace};
#define suitPos 0
#define levelPos 1

struct card {
	char* name;
	unsigned char* data;//0 is suit, 1 is level
};
typedef struct card Cards;

class deck {
	public:
		deck();
		~deck();

		void shuffle();

		void printDeck();

		Cards drawCard();
		Cards* drawCards(int count);

		Cards peek();
		Cards peek(int position);

	private:
		Cards* cards;
		int deckLength;
		int topCard;
		char* setName(int suit, int level);

};
class player {
	public:
		Cards* hand;
		std::string name;

		player(Cards* givenHand);
		~player();

		void printHand();
		void printHand(int noNewlines);

		int getHandsize();

	protected:
		int handsize;
};

//should mostly be used for debug...
void printCards(Cards* hand, int count);
void printCardsNNL(Cards* hand, int count);
char* setName(int suit, int level);