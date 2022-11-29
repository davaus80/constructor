#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#include "illegalmoveexn.h"
#include "controller.h"

// getStandardController returns a unique pointer of type AbstractController
// to an instance of StandardController
std::unique_ptr<AbstractController> getStandardController() {
	return std::move(std::make_unique<StandardController>());
}

// randomBoardString generates string defining a random layout for a standard board (that is,
// a board with nineteen hexagonal tiles and a fixed number of each resource/roll.
std::string randomBoardString() {
	std::vector<int> resources = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 7};
	std::vector<int> values = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};
	std::string board;
	int indexRes, indexVal, reduceIR = 0, reduceIV = 0;
	for(int i  = 0; i < 19; ++i) {
		indexRes = rand() % (19 - reduceIR);
		indexVal = rand() % (18 - reduceIV);
		if(resources[indexRes] == 7) {
			board.append("5 7 ");
		} else {
			board.append(std::to_string(resources[indexRes]) + " ");
			board.append(std::to_string(values[indexVal]) + " ");
			values.erase(values.begin() + indexVal);
			++reduceIV;
		}
		resources.erase(resources.begin() + indexRes);
		++reduceIR;
	}
	return board;
}

// This is the main function that is called when the constructor executable is run. It assesses
// the command line arguments and calls the controller accordingly.
int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cout << "Please call command like: ./constructor [-seed <seed>] [-load <fileName>] [-board <fileName>] [-random-board]" << std::endl;
		return 1;
	}
	std::string load, boardSeed = "", arg, loadFile;
	bool randomBoard = false, isSeed = false, isLoad = false;
	for(int argi = 1; argi < argc; ++argi) {
		arg = argv[argi];
		if(arg == "-seed") {
			if(argi != argc - 1) {
				++argi;
				arg = argv[argi];
			} else {
				std::cout << "Invalid seed.\n";
				return 1;
			}
			try {
				srand(std::stoi(arg));
				isSeed = true;
			} catch(std::invalid_argument &e) {
				std::cout << "Invalid seed.\n";
				return 1;
			}
		} else if (arg == "-load") { 
			if (argi != argc - 1) {
				++argi;
				loadFile = argv[argi];
				isLoad = true;
			} else {
				std::cout << "Please provide a load file.\n";
				return 1;
			}
		} else if(arg == "-board") {
			if(argi != argc - 1) {
				++argi;
				arg = argv[argi];
			} else {
				std::cout << "Invalid seed.\n";
				return 1;
			}
			std::ifstream boardLoadFile(arg);
			if(!boardLoadFile) {
				std::cout << "Invalid board seed file.\n" << std::endl;
				return 1;
			}
			getline(boardLoadFile, boardSeed);
			if (boardSeed == "") {
				std::cout << "Board seed file is empty.\n" << std::endl;
				return 1;
			}
		} else if(arg == "-random-board") {
			randomBoard = true;
		}
		else {
			std::cout << "Invalid Input.\n";
			return 1;
		}
	}
	if(!isSeed) srand(time(NULL));
	if(randomBoard && !isLoad && boardSeed == "") boardSeed = randomBoardString();
	std::unique_ptr<AbstractController> gameController = nullptr;
	gameController = getStandardController();
	if (isLoad) {
		try {
			gameController->loadGame(loadFile);
		} catch (IllegalMoveExn &e) {
			std::cout << e.getMessage() << std::endl;
			return 1;
		}
	} else {
		try {
			gameController->setSeed(boardSeed);
			gameController->beginSeeded(boardSeed);
		} catch (IllegalMoveExn &e) {
			std::cout << e.getMessage() << std::endl;
			return 1;
		}
	}
	std::cout << "\n";
	std::string response = "";
	while(response != "yes" && response != "no") {
		std::cout << "Do you want to play again?\n>";
		getline(std::cin, response);
		if(response == "yes") { 
			if(randomBoard || isLoad) boardSeed = randomBoardString();
			gameController->beginSeeded(boardSeed);
		}
		else if(response != "no") std::cout << "Invalid input. Input yes or no.\n>";
	}
	return 1;
}
