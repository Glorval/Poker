# include <iostream>
#include "poker.h"
#include "GlorwynMemory.h"
#include <stdlib.h>
#include <time.h>
const int allCards = -1;



int main() {
	Cards* testhand = (Cards*)gcalloc(5, sizeof(Cards));
	testhand[0].data = (unsigned char*)calloc(2, 4);
	testhand[1].data = (unsigned char*)calloc(2, 4);
	testhand[2].data = (unsigned char*)calloc(2, 4);
	testhand[3].data = (unsigned char*)calloc(2, 4);
	testhand[4].data = (unsigned char*)calloc(2, 4);
	testhand[0].data[0] = diamonds;
	testhand[0].data[1] = seven;
	testhand[1].data[0] = diamonds;
	testhand[1].data[1] = five;
	testhand[2].data[0] = diamonds;
	testhand[2].data[1] = five;
	testhand[3].data[0] = diamonds;
	testhand[3].data[1] = three;
	testhand[4].data[0] = spades;
	testhand[4].data[1] = three;

	findValue(testhand);


	deck testdeck;
	testdeck.shuffle();
	humanPlayer testplayer(testdeck.drawCards(5), 1000);
	testplayer.printHand();
	//findHandValue(testplayer.hand);


	//poker newGame;
	//newGame.playGame();
}