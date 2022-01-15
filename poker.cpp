#include "poker.h"

enum handTypes { Highcard, TwoKind, TwoPair, ThreeKind, Straight, Flush, FullHouse, FourKind, StraightFlush, RoyalFlush };
int handScore[] = { 0,			6,				10,		13,				22,		25,		30,			50,			75,				100 };
int pairFullHouseDiff = 4;
int pairTwoKindDiff = 3;
int pairThreeKindDiff = 5;
int straightDifficultyLevels[] = { 0, 8, 18, -1, -1 };
int flushDifficultyLevels[] = { 0, 3, 7, -1, -1 };

int qsortCompDesecndingFirstByte(const void* a, const void* b) {
 return(*(char*)b - *(char*)a);
}

int qsortCompDesecndingCardsLevel(const void* a, const void* b) {
	return(((const Cards*)b)->data[levelPos] - ((const Cards*)a)->data[levelPos]);
}


pokerPlayer::pokerPlayer(Cards* givenHand, int givenmoney) : player(givenHand) {
	money = givenmoney;
	ourHandValue = { 0 };
	ourBet = 0;
}
pokerPlayer::pokerPlayer(Cards* givenHand, int givenmoney, int sorted) : player(givenHand) {
	money = givenmoney;
	ourHandValue = { 0 };
	ourBet = 0;
	qsort(hand, pSize(hand) / sizeof(Cards), sizeof(Cards), qsortCompDesecndingCardsLevel);
}

void pokerPlayer::sortOurHand() {
	qsort(hand, pSize(hand) / sizeof(Cards), sizeof(Cards), qsortCompDesecndingCardsLevel);
}

void pokerPlayer::evaluateHand() {
	sortOurHand();
	ourHandValue = findValue(hand);
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

int humanPlayer::takeNormalTurn(int curBet, int someoneCalled) {
	return 0;
}

cardDiscards humanPlayer::takeDiscardTurn() {
	return *(cardDiscards*)&"F";
}

aiPlayer::aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName) :pokerPlayer(givenHand, givenmoney, 1) {
	name = givenName;
	aggressiveness = givenaggresiveness;
	maxBet = -1;
}

