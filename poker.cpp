#include "poker.h"



pokerPlayer::pokerPlayer(Cards* givenHand, int givenmoney) : player(givenHand) {
	money = givenmoney;
	ourBet = 0;
}

humanPlayer::humanPlayer(Cards* givenHand, int givenmoney) :pokerPlayer(givenHand, givenmoney) {}

int humanPlayer::taketurn(int curBet, int stage) {
	std::cout << "\nIt is now your turn,\n";
	std::cout << "Current bet: $" << curBet << ", You have $" << money - ourBet << " and $" << ourBet << " on the table.";
	std::cout << "\nHand:  ";
	printHand(1);
	std::cout << "\n\nCall, Raise, or Fold?\t";

	userInput.clear();
	std::cin >> userInput;
	std::cout << "\n\n";
	if (userInput.compare("Call") == 0 || userInput.compare("call") == 0) {
		if (curBet > money) {
			std::cout << "Sorry, you do not have enough money. You have to fold.\n\n\n";
			return(-1);
		}
		ourBet = curBet;
		return(0);
	} else if (userInput.compare("Raise") == 0 || userInput.compare("raise") == 0) {
		if (curBet > money) {
			std::cout << "Sorry, you do not have enough money. You have to fold.\n\n\n";
			return(-1);
		}
		int raise = 0;
		std::cin >> raise;
		ourBet = raise + curBet;
		if (ourBet > money) {
			std::cout << "Sorry, you do not have enough money for that. You will instead raise the bet to $" << money << "\n\n\n";
			ourBet = money;
			return(ourBet);
		}
		return(raise + curBet);
	} else if (userInput.compare("Fold") == 0 || userInput.compare("fold") == 0) {
		return(-1);
	} else {//ez pz contigency is that invalid input is a call
		if (curBet > money) {
			std::cout << "Sorry, you do not have enough money. You have to fold.\n\n\n";
			return(-1);
		}
		ourBet = curBet;
		return(0);
	}

}

aiPlayer::aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName) :pokerPlayer(givenHand, givenmoney) {
	name = givenName;
	aggressiveness = givenaggresiveness;
	maxBet = -1;
}

void aiPlayer::initializeMaxBet() {
	//Essentially, average the aggressiveness, a random multiplier, and hand value for the max amount of money we will bet
	//(also, its 'fuck you' math time xddddd)
	maxBet = (int)((((double)(aggressiveness + (rand() % 100)) + (calcHandValue(hand, handsize) / 2)) / 700) * money);
	//std::cout << "AI max bet: " << maxBet <<"Aggressiveness: "<<aggressiveness<<"\n";//debug line
}

int aiPlayer::taketurn(int curBet, int stage) {
	std::cout << "It is AI " << name << "'s turn. The current bet to match is $" << curBet << ", they have $" << money - ourBet << " and $" << ourBet << " on the table.\n";
	if (stage == 0) {
		initializeMaxBet();
	}

	//someone before us called
	if (stage == 2) {
		//call protocol
		//std::cout << "DEBUG: Call protocol\n";
		if (rand() % 2 == 0) {
			ourBet = curBet;
			std::cout << "They called.\n\n";
			return(0);
		}
	}

	if ((double)curBet < (double)maxBet * 0.70) {
		//comfortably below the max bet margin
		int distance = maxBet - curBet;
		//wacky raiseby logic here, essentially we take the distance between our current bet and the maximum we wish to do,
		//multiply it by our aggressiveness scaled down to something similar to a percentage, but we never can do a 100% raise.
		int raiseby = (int)((double)distance * ((double)aggressiveness / (150 + (rand() % 100))));
		raiseby += rand() % 10;
		ourBet = curBet + raiseby;
		std::cout << "They raised $" << (int)raiseby << ", bringing the current bet to match to $" << curBet + raiseby << "\n\n";
		return(curBet + raiseby);
	} else if (curBet < maxBet) {
		double chance = (double)curBet / (double)maxBet;
		if (rand() % 100 <= (int)(chance * 100)) {
			std::cout << "They called.\n\n";
			ourBet = curBet;
			return(0);
		}
		if (rand() % 100 == 0) {
			std::cout << "They folded\n\n";
			return(-1);
		}

	} else {
		std::cout << "They folded\n\n";
		return(-1);
	}


	//backup code
	std::cout << "They called.\n\n";
	ourBet = curBet;
	return(0);
}


poker::poker() {
	ourDeck = new deck();
	ourDeck->shuffle();

	stageOfGame = -1;
	int currentPlayer = 0;
	currentBet = 0;
	playercount = 0;
	players = nullptr;
	currentPlayer = 0;
}

int poker::checkContinuePlay(int flag) {
	if (flag) {
		std::cout << "Would you like to play another round? You have $" << players[playercount - 1]->money << ". y/n\t\t";
		int yesno = _getche();
		if (yesno == 'y') {
			ourDeck->shuffle();
			for (int c = 0; c < playercount; c++) {
				if (players[c]->money <= 5) {
					players[c]->ourBet = 0;
					std::cout << "Player " << players[c]->name << " does not have enough money to continue to play.";
				} else {
					players[c]->hand = ourDeck->drawCards(pokerHandSize);//memory leak but getting really short on timeeee aaaaaaaah
					players[c]->ourBet = 5;
				}
			}
			std::cout << "\n\n\n";
			return(1);
		} else {
			std::cout << "\n\nHope you had fun.\n";
			exit(0);
		}
	} else {
		std::cout << "Would you like to play a game? y/n\t\t";
		int yesno = _getche();
		std::cout << "\n";
		if (yesno == 'y') {
			std::cout << "And how many AI? (1-4)   \t";
			int aiCount = _getche() - '0';
			std::cout << '\n';
			if (aiCount > 4) {
				aiCount = 4;
			}

			std::string names[] = { "Helga", "Schultz", "Hogan", "Hilda" };
			playercount = aiCount + 1;
			players = new pokerPlayer * [playercount];
			for (int c = 0; c < aiCount; c++) {
				players[c] = new aiPlayer(ourDeck->drawCards(pokerHandSize), startingMoney, rand() % 100, names[c]);
				players[c]->ourBet = 5;
			}
			players[aiCount] = new humanPlayer(ourDeck->drawCards(pokerHandSize), startingMoney);
			players[aiCount]->ourBet = 5;

			std::cout << "What would you like your name to be?  \t";
			std::cin >> players[playercount - 1]->name;
			std::cout << "\n\n\n";
			return(1);
		} else {
			std::cout << "\n\nI'll see you next time.\n";
			exit(0);
		}
	}
}

void poker::endOfRound(int winner) {
	int pot = 0;
	for (int cPlayer = 0; cPlayer < playercount; cPlayer++) {
		pot += players[cPlayer]->ourBet;
		players[cPlayer]->money -= players[cPlayer]->ourBet;
		std::cout << "Player " << players[cPlayer]->name << "'s hand: ";
		players[cPlayer]->printHand(1);
		std::cout << "\n";
	}
	std::cout << "\n";
	players[winner]->money += pot;
	std::cout << "Player " << players[winner]->name << " won the pot of $" << pot << "!\n\n\n";
}

void poker::playRound() {
	stageOfGame = 0;
	int* foldedPlayers = new int[playercount]();
	int firstCall = -1;
	int callChaining = -1;
	for (int cp = 0; cp < playercount; cp++) {
		if (players[cp]->ourBet == 0) {
			foldedPlayers[cp] = 1;
		}
	}
	while (1) {
		for (int c = 0; c < playercount; c++) {
			if (firstCall == c && callChaining == 1) {
				double highestScore = 0;
				int winnerplayer = 0;
				for (int cPlayer = 0; cPlayer < playercount; cPlayer++) {
					if (foldedPlayers[cPlayer] == 0) {
						double curScore = calcHandValue(players[cPlayer]->hand, players[cPlayer]->getHandsize());
						if (curScore > highestScore) {
							highestScore = curScore;
							winnerplayer = cPlayer;
						}
					}
				}
				endOfRound(winnerplayer);
				return;
			}

			if (foldedPlayers[c] == 0) {
				int playCode = players[c]->taketurn(currentBet, stageOfGame);
				if (playCode == -1) {
					foldedPlayers[c] = 1;

					int folded = 0;
					int lastPlayerIn;
					for (int check = 0; check < playercount; check++) {
						if (foldedPlayers[check]) {
							folded++;
						} else {
							lastPlayerIn = check;
						}
					}
					if (folded == playercount - 1) {
						endOfRound(lastPlayerIn);
						return;
					}
				} else if (playCode > 0) {
					currentBet = playCode;
					callChaining = -1;
					if (stageOfGame == 2) {
						stageOfGame = 1;
					}
				} else if (playCode == 0 && callChaining == -1) {
					firstCall = c;
					callChaining = 1;
					if (stageOfGame == 1) {
						stageOfGame = 2;
					}
				}
			}
		}//end of for loop

		if (stageOfGame == 0) {
			stageOfGame = 1;
		}
	}
}

