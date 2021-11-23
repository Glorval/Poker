#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "GlorwynMemory.h"
enum suits { diamonds, hearts, clubs, spades };
enum levels {
	two, three, four, five, six, seven, eight, nine, ten, jack, queen,
	king, ace
};
struct card {
	char* name;
	unsigned char* data;
};
typedef struct card Cards;
//Note- unless otherwise specified, all behaviour is designed
//around the deck not having its data changed 
class deck {
public:
	//default constructor is to make a standard 52
	//card deck. Could use inheritance to make other types
	//of decks, but really the only major reason for that I
	//can see is to make them more interchangable.
	deck() {
		srand(time(NULL));
		deckLength = 52;
		topCard = 0;
		cards = (Cards*)gcalloc(deckLength, sizeof(Cards));
		for (int level = two; level <= ace; level++) {
			for (int suit = diamonds; suit <= spades; suit++) {
				cards[(level * 4) + suit].name = setName(suit,
					level);
				cards[(level * 4) + suit].data = (unsigned char*)
					gcalloc(2, sizeof(char));
				cards[(level * 4) + suit].data[0] = (unsigned
					char)suit;
				cards[(level * 4) + suit].data[1] = (unsigned
					char)level;
			}
		}
	}
	~deck() {
		for (int cCard = 0; cCard < deckLength; cCard++) {
			gfree(cards[cCard].name);
			gfree(cards[cCard].data);
		}
		gfree(cards);
	}
	void shuffle() {
		//just use a permutation of the deck rather than
		//selecting two random positions and swapping over and over
		//a ton of times and hoping that it's good
		srand(time(NULL));
		topCard = 0;
		for (int c = deckLength - 1; c >= 1; c--) {
			Cards temp = cards[c];
			int pos = rand() % c;
			cards[c] = cards[pos];
			cards[pos] = temp;
		}
	}
	void printDeck() {
		for (int c = 0; c < deckLength; c++) {
			std::cout << cards[c].name << "\n";
		}
	}
	Cards drawCard() {
		topCard++;
		if (topCard >= deckLength) {
			topCard = deckLength - 1;
		} else {
			Cards returncard = cards[topCard - 1];
			return(returncard);
		}
	}
	Cards* drawCards(int count) {
		Cards* returnCards = (Cards*)gcalloc(count, sizeof(Cards));
		for (int cCard = topCard; cCard < count + topCard; cCard++) {
			returnCards[cCard - topCard] = cards[cCard];
		}
		topCard += count;
		return(returnCards);
	}
	Cards peek() {
		return(cards[topCard]);
	}
	Cards peek(int position) {
		return(cards[position]);
	}
private:
	Cards* cards;
	int deckLength;
	int topCard;
	char* setName(int suit, int level) {
		//char* name = (char*)calloc(1, sizeof(char));
		std::string name;
		switch (level) {
			case two:
				name.append("Two of ");
				break;
			case three:
				name.append("Three of ");
				break;
			case four:
				name.append("Four of ");
				break;
			case five:
				name.append("Five of ");
				break;
			case six:
				name.append("Six of ");
				break;
			case seven:
				name.append("Seven of ");
				break;
			case eight:
				name.append("Eight of ");
				break;
			case nine:
				name.append("Nine of ");
				break;
			case ten:
				name.append("Ten of ");
				break;
			case jack:
				name.append("Jack of ");
				break;
			case queen:
				name.append("Queen of ");
				break;
			case king:
				name.append("King of ");
				break;
			case ace:
				name.append("Ace of ");
				break;
		}
		switch (suit) {
			case diamonds:
				name.append("Diamonds");
				break;
			case hearts:
				name.append("Hearts");
				break;
			case spades:
				name.append("Spades");
				break;
			case clubs:
				name.append("Clubs");
				break;
		}
		char* returns = (char*)gcalloc(name.length() + 1, sizeof(char));
		memcpy(returns, name.c_str(), name.length() + 1);
		return(returns);
	}
};
class player {
public:
	Cards* hand;
	// things that I might have created a generic 'poker player' derivativeof this class for if I took the extra time for
	int ourBet;
	int money;
	std::string name;
	player() {
		hand = NULL;
		handsize = 0;
	}
	player(Cards* givenHand) {
		int cardcount = pSize(givenHand) / sizeof(Cards);
		handsize = cardcount;
		hand = (Cards*)gcalloc(cardcount, sizeof(Cards));
		for (int cCard = 0; cCard < cardcount; cCard++) {
			hand[cCard].data = givenHand[cCard].data;
			hand[cCard].name = givenHand[cCard].name;
			/*
			hand[cCard].data = (unsigned
			char*)gcalloc(pSize(givenHand[cCard].data), sizeof(unsigned char));
			memcpy(hand[cCard].data, givenHand[cCard].data,
			pSize(givenHand[cCard].data));
			hand[cCard].name =
			(char*)gcalloc(pSize(givenHand[cCard].name), sizeof(char));
			memcpy(hand[cCard].name, givenHand[cCard].name,
			pSize(givenHand[cCard].name));
			*/
		}
		gfree(givenHand);
	}
	~player() {
		if (hand != NULL) {
			//for (int cCard = 0; cCard < handsize; cCard++) {
			// gfree(hand[cCard].name);
			// gfree(hand[cCard].data);
			//}
			gfree(hand);
		}
	}
	void printHand() {
		for (int cCard = 0; cCard < handsize; cCard++) {
			std::cout << hand[cCard].name << '\n';
		}
	}
	void printHand(int noNewlines) {
		for (int cCard = 0; cCard < handsize; cCard++) {
			std::cout << hand[cCard].name;
			if (cCard < handsize - 1) {
				std::cout << ", ";
			}
		}
	}
	int getHandsize() {
		return(handsize);
	}
	//note - if I were to take some extra time, I'd likely have made another generic class, 'poker player', that derives
		//from this one and adds in the support for take turn there.
		//virtual void taketurn() = 0;
		virtual int taketurn(int curBet, int stage) {
		std::cout << "error";
		return(-1);
	}
protected:
	int handsize;
};