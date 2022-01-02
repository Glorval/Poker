#include "poker.h"

int qsortCompDesecndingFirstByte(const void* a, const void* b) {
 return(*(char*)b - *(char*)a);
}

int qsortCompDesecndingCardsLevel(const void* a, const void* b) {
	return(((const Cards*)b)->data[levelPos] - ((const Cards*)a)->data[levelPos]);
}


pokerPlayer::pokerPlayer(Cards* givenHand, int givenmoney) : player(givenHand) {
	money = givenmoney;
	ourBet = 0;
}
pokerPlayer::pokerPlayer(Cards* givenHand, int givenmoney, int sorted) : player(givenHand) {
	money = givenmoney;
	ourBet = 0;
	qsort(hand, pSize(hand) / sizeof(Cards), sizeof(Cards), qsortCompDesecndingCardsLevel);
}

humanPlayer::humanPlayer(Cards* givenHand, int givenmoney) :pokerPlayer(givenHand, givenmoney, 1) {}

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

aiPlayer::aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName) :pokerPlayer(givenHand, givenmoney, 1) {
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



//only works with size 5 hands
handValue findValue(Cards* sortedHand) {
	handValue ourHandValue;
	ourHandValue.handType = -1;
	ourHandValue.highestCard = sortedHand[0].data[1];
	ourHandValue.potentialHands = NULL;
	int potentialHandCount = 0;

	int distances[5] = {
		sortedHand[2].data[levelPos] - sortedHand[0].data[levelPos],
		sortedHand[2].data[levelPos] - sortedHand[1].data[levelPos],
		0,//Middle card is always just itself
		sortedHand[2].data[levelPos] - sortedHand[3].data[levelPos],
		sortedHand[2].data[levelPos] - sortedHand[4].data[levelPos]
	};

	int suits[5] = {
		sortedHand[0].data[suitPos],
		sortedHand[1].data[suitPos],
		sortedHand[2].data[suitPos],
		sortedHand[3].data[suitPos],
		sortedHand[4].data[suitPos]
	};

	checkReturn returns = checkStraight(distances, suits);
	char* entry = (char*)&returns;
	int usefulCount = (entry[0] + entry[1] + entry[2] + entry[3] + entry[4]);


	return(ourHandValue);
}

checkReturn checkStraight(int* distances, int* suits) {
	//Optimization Note - Could somehow remove the distance zero search, but then we'd have to mark it useful anyhow
	int lookingFor[5] = { -2, -1, 0, 1, 2 };
	char distancesFound[5] = { 0,0,0,0,0 };
	char usefulCards[5] = { 0, 0, 0, 0, 0 };

	for (int cCard = 0; cCard < 5; cCard++) {
		for (int checker = 0; checker < 5; checker++) {
			if (distancesFound[checker] != 1 && distances[cCard] == lookingFor[checker]) {
				usefulCards[cCard] = 1;
				distancesFound[checker] = 1;
				break;
			}
		}
	}

	return(*(checkReturn*)&usefulCards);
}

//requires that the hand be sorted upon entry.
handValue findHandValue(Cards* hand) {
	int handsize = pSize(hand) / sizeof(Cards);
	//Cards** sortedHand = (Cards**)gcalloc(handsize, sizeof(Cards**));
	handValue ourHandValue;
	ourHandValue.handType = -1;
	

	ourHandValue.highestCard = hand[0].data[1];

	

	int neededCards[5] = { 0 };//Mark these as a needed slot just as we go down.
	//This will be used to allow a function to check if the given potential tradeup
	//will cuck our current hand by getting rid of a card we need.
	ourHandValue.potentialHands = (handPotential*)gcalloc(1, sizeof(handPotential));
	int potentialHandCount = 0;

	//Checking against all the functions
	//Standard pattern is 'is our return flag 1, if so the function did a thing.'
	//Then from there some internal logic specific to the insides of each one.
	//Some might need to check certain things, but ultimately it falls down to
	//saving this as as the best full hand type we can muster, saving it as a potential hand to shoot for,
	//or just moving on because we aren't involved.


	//Todo- Combine straight & flush into one function.
	if (straight(hand, &ourHandValue.potentialHands[potentialHandCount], handsize) == 1) {
		//If difficulty = 0, then this is a whole hand and we save it.
		if (ourHandValue.potentialHands[potentialHandCount].difficulty == 0 && ourHandValue.handType != -1) {
			ourHandValue.handType = ourHandValue.potentialHands[potentialHandCount].potentialHandValue;
			ourHandValue.handTypeHighest = ourHandValue.potentialHands[potentialHandCount].currentHighcard;
			neededCards[0] = 1;//easier to just set all to one here since we need all in a straight
			neededCards[1] = 1;
			neededCards[2] = 1;
			neededCards[3] = 1;
			neededCards[4] = 1;
		} else {//Otherwise, we keep it solely saved to ourHandValue and we'll just overwrite it next time.
			potentialHandCount++;
			ourHandValue.potentialHands = (handPotential*)grealloc(ourHandValue.potentialHands, sizeof(handPotential) * (potentialHandCount + 1));
		}
	}


	if (flush(hand, &ourHandValue.potentialHands[potentialHandCount], handsize) == 1) {
		//If difficulty = 0, then this is a whole hand and we save it.
		if (ourHandValue.potentialHands[potentialHandCount].difficulty == 0 && ourHandValue.handType != -1) {
			ourHandValue.handType = ourHandValue.potentialHands[potentialHandCount].potentialHandValue;
			ourHandValue.handTypeHighest = ourHandValue.potentialHands[potentialHandCount].currentHighcard;
			neededCards[0] = 1;//easier to just set all to one here since we need all in a flush
			neededCards[1] = 1;
			neededCards[2] = 1;
			neededCards[3] = 1;
			neededCards[4] = 1;
		} else {//Otherwise, we keep it solely saved to ourHandValue and we'll just overwrite it next time.
			potentialHandCount++;
			ourHandValue.potentialHands = (handPotential*)grealloc(ourHandValue.potentialHands, sizeof(handPotential) * (potentialHandCount + 1));
		}
	}












	ourHandValue.handType = 1;
	return(ourHandValue);
}

//struct handpotential {
//	int difficulty;
//	char positions[5];
//	int potentialHandValue;
//	char failureBad;
//};


void findHighcard(Cards* hand, handValue* handval,int handsize) {
	int highcard = -1;
	for (int c = 0; c < handsize; c++) {
		if (hand[c].data[1] > highcard) {
			highcard = hand[c].data[1];
		}
	}
	handval->highestCard = highcard;
}


//How this works:
//First step is to get distance between each card and the middle one
//Second is to run through and look for cards -2, -1, 1, and 2 away from the middle.
//Now we have all the useful cards, the inverse of cards we need to swap.
//Difficulty is 6 for one off, 15 for two off.
int straight(Cards* sortedHand, handPotential* handpotential, int handsize) {
	if (handsize != 5) {
		return(0);
	}
	int distance[4] = { 0 };

	//All cards measured against the first
	distance[0] = sortedHand[2].data[levelPos] - sortedHand[0].data[levelPos];
	distance[1] =  sortedHand[2].data[levelPos] - sortedHand[1].data[levelPos];
	distance[2] = sortedHand[2].data[levelPos] - sortedHand[3].data[levelPos];
	distance[3] = sortedHand[2].data[levelPos] - sortedHand[4].data[levelPos];

	//Search through all cards looking for -2, -1, 1, and 2 distances from the centre.
	char usefulCards[4] = { 0,0,0,0 };
	int usefulCount = 0;
	int currentSearch = -2;
	for (int cRun = 0; cRun < 4; cRun++) {
		for (int cItem = 0; cItem < 4; cItem++) {
			if (distance[cItem] == currentSearch) {
				usefulCards[cItem] = 1;
				usefulCount++;
				break;
			}
		}
		currentSearch++;
		if (currentSearch == 0) {
			currentSearch++;
		}
	}

	int difficulty = 0;
	if (usefulCount == 3) {
		difficulty = 6;
	} else if (usefulCount == 2) {
		difficulty = 15;
	} else if (usefulCount < 2){//Too few useful cards, insanity territory to try for a straight
		return(-1);
	}
	handpotential->difficulty = difficulty;
	handpotential->currentHighcard = sortedHand[0].data[levelPos];
	handpotential->neededToSwitch[0] = !usefulCards[0];
	handpotential->neededToSwitch[1] = !usefulCards[1];
	handpotential->neededToSwitch[3] = !usefulCards[2];
	handpotential->neededToSwitch[4] = !usefulCards[3];
	handpotential->potentialHandValue = Straight;
	return(1);
}


//How this works is it tallies up how many of each suit the hand has and keeps track of which cards
//are in each suit. Then, it sends back the positions of each card that'd need to be replaced to make
//the flush.
//Difficulty goes 1, 4
int flush(Cards* sortedHand, handPotential* handpotential, int handsize) {
	int wildPos[2] = { -1, -1 };//Allows up to two wilds
	int difficulty = 0;

	//enum suits { diamonds, hearts, clubs, spades }; < reference
	//this follows the same order.
	struct item {
		char count;
		char ourCards[5] = { 1, 2, 3, 4 ,5 };
	}Item[4];
	//memcpy(Item, &zero, sizeof(Item) * 4);
	char zero = 0;//Zero it all out
	for (int current = 0; current < sizeof(Item); current++) {
		char* temp = ((char*)Item + current);
		*temp = zero;
	}
	

	//Count all the occurances of the suits
	for (int currentCard = 0; currentCard < handsize; currentCard++) {
		if (sortedHand[currentCard].data[suitPos] == diamonds) {
			Item[0].count++;
			Item[0].ourCards[currentCard] = 1;
		} else if (sortedHand[currentCard].data[suitPos] == hearts) {
			Item[1].count++;
			Item[1].ourCards[currentCard] = 1;
		} else if (sortedHand[currentCard].data[suitPos] == clubs) {
			Item[2].count++;
			Item[2].ourCards[currentCard] = 1;
		} else if (sortedHand[currentCard].data[suitPos] == spades) {
			Item[3].count++;
			Item[3].ourCards[currentCard] = 1;
		}
	}

	qsort(Item, 4, sizeof(struct item), qsortCompDesecndingFirstByte);

	//are all in the same category, aka a flush?
	if (Item[0].count == handsize) {
		//victory, we have a flush xd
		//makes it very easy on us
		handpotential->difficulty = 0;
		handpotential->currentHighcard = sortedHand[0].data[levelPos];
		handpotential->necessaryCards[0] = 1;
		handpotential->necessaryCards[1] = 1;
		handpotential->necessaryCards[2] = 1;
		handpotential->necessaryCards[3] = 1;
		handpotential->necessaryCards[4] = 1;
		return(1);
	}

	//two or less cards off
	int offby = handsize - Item[0].count;
	if (offby <= 2) {

		for (int cCard = 0; cCard < handsize; cCard++) {
			handpotential->neededToSwitch[cCard] = Item[1].ourCards[cCard] + Item[2].ourCards[cCard] + Item[3].ourCards[cCard];
		}


		handpotential->difficulty = offby * offby;
	}


	return(1);
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
