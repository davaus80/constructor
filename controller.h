#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <memory>
#include <string>
#include "die.h"
#include "board.h"

// #########################################################################
// AbstractController and its concrete subclasses act as the conduit between
// the user and the game board in the Constructor game. Controller manages
// the setup and the execution of the game.
// #########################################################################

class AbstractController {
       protected:
	std::unique_ptr<AbstractBoard> board; // Points to the game board
	std::string seed; // random seed
	void backupGame(int player, int numOfPlayers); // saves the game to a file, backup.sv 
       public:
	AbstractController();
	virtual void beginSeeded(std::string seed) = 0; //begins the game from a board layout string, seed
	virtual void loadGame(std::string fileName) = 0; // begins the game from the information in the file at fileName
	void runGameStart(); // runs the placing of the residences
	void runGameMid(int startingPlayer = 0, bool loadedSave = false); // manages players' turns until the game is won
	void setSeed(std::string seedStr); // sets the random seed for the game
};

// StandardController is a concrete sublass of AbstractController that uses StandardBoard
// as its board type.
class StandardController: public AbstractController {
      public:
	StandardController();
	std::unique_ptr<AbstractBoard> getUniquePtrSeeded(std::string seed);
	void beginSeeded(std::string seed) override;
	void loadGame(std::string fileName) override;
};


#endif
