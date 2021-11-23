# include <iostream>
#include "poker.h"
#include "GlorwynMemory.h"
#include <stdlib.h>
#include <time.h>

//NOTE - I realize this is pretty big, definitely way bigger than it needed to be...
//oh well, this is something I wanted to make at least a basic version of at some point regardless, so here it is!
//Now, I know this was for an assignment to show polymorphism, so I'll point you to where that is such that you
//do not need to hunt for it. The cards.h file defines a 'player' class and in poker.h two classes derive from it,
//aiPlayer and humanPlayer. These then are both used in a player array roughly at line 185 of poker.h
//Then later on this array gets cycled through (While/for loops at 228 or so) and have several occurances
//of being treated solely as a player object. Such as retriving their hand at line 235 or calling 'take turn' at line 247.
// 
// 
//Overall, this is definitely not top QUALITY as there are a lot of hotfixed stuff and it's kinda a bit shakily held together,
//but that is mostly a product of going 'o hecc I didnt get my experimental code off the pc in the classroom' and having to
//make sure I could add that in after, and of course coupled with the fact that I thought I had more time to do this assignment
//than I realized, thus meaning that I had to just /get it working/ before trying to make it look nice, polishing inputs, adding 
//accessor methods and such...
//ALSO - I did not separate the parts of a class between the header and the cpp file, I know how to do this but once again...
//... time... 


int main() {
	poker newGame;
	newGame.playGame();
}