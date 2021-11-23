#include "poker.h"















//this section is pain...
int straights = 0;
int flushes = 0;

double handValues[] = { 1000,		950,					900,			800,		600,		500,			 400,			300,		200 };
enum handTypes { RoyalFlush, StraightFlush, FourKind, FullHouse, Flush, Straight, ThreeKind, TwoPair, OnePair };
//								1000					800				700			500		300		250			100				75		50

double calcHandValue(card* hand, int cardCount) {
	char highcard = 0;
	for (int c = 0; c < cardCount; c++) {
		if (hand[c].data[1] > highcard) {
			highcard = hand[c].data[1];
		}
	}

	//todo - Find out why straights are so rare
	int isInline = inlineCards(hand, &cardCount);
	int isSameSuit = sameSuit(hand, &cardCount);
	if (isInline) {
		straights++;
	}
	if (isSameSuit) {
		flushes++;
	}
	//std::cout << straights << ", " << flushes << ". \n";
	if (isInline & isSameSuit) {
		if (royalFlush(hand, &cardCount, 1)) {
			return(handValues[RoyalFlush]);
		}
	}
	if (isInline && isSameSuit) {
		return(handValues[StraightFlush] + highcard);
	}
	if (isSameSuit) {
		return(handValues[Flush] + highcard);
	}
	if (isInline) {
		return(handValues[Straight] + highcard);
	}	
	if (fourkind(hand, &cardCount)) {
		return(handValues[FourKind] + highcard);
	}
	if (fullHouse(hand, &cardCount)) {
		return(handValues[FullHouse] + highcard);
	}
	if (threekind(hand, &cardCount)) {
		return(handValues[ThreeKind] + highcard);
	}
	if (twopair(hand, &cardCount)) {
		return(handValues[TwoPair] + highcard);
	}
	if (onepair(hand, &cardCount)) {
		return(handValues[OnePair] + highcard);
	}
	
	
	
	
	
	
	highcard *= 2;
	return(highcard);
}

int sameSuit(card* hand, int* cardCount) {
	for (int c = 1; c < *cardCount; c++) {
		if (hand[c].data[0] != hand[0].data[0]) {
			return(0);
		}
	}
	return(1);
}

int inlineCards(card* hand, int* cardCount) {
	if (hand[1].data[1] > hand[0].data[1]) {//ascending check
		for (int c = 0; c < *cardCount - 1; c++) {
			if (hand[c].data[1] != hand[c + 1].data[1] - (char)1) {
				return(0);
			}
		}
		return(1);
	}

	for (int c = 0; c < *cardCount - 1; c++) {//if we're not ascending we go down ofc.
		if (hand[c].data[1] != hand[c + 1].data[1] + (char)1) {
			return(0);
		}
	}

	return(1);
}

int sameLevel(card* hand, int* cardCount) {
	char key = hand[0].data[1];
	for (int c = 1; c < *cardCount; c++) {
		if (hand[c].data[1] != key) {
			return(0);
		}
	}
	return(1);
}

int royalFlush(card* hand, int* cardCount) {
	if (*cardCount < 5) {
		return(0);
	}

	if (!sameSuit(hand, cardCount) || !inlineCards(hand, cardCount)) {
		return(0);
	}

	return(royalFlush(hand, cardCount, 1));
}

int royalFlush(card* hand, int* cardCount, int alreadySet) {
	for (int c = 0; c < *cardCount; c++) {
		if (hand[c].data[1] == ace) {
			return(1);
		}
	}
	return(0);
}

int straightFlush(card* hand, int* cardCount) {
	if (sameSuit(hand, cardCount) && inlineCards(hand, cardCount)) {
		return(1);
	}
	return(0);
}

//note: Only officially supports 4/5 card hands
int fourkind(card* hand, int* cardCount) {
	if (*cardCount < 4) {
		return(0);
	}
	char key;
	if (hand[0].data[1] == hand[1].data[1] || hand[0].data[1] == hand[2].data[1]) {
		key = hand[0].data[1];
	} else if (hand[1].data[1] == hand[2].data[1]) {
		key = hand[1].data[1];
	} else {
		return(0);
	}
	int amount = 0;
	for (int c = 2; c < *cardCount; c++) {
		if (hand[c].data[1] != key) {
			amount++;
		}
	}
	if (amount > 1) {
		return(0);
	}
	return(1);
}

int fullHouse(card* hand, int* cardCount) {
	if (threekind(hand, cardCount) && onepair(hand, cardCount)) {
		return(1);
	}
	return(0);
}

int flush(card* hand, int* cardCount) {
	return(sameSuit(hand, cardCount));
}

int straight(card* hand, int* cardCount) {
	return(inlineCards(hand, cardCount));
}

int threekind(card* hand, int* cardCount) {
	if (*cardCount < 3) {
		return(0);
	}
	if (*cardCount == 3) {//if there's only three cards, we only need to check if they're all equal
		if (hand[0].data[1] == hand[1].data[1] && hand[0].data[1] == hand[2].data[1]) {
			return(1);
		}
	}
	char currentKey = -1;
	int currentCap = 0;
	for (int runthrough = 0; runthrough < *cardCount; runthrough++) {
		currentKey = hand[runthrough].data[1];
		for (int c = 0; c < *cardCount; c++) {
			if (hand[c].data[1] == currentKey) {
				currentCap++;
			}
		}
		if (currentCap == 3) {
			return(1);
		}
		currentCap = 0;
	}
	return(0);
}

int twopair(card* hand, int* cardCount) {
	if (*cardCount < 2) {
		return(0);
	}
	char currentKey = -1;
	int currentCap = 0;
	int paircount = 0;
	char* oldKeys = new char[*cardCount];
	int oldKeyCount = 0;
	int isInKeys = 0;
	for (int runthrough = 0; runthrough < *cardCount; runthrough++) {
		currentKey = hand[runthrough].data[1];
		for (int c = 0; c < oldKeyCount; c++) {
			if (currentKey == oldKeys[c]) {
				isInKeys = 1;
				break;
			}
		}
		if (!isInKeys) {
			oldKeys[oldKeyCount] = currentKey;
			oldKeyCount++;
		} else {
			isInKeys = 0;
			continue;
		}
		for (int c = 0; c < *cardCount; c++) {
			if (hand[c].data[1] == currentKey) {
				currentCap++;
			}
		}
		if (currentCap == 2) {
			paircount++;
		}
		currentCap = 0;

	}
	if (paircount == 2) {
		delete[] oldKeys;
		return(1);
	}
	delete[] oldKeys;
	return(0);
}

int onepair(card* hand, int* cardCount) {
	if (*cardCount < 2) {
		return(0);
	}
	if (*cardCount == 2) {//if there's only two cards, we only need to check if they're all equal
		if (hand[0].data[1] == hand[1].data[1]) {
			return(1);
		}
	}

	char currentKey = -1;
	int currentCap = 0;
	for (int runthrough = 0; runthrough < *cardCount; runthrough++) {
		currentKey = hand[runthrough].data[1];
		for (int c = 0; c < *cardCount; c++) {
			if (hand[c].data[1] == currentKey) {
				currentCap++;
			}
		}
		if (currentCap == 2) {
			return(1);
		}
		currentCap = 0;
	}

	return(0);
}
