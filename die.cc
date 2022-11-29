#include "die.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

int FairDie::roll()  {
	int rollNum;
	rollNum = rand() % 6 + 1;
	rollNum = rollNum + (rand() % 6 + 1);
	return rollNum;
	
}

int LoadedDie::roll() {
	return 13;
}
