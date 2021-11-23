#pragma once
#include "cards.h"
#include <conio.h>
#define pokerHandSize 5
#define startingMoney 500
#define call 0
#define fold -1
#define startingBet 5

double calcHandValue(card* hand, int cardCount);

//note - demonstates polymorphism because both have a 'take turn' function
//and are all treated as a 'player' object later on when driving code.
class humanPlayer : public player {
	public:
		humanPlayer(Cards* givenHand, int givenmoney):player(givenHand){
			money = givenmoney;
		}

		int taketurn(int curBet, int stage) override {
			std::cout << "\nIt is now your turn,\n";
			std::cout << "Current bet: $" << curBet << ", You have $"<<money-ourBet<<" and $"<<ourBet<<" on the table.";
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
					std::cout << "Sorry, you do not have enough money for that. You will instead raise the bet to $" << money<<"\n\n\n";
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

	private:
		std::string userInput;
};

class aiPlayer : public player {
	public:
		
		aiPlayer(Cards* givenHand, int givenmoney, int givenaggresiveness, std::string givenName) :player(givenHand) {
			name = givenName;
			money = givenmoney;
			aggressiveness = givenaggresiveness;
			maxBet = -1;
		}

		void initializeMaxBet() {
			//Essentially, average the aggressiveness, a random multiplier, and hand value for the max amount of money we will bet
			//(also, its 'fuck you' math time xddddd)
			maxBet = (((double)(aggressiveness + (rand() % 100)) + (calcHandValue(hand, handsize)/2))/700) * money;
			//std::cout << "AI max bet: " << maxBet <<"Aggressiveness: "<<aggressiveness<<"\n";//debug line
		}	

		int taketurn(int curBet, int stage) {
			std::cout << "It is AI " << name << "'s turn. The current bet to match is $" << curBet << ", they have $"<<money-ourBet<<" and $"<<ourBet<<" on the table.\n";
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
				int raiseby = (int)((double)distance * ((double)aggressiveness / (150 + (rand()%100))));
				raiseby += rand()%10;
				ourBet = curBet + raiseby;
				std::cout << "They raised $" << (int)raiseby << ", bringing the current bet to match to $" << curBet + raiseby << "\n\n";
				return(curBet + raiseby);
			} else if (curBet < maxBet){
				double chance = (double)curBet/(double)maxBet;
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

	private:
		int maxBet;
		int aggressiveness;//0-100	
};

class poker {
	public:

		poker() {
			ourDeck = new deck();
			ourDeck->shuffle();
			stageOfGame = -1;
			int currentPlayer = 0;
			currentBet = 0;

		}

		int checkContinuePlay(int flag) {
			if (flag) {
				std::cout << "Would you like to play another round? You have $"<<players[playercount - 1]->money<<". y/n\t\t";
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
					players = new player * [playercount];
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

		void endOfRound(int winner) {
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

		void playRound() {
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
								}else {
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
							if (stageOfGame  == 1) {
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

		void playGame() {
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


	private:
		deck* ourDeck;

		int currentBet;
		int stageOfGame;

		int currentPlayer;
		int playercount;
		player** players;
};










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