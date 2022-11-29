#ifndef _DIE_H_
#define _DIE_H_

// Abstract base class for a die.
class AbstractDie {
      public:
	virtual int roll() = 0;
};

// Concrete subclass of AbstractDie representing rolling two random dice
class FairDie: public AbstractDie {
       public:		
	int roll() override; // returns a random number modelling the roll of two standard dice
};

// Concrete subclass of Abstract die.
class LoadedDie: public AbstractDie {
       public:
	int roll() override; // returns a value outside range two-twelve
};

#endif
