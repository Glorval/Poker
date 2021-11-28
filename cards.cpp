#include "cards.h"


//deck functions
deck::deck() {
	srand((unsigned int)time(NULL));
	deckLength = 52;
	topCard = 0;
	cards = (Cards*)gcalloc(deckLength, sizeof(Cards));
	for (int level = two; level <= ace; level++) {
		for (int suit = diamonds; suit <= spades; suit++) {
			cards[(level * 4) + suit].name = setName(suit, level);
			cards[(level * 4) + suit].data = (unsigned char*)gcalloc(2, sizeof(char));
			cards[(level * 4) + suit].data[0] = (unsigned char)suit;
			cards[(level * 4) + suit].data[1] = (unsigned char)level;
		}
	}
}

deck::~deck() {
	for (int cCard = 0; cCard < deckLength; cCard++) {
		gfree(cards[cCard].name);
		gfree(cards[cCard].data);
	}
	gfree(cards);
}

void deck::shuffle() {
	//just use a permutation of the deck rather than
	//selecting two random positions and swapping over and over
	//a ton of times and hoping that it's good
	srand((unsigned int)time(NULL));
	topCard = 0;
	for (int c = deckLength - 1; c >= 1; c--) {
		Cards temp = cards[c];
		int pos = rand() % c;
		cards[c] = cards[pos];
		cards[pos] = temp;
	}
}
void deck::printDeck() {
	for (int c = 0; c < deckLength; c++) {
		std::cout << cards[c].name << "\n";
	}
}
Cards deck::drawCard() {
	topCard++;
	if (topCard >= deckLength) {
		topCard = deckLength - 1;
		return(cards[topCard]);
	} else {
		Cards returncard = cards[topCard - 1];
		return(returncard);
	}
}
Cards* deck::drawCards(int count) {
	Cards* returnCards = (Cards*)gcalloc(count, sizeof(Cards));
	for (int cCard = topCard; cCard < count + topCard; cCard++) {
		returnCards[cCard - topCard] = cards[cCard];
	}
	topCard += count;
	return(returnCards);
}
Cards deck::peek() {
	return(cards[topCard]);
}
Cards deck::peek(int position) {
	return(cards[position]);
}
char* deck::setName(int suit, int level) {
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





//Player functions
player::player(Cards* givenHand) {
	int cardcount = pSize(givenHand) / sizeof(Cards);
	handsize = cardcount;
	hand = (Cards*)gcalloc(cardcount, sizeof(Cards));
	for (int cCard = 0; cCard < cardcount; cCard++) {
		hand[cCard].data = givenHand[cCard].data;
		hand[cCard].name = givenHand[cCard].name;
	}
	gfree(givenHand);
}
player::~player() {
	if (hand != NULL) {
		gfree(hand);
	}
}

void player::printHand() {
	for (int cCard = 0; cCard < handsize; cCard++) {
		std::cout << hand[cCard].name << '\n';
	}
}

void player::printHand(int noNewlines) {
	for (int cCard = 0; cCard < handsize; cCard++) {
		std::cout << hand[cCard].name;
		if (cCard < handsize - 1) {
			std::cout << ", ";
		}
	}
}

int player::getHandsize() {
	return(handsize);
}