void poker::playGame(){
	std::cout << "Welcome to my poker game!\n\nThe rules of this iteration of poker are very simple (Totally not because I wished to get this done a lil faster as it was already vastly overambitious).\nFor each round, betting will circle around until a 'circle of calls' happens. That is to say, if someone calls and the turns circle back to them and no raises have been made, hands are revealed and a player will win.\n\n";
	std::cout << "When that happens, all player's provived bets will be collected up and given to the winner!\n\n";
	std::cout << "And a note upon how the winner is determined: Due to complexity reasons and already going vastly overbudget on how much time I could put towards this, all types of combinations are worth a set value of points added onto by the highest card in the hand. As in, an King pair itself is worth the same as a Jack pair, difference being that the highest card in either hand nudges the value a little such that the Jack pair could win if they had an Ace highcard.\n\n";
	std::cout << "From there, it is up to the human player to decide whether or not to continue, or to end the game entirely.\n";
	std::cout << "The controls are equally simple, 'call' or 'Call' is a call. Same goes for folding. Raising is similar, but the amount you wish to raise by should follow it. Example: Raise 50\n\nHave fun!\n\n\n";

	int flag = 0;
	while (checkContinuePlay(flag)) {
		flag = 1;
		currentBet = 5;
		playRound();
	}
}



enum handTypes { RoyalFlush, StraightFlush, FourKind, FullHouse, Flush, Straight, ThreeKind, TwoPair, OnePair };

handValue findHandValue(Cards* hand) {
	int handsize = pSize(hand) / sizeof(Cards);
	Cards** sortedHand = (Cards**)calloc(handsize, sizeof(Cards**));
	handValue returns;

	int* list = fillPseudolist(handsize);
	Cards* greatest = NULL;
	int greatestIndex = 0;
	for (int sweep = 0; sweep < handsize; sweep++) {
		for (int c = 0; c < handsize; c++) {
			if (list[c] == 0) {//if it hasnt been removed
				if (greatest == NULL) {
					greatest = &hand[c];
					greatestIndex = c;
				}else if (hand[c].data[1] > greatest->data[1]) {
					greatest = &hand[c];
					greatestIndex = c;
				}
			}
		}
		list[greatestIndex] = 1;
		sortedHand[sweep] = greatest;
		greatest = NULL;
	}
	gfree(list);

























	free(sortedHand);
	returns.handType = 1;
	return(returns);
}




void findHighcard(Cards** hand, handValue* handval,int handsize) {
	int highcard = -1;
	for (int c = 0; c < handsize; c++) {
		if (hand[c]->data[1] > highcard) {
			highcard = hand[c]->data[1];
		}
	}
	handval->highestCard = highcard;
}

int straight(Cards** hand, int handsize) {
	for (int c = 1; c < handsize; c++) {
		if (hand[c]->data[1] != hand[c - 1]->data[1] - 1) {
			int distance = hand[c]->data[1] - hand[c - 1]->data[1];
		}
	}
}


//this section is pain...
//NOTE - redo the whole thing. 
//Make a struct to return the highest match type, what the highcard of that type is, and highcard overall.
int straights = 0;
int flushes = 0;

double handValues[] = { 1000,		950,					900,			800,		600,		500,			 400,			300,		200 };

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