void aiPlayer::initializeMaxBet() {
	//Essentially, average the aggressiveness, a random multiplier, and hand value for the max amount of money we will bet
	//(also, its 'fuck you' math time xddddd)
	//TODO : Swap out calchandvalue
	//maxBet = (int)((((double)(aggressiveness + (rand() % 100)) + (calcHandValue(hand, handsize) / 2)) / 700) * money);
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

int aiPlayer::takeNormalTurn(int curBet, int someoneDidSomething) {
	if (someoneDidSomething == call && (rand() % 2) == 0) {
		return(call);
	}
	else if (someoneDidSomething == fold && (rand() % 6) == 0) {
		return(fold);
	}

	if (curBet > maxBet) {
		return(fold);
	} else {
		int distance = curBet - maxBet;
		//int raiseby = (int)((double)distance * ((double)aggressiveness / (150 + (rand() % 100))));
		//idk what the fugg I'm doing here
		int raiseby = ((double)(distance - 75) * ((double)aggressiveness / 75)) + ( (double)aggressiveness * (double)(50 + (long)(rand() % 100)) * 0.005);
		if (raiseby < 0) {
			raiseby = (rand() % 10);
		}
		raiseby += 5;
		
		//once the bet is up to half of their max bet, there starts to be a linear increase in the chance to resign
		int resignChance = (ourBet + raiseby - (maxBet/2)) / maxBet;
		if (rand() % 100 > resignChance) {
			return(fold);
		} else if (raiseby + ourBet > money) {
			return(call);
		} else {
			return(raiseby);
		}
	}

}

cardDiscards aiPlayer::takeDiscardTurn(){
	//			1-0.33333, inversely scaled on aggression
	int inverseScalar = 50 / (aggressiveness + 50);
	int curHandScore = handScore[ourHandValue.handType] * inverseScalar + (ourHandValue.handTypeHighest / 6) + (ourHandValue.highestCard / 12);//handscore * inverse scalar is 0-50, but generally single digits to low double.

	int potentialHandCount = pSize(ourHandValue.potentialHands)/sizeof(handPotential);
	int bestPotential = 0;
	int bestPos = 0;
	//0.5-1.5, linearly scaled off aggressiveness
	int scalar = (aggressiveness + 50) / 100;
	for (int curPotential = 0; curPotential < potentialHandCount; curPotential++) {
		int curScore = ourHandValue.potentialHands[curPotential].potentialHandValue * scalar / ourHandValue.potentialHands[curPotential].difficulty;

		//Factor in the potential of losing our current hand.
		curScore -= (ourHandValue.potentialHands[curPotential].failureBad) * inverseScalar * curHandScore;
		if (curScore > bestPotential) {
			bestPotential = curScore;
			bestPos = curPotential;
		}
	}

	char discardable[5] = { 0 };
	if (bestPotential > curHandScore) {
		discardable[0] = ourHandValue.potentialHands[bestPos].neededToSwitch[0];
		discardable[1] = ourHandValue.potentialHands[bestPos].neededToSwitch[1];
		discardable[2] = ourHandValue.potentialHands[bestPos].neededToSwitch[2];
		discardable[3] = ourHandValue.potentialHands[bestPos].neededToSwitch[3];
		discardable[4] = ourHandValue.potentialHands[bestPos].neededToSwitch[4];
	}
	return(*(cardDiscards*)&discardable);
	
}

poker::poker() {
	ourDeck = new deck();
	ourDeck->shuffle();

	stageOfGame = -1;
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
						double curScore = 0;//calcHandValue(players[cPlayer]->hand, players[cPlayer]->getHandsize());
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




//only works with size 5 hands
handValue findValue(Cards* sortedHand) {
#ifdef DEBUG
	printf("\nHand being evaluated:\n");
	printCardsNNL(sortedHand, 5);
	printf("\n\n");
#endif



	handValue ourHandValue = { 0 };
	ourHandValue.handType = Highcard;
	ourHandValue.highestCard = sortedHand[0].data[1];
	ourHandValue.potentialHands = NULL;
	int potentialHandCount = 0;

	//ease of use
	int levels[5] = {
		sortedHand[0].data[levelPos],
		sortedHand[1].data[levelPos],
		sortedHand[2].data[levelPos],
		sortedHand[3].data[levelPos],
		sortedHand[4].data[levelPos]
	};

	int suits[5] = {
		sortedHand[0].data[suitPos],
		sortedHand[1].data[suitPos],
		sortedHand[2].data[suitPos],
		sortedHand[3].data[suitPos],
		sortedHand[4].data[suitPos]
	};

	handPotential* potentialHands = NULL;
	potentialHands = (handPotential*)grealloc(potentialHands, (potentialHandCount + 1) * sizeof(handPotential));

	//PAIRS
	//Allowed three off... Reason being that any more off and you've not got a pair at all...
	//int pairDifficultyLevels[] = { 0, 1, 1, 1, 1 };
	setPotential((char*)&checkPairs(levels), potentialHands, &potentialHandCount, &ourHandValue, 0, 3);
	potentialHands = (handPotential*)grealloc(potentialHands, (potentialHandCount + 1) * sizeof(handPotential));
	//END OF PAIRS

	//STRAIGHT
	//allowed two off from a straight to consider it
	//int straightDifficultyLevels[] = { 0, 8, 18, -1, -1 };
	int isStraight = setPotential((char*)&checkStraight(levels), potentialHands, &potentialHandCount, &ourHandValue, straightDifficultyLevels, 2);
	potentialHands = (handPotential*)grealloc(potentialHands, (potentialHandCount + 1) * sizeof(handPotential));
	//END OF STRAIGHT

	//FLUSH
	//allowed two off from a flush to consider it
	//int flushDifficultyLevels[] = { 0, 3, 7, -1, -1 };
	int isFlush = setPotential((char*)&checkFlush(suits), potentialHands, &potentialHandCount, &ourHandValue, flushDifficultyLevels, 2);
	potentialHands = (handPotential*)grealloc(potentialHands, potentialHandCount * sizeof(handPotential));
	//END OF FLUSH

	//STRAIGHT FLUSH/ROYAL FLUSH
	//(After the previous two because they aren't combined)
	//Just for registering if we have one, not supported to try and shoot for one of these.
	if (isStraight == 1 && isFlush == 1) {
		if (sortedHand[0].data[levelPos] == ace) {
			ourHandValue.handType = RoyalFlush;
			ourHandValue.highestCard = ace;
			ourHandValue.handTypeHighest = ace;
		} else {
			ourHandValue.handType = StraightFlush;
			ourHandValue.highestCard = sortedHand[0].data[levelPos];
			ourHandValue.handTypeHighest = sortedHand[0].data[levelPos];
		}
	}
	//END OF STRAIGHT FLUSH/ROYAL FLUSH
	
	ourHandValue.potentialHands = potentialHands;

#ifdef DEBUG
	//print out everything.
	printf("\n\nEnd of hand reading report:\n\n");
	printf("\tOur current hand's data:\n");
	printf("\t\tHand Type: %d,\n\t\tHighest Card In Hand: %d\n\t\tHighest Card: %d\n", ourHandValue.handType, ourHandValue.handTypeHighest, ourHandValue.highestCard);
	char* necCards = ourHandValue.necessaryCards;
	printf("\t\tNecessary Cards: %d, %d, %d, %d, %d\n\n", necCards[0], necCards[1], necCards[2], necCards[3], necCards[4]);

	printf("\tPotential Hand Count: %d\n", potentialHandCount);
	for (int cH = 0; cH < potentialHandCount; cH++) {
		printf("\t\tHand #: %d\n", cH);
		printf("\t\tPotential Value: %d\n\t\tDifficulty: %d\n\t\tFailure Bad: %d\n\t\tHighest In Hand: %d\n", potentialHands[cH].potentialHandValue, potentialHands[cH].difficulty, potentialHands[cH].failureBad, potentialHands[cH].highestInHand);
		printf("\t\tNeeded to Switch: %d, %d, %d, %d, %d\n\n", potentialHands[cH].neededToSwitch[0], potentialHands[cH].neededToSwitch[1], potentialHands[cH].neededToSwitch[2], potentialHands[cH].neededToSwitch[3], potentialHands[cH].neededToSwitch[4]);
	}
	printf("\n\nEnd of debug block\n\n");
#endif // DEBUG
	

	return(ourHandValue);
}

//data is five cards useless or not, hand value, and the highcard (Could be -1 if hand incomplete, but don't rely)
//Potential hands is where we save it if it is almost usable, handcounts is just how long this array is
//actualHand is for saving to if we've got a whole hand
//difficulties are told to us for setting potentialHands based off how many missing we have
//margin of error is how many cards we can have be bad before we just throw it out
int setPotential(char* data, handPotential* potentialHands, int *handcounts, handValue* actualHand, int difficulties[5], int marginOfError) {
	int error = data[0] + data[1] + data[2] + data[3] + data[4];//find the margin of error in the hand
	#ifdef DEBUG
	printf("\nRecording hand. Error: %d. Margin of error allowed: %d.\n", error, marginOfError);
	#endif
	if (error > marginOfError) {//if it is unacceptably high, abort
		#ifdef DEBUG
		printf("Unacceptable error.\n");
		#endif
		return -1;
	} else if (error != 0 && data[7] != 1) {//If incomplete and not a pairtype of hand
		//otherwise,
		potentialHands[*handcounts].difficulty = difficulties[error];//set the difficulty based on how much error we have
		potentialHands[*handcounts].potentialHandValue = data[5];//take the potential hand value
		potentialHands[*handcounts].neededToSwitch[0] = data[0];//and set all those we need to switch
		potentialHands[*handcounts].neededToSwitch[1] = data[1];
		potentialHands[*handcounts].neededToSwitch[2] = data[2];
		potentialHands[*handcounts].neededToSwitch[3] = data[3];
		potentialHands[*handcounts].neededToSwitch[4] = data[4];
		potentialHands[*handcounts].highestInHand = 0;

		#ifdef DEBUG
		printf("Non-pairtype incomplete hand:\nUseless list: %d, %d, %d, %d, %d\nDifficulty: %d\n", data[0], data[1], data[2], data[3], data[4], difficulties[error]);
		#endif	

		if (actualHand->handType != Highcard) {//If we've gotten a hand earlier, then check against all necessary cards to see if shooting for this new one kills it.

			#ifdef DEBUG
			printf("Checking non-pairtype incomplete hand to see if it conflicts with another.\nNecessary cards: %d, %d, %d, %d, %d\n", actualHand->necessaryCards[0], actualHand->necessaryCards[1], actualHand->necessaryCards[2], actualHand->necessaryCards[3], actualHand->necessaryCards[4]);
			#endif

			//data is useless cards, the right is necessary
			//Therefore, if one equals a necessary card, it means we'd lose the previous hand to get the new one
			if ((data[0] == actualHand->necessaryCards[0] && data[0] == 1) ||
				(data[1] == actualHand->necessaryCards[1] && data[1] == 1) ||
				(data[2] == actualHand->necessaryCards[2] && data[2] == 1) ||
				(data[3] == actualHand->necessaryCards[3] && data[3] == 1) ||
				(data[4] == actualHand->necessaryCards[4] && data[4] == 1)) {
				//If we try for this hand, we will lose cards from the current that we need
				potentialHands[*handcounts].failureBad = 1;
				#ifdef DEBUG
				printf("Failure set to 'bad'.\n");
				#endif
			} else {
				//we good to shoot for this
				potentialHands[*handcounts].failureBad = 0;
				#ifdef DEBUG
				printf("Failure set to 'acceptable'.\n");
				#endif
			}
		} else {
			#ifdef DEBUG
			printf("Incomplete non-pairtype hand marking it as non-conflicting.\nNecessary cards: %d, %d, %d, %d, %d\n", actualHand->necessaryCards[0], actualHand->necessaryCards[1], actualHand->necessaryCards[2], actualHand->necessaryCards[3], actualHand->necessaryCards[4]);
			#endif
			potentialHands[*handcounts].failureBad = 0;
		}
		*handcounts += 1;//we need to count up how many potential hands we have
		return(0);
	} else if (data[7] == 1) {//If we've got a pair type, aka, we improve onto the same cards by discarding others

		if (data[5] != FullHouse && data[5] != FourKind) {//Are we an improvable pairkind
			#ifdef DEBUG
			printf("Checking in 'improvable pair-kind'\n");
			#endif
			int potentialValue = 0;
			if (data[5] == TwoPair) {
				#ifdef DEBUG
				printf("Is two pair, can shoot for full house.\n");
				#endif
				potentialValue = FullHouse;
				potentialHands[*handcounts].difficulty = pairFullHouseDiff;
			} else if (data[5] == TwoKind) {
				#ifdef DEBUG
				printf("Is two kind, can shoot for three/fourkind.\n");
				#endif
				potentialValue = FourKind;
				potentialHands[*handcounts].difficulty = pairTwoKindDiff;
			} 
			else if (data[5] == ThreeKind) {
				#ifdef DEBUG
				printf("Is three kind, can shoot for fourkind.\n");
				#endif
				potentialValue = FourKind;
				potentialHands[*handcounts].difficulty = pairThreeKindDiff;
			}

			potentialHands[*handcounts].potentialHandValue = potentialValue;
			potentialHands[*handcounts].neededToSwitch[0] = data[0];//and set all those we need to switch
			potentialHands[*handcounts].neededToSwitch[1] = data[1];
			potentialHands[*handcounts].neededToSwitch[2] = data[2];
			potentialHands[*handcounts].neededToSwitch[3] = data[3];
			potentialHands[*handcounts].neededToSwitch[4] = data[4];
			potentialHands[*handcounts].failureBad = 0;
			handcounts++;//we need to count up how many potential hands we have
		}
		actualHand->handType = data[5];
		actualHand->handTypeHighest = data[6];
		actualHand->necessaryCards[0] = !data[0];
		actualHand->necessaryCards[1] = !data[1];
		actualHand->necessaryCards[2] = !data[2];
		actualHand->necessaryCards[3] = !data[3];
		actualHand->necessaryCards[4] = !data[4];
		return(1);
	} else {//If it's a complete non-pair hand
		#ifdef DEBUG
		printf("Is complete hand of type '%d'.\n\n", data[5]);
		#endif
		actualHand->handType = data[5];
		actualHand->handTypeHighest = data[6];
		actualHand->necessaryCards[0] = 1;
		actualHand->necessaryCards[1] = 1;
		actualHand->necessaryCards[2] = 1;
		actualHand->necessaryCards[3] = 1;
		actualHand->necessaryCards[4] = 1;
		return(1);
	}
}

checkReturn checkStraight(int* levels) {
	//Optimization Note - Could somehow remove the distance zero search, but then we'd have to mark it useful anyhow
	int lookingFor[5] = { -2, -1, 0, 1, 2 };
	char distancesFound[5] = { 0,0,0,0,0 };
	char uselessCards[7] = {1, 1, 1, 1, 1, Straight, 0};

	char temp[5] = { 0 };
	temp[0] = levels[0];
	temp[1] = levels[1];
	temp[2] = levels[2];
	temp[3] = levels[3];
	temp[4] = levels[4];


	for (int cCard = 0; cCard < 5; cCard++) {
		if (levels[cCard] > uselessCards[6]) {//find the highcard on the way
			uselessCards[6] = levels[cCard];
		}
		for (int checker = 0; checker < 5; checker++) {
			int distance = levels[cCard] - levels[2];
			if (distancesFound[checker] != 1 && (levels[cCard] - levels[2]/*middle card*/)== lookingFor[checker]) {
				uselessCards[cCard] = 0;
				distancesFound[checker] = 1;
				break;
			}
		}
	}
	
	return(*(checkReturn*)&uselessCards);
}

checkReturn checkFlush(int* suits) {
	//Expansion Note - Only supports saying stuff is useful if there's already three of a suit, could be made to 
	//do more for if it was desired to go 'you have two clubs, chance of getting three more is x%'

	//enum suits { diamonds, hearts, clubs, spades };
	char suitOccurances[4] = { 0,0,0,0 };
	char uselessCards[6] = { 1, 1, 1, 1, 1, Flush};

	for (int cCard = 0; cCard < 5; cCard++) {
		switch (suits[cCard]) {
			case(diamonds):
				suitOccurances[diamonds]++;
				break;
			case(hearts):
				suitOccurances[hearts]++;
				break;
			case(clubs):
				suitOccurances[clubs]++;
				break;
			case(spades):
				suitOccurances[spades]++;
				break;
		}
	}

	int highestSuitOccurancePosition = 0;//Note - this also represents the suit itself. 
	//Reason being that the suits go '0 = diamonds, 1 = hearts... etc' and the array here
	//is in that order too.
	for (int cPos = 1; cPos < 4; cPos++) {
		if (suitOccurances[cPos] > suitOccurances[highestSuitOccurancePosition]) {
			highestSuitOccurancePosition = cPos;
		}
	}
	if (suitOccurances[highestSuitOccurancePosition] <= 2) {//If we have shitall for this it's not even worth considering
		return(*(checkReturn*)&uselessCards);
	}

	//Go through each card and check to see if it's the right suit
	//Probably not the best way to do this, as stuff could be done to mark them out whilst counting the most frequent suit
	for (int cCard = 0; cCard < 5; cCard++) {
		if (suits[cCard] == highestSuitOccurancePosition) {
			uselessCards[cCard] = 0;
		}
	}

	return(*(checkReturn*)&uselessCards);
}

checkReturn checkPairs(int* levels) {
	checkReturn returns;
	//enum levels {	 two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };
	struct item {
		char levelFreq;
		char ourLevel;
	}Items[13] = { 0 };
	for (int cCard = 0; cCard < 5; cCard++) {
		Items[levels[cCard]].levelFreq++;
		Items[levels[cCard]].ourLevel = levels[cCard];
	}
	int temp[5] = { levels[0], levels[1],levels[2],levels[3] ,levels[4] };
	qsort(Items, 13, sizeof(struct item), qsortCompDesecndingFirstByte);
	
	switch (Items[0].levelFreq) {
		case(4):
			returns.handValue = FourKind;
			returns.highestInHand = Items[0].ourLevel;
			returns.uselessCards[0] = (levels[0] != Items[0].ourLevel);
			returns.uselessCards[1] = (levels[1] != Items[0].ourLevel);
			returns.uselessCards[2] = (levels[2] != Items[0].ourLevel);
			returns.uselessCards[3] = (levels[3] != Items[0].ourLevel);
			returns.uselessCards[4] = (levels[4] != Items[0].ourLevel);
			returns.isPairKind = 1;
			break;
		case(3):
			if (Items[1].levelFreq == 2) {//full house edgecase
				returns.handValue = FullHouse;
				if (Items[0].ourLevel > Items[1].ourLevel) {//If the 3 or the 2 has the highest card
					returns.highestInHand = Items[0].ourLevel;
				} else {
					returns.highestInHand = Items[1].ourLevel;
				}
				//All cards needed in a full house
				returns.uselessCards[0] = 0;
				returns.uselessCards[1] = 0;
				returns.uselessCards[2] = 0;
				returns.uselessCards[3] = 0;
				returns.uselessCards[4] = 0;
				returns.isPairKind = 1;
				break;
			} else {//not full house
				returns.handValue = ThreeKind;
				returns.highestInHand = Items[0].ourLevel;
				returns.uselessCards[0] = (levels[0] != Items[0].ourLevel);
				returns.uselessCards[1] = (levels[1] != Items[0].ourLevel);
				returns.uselessCards[2] = (levels[2] != Items[0].ourLevel);
				returns.uselessCards[3] = (levels[3] != Items[0].ourLevel);
				returns.uselessCards[4] = (levels[4] != Items[0].ourLevel);
				returns.isPairKind = 1;
				break;
			}
		case(2):
			if (Items[1].levelFreq == 2) {//Two pair edgecase
				returns.handValue = TwoPair;
				if (Items[0].ourLevel > Items[1].ourLevel) {//If the first or second has the highest card
					returns.highestInHand = Items[0].ourLevel;
				} else {
					returns.highestInHand = Items[1].ourLevel;
				}
				returns.uselessCards[0] = (levels[0] != Items[0].ourLevel && levels[0] != Items[1].ourLevel);
				returns.uselessCards[1] = (levels[1] != Items[0].ourLevel && levels[1] != Items[1].ourLevel);
				returns.uselessCards[2] = (levels[2] != Items[0].ourLevel && levels[2] != Items[1].ourLevel);
				returns.uselessCards[3] = (levels[3] != Items[0].ourLevel && levels[3] != Items[1].ourLevel);
				returns.uselessCards[4] = (levels[4] != Items[0].ourLevel && levels[4] != Items[1].ourLevel);
				returns.isPairKind = 1;
				break;
			} else {//otherwise, two of a kind
				returns.handValue = TwoKind;
				returns.highestInHand = Items[0].ourLevel;
				returns.uselessCards[0] = (levels[0] != Items[0].ourLevel);
				returns.uselessCards[1] = (levels[1] != Items[0].ourLevel);
				returns.uselessCards[2] = (levels[2] != Items[0].ourLevel);
				returns.uselessCards[3] = (levels[3] != Items[0].ourLevel);
				returns.uselessCards[4] = (levels[4] != Items[0].ourLevel);				
				returns.isPairKind = 1;
				break;
			}
		default:
			returns.uselessCards[0] = 1;
			returns.uselessCards[1] = 1;
			returns.uselessCards[2] = 1;
			returns.uselessCards[3] = 1;
			returns.uselessCards[4] = 1;
			returns.handValue = -1;
			returns.highestInHand = -1;
			returns.isPairKind = 0;
			return(returns);
	}

	return(returns);
}