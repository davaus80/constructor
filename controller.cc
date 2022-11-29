#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <algorithm>
#include "board.h"
#include "illegalmoveexn.h"
#include "controller.h"

// Returns str but with color modification
std::string col(int colNum, std::string str) {
	if(colNum == 0) return "\033[36m" + str + "\033[0m";
	else if(colNum == 1) return "\033[31m" + str + "\033[0m";
	else if(colNum == 2) return "\033[38;5;202m" + str + "\033[0m";
	return "\033[33m" + str + "\033[0m";
}

AbstractController::AbstractController() : board{nullptr} {};

// local function to convert the player number to its corresponding colour
std::string playerColour(int player) {
	if(player == 0) return col(0, "Blue");
	else if(player == 1) return col(1, "Red");
	else if(player == 2) return col(2, "Orange");
	else if (player == 3) return col(3, "Yellow");
	return "<Invalid Player Number>";
}

// returns the resource string corresponding to a given integer
std::string resourceName(int resource) {
	if (resource == 0) return "Brick";
	else if (resource == 1) return "Energy";
	else if (resource == 2) return "Glass";
	else if (resource == 3) return "Heat";
	else if (resource == 4) return "Wifi";
	else return "<Invalid Resource number>";
}

// returns the integer corresponding to a given player colour string
int playerNumFromColour(std::string colour) {
	if (colour == "Blue" || colour == "blue") return 0;
	else if (colour == "Red" || colour == "red") return 1;
	else if (colour == "Orange" || colour == "orange") return 2;
	else if (colour == "Yellow" || colour == "yellow") return 3;
	else throw IllegalMoveExn{"Invalid player colour"};
	return -1;
}

// returns the integer corresponding to a given resource name string
int resourceNumFromStr(std::string resource) {
	if (resource == "brick" || resource == "bricks") return 0;
	else if (resource == "energy") return 1;
	else if (resource == "glass") return 2;
	else if (resource == "heat") return 3;
	else if (resource == "wifi") return 4;
	else throw IllegalMoveExn{"Invalid resource"};
	return -1;
}

std::string clear(int n = 4) {
	std::string clr;
	for(int i = 0; i < n; ++i) clr += "\n";
	return clr;
}

void AbstractController::backupGame(int player, int numOfPlayers) {
	std::string fileName = "backup.sv";
	std::ofstream outFile;
	outFile.open(fileName);
	if (!outFile) {
		std::cout << "There was an error in backing up to the file" << fileName << std::endl;;
		return;
	}
	outFile << std::to_string(player) << "\n";
	for (int i = 0; i < numOfPlayers; i++) {
		std::vector<int> resources = board->getResources(i);
		for (int j = 0; j < resources.size(); j++) {
			outFile << std::to_string(resources[j]) << " ";
		}
		std::vector<int> roads = board->getRoads(i);
		outFile << "r ";
		for (int j = 0; j < roads.size(); j++) {
			outFile << std::to_string(roads[j]) << " ";
		}
		std::vector<std::vector<int>> residences = board->getResidences(i);
		outFile << "h";
		for (int j = 0; j < residences.size(); j++) {
			outFile << " " << std::to_string(residences[j][0]) << " ";
			int bt = residences[j][1];
			if (bt == 0) { outFile << "B"; }
			else if (bt == 1) { outFile << "H"; }
			else if (bt == 2) { outFile << "T"; }
		}
		outFile << std::endl;
	}
	std::vector<std::vector<int>> boardMap = board->getBoard();
	for (int i = 0; i < boardMap.size(); i++) {
		outFile << std::to_string(boardMap[i][0]) << " " << std::to_string(boardMap[i][1]) << " ";
	}
	outFile << std::endl << std::to_string(board->getGoose()) << std::endl;
	std::cout << "Backup saved to " << fileName << std::endl;
	outFile.close();
}

void AbstractController::runGameStart() {	
	// inital residence placing
	int numOfPlayers = 4;
	for(int i = 0; i < numOfPlayers; ++ i) {
		int vertex;
		bool invalid = true;
			
		std::cout << clear() << *board << clear();
		std::cout << "Builder " << playerColour(i) << " where do you want to build a basement?\n>";
                while(invalid == true) {
			if(!(std::cin >> vertex)) {
				if (std::cin.eof()) {
					backupGame(i, numOfPlayers);
					throw IllegalMoveExn{"EOF marker received"};
				}
				std::cin.clear();
				std::cin.ignore(10000, '\n'); 
			 	std::cout << "Invalid input.\nChoose another spot for your basement\n>";
			
			} else if (vertex >= 0 && vertex <= 53) { // check that vertex is in range - will change tho is we are doing dynamic board sizing
				try {
					board->placeRes(vertex, i, true);
					invalid = false;
				} catch(IllegalMoveExn &e) {
					std::cout << e.getMessage() << ".\nChoose another spot for your basement\n>";
				}
			} else std::cout << "Invalid input.\nChoose another spot for your basement\n>"; 
		}
		std::cout << "\n";
	}
	std::cin.ignore();
}

void AbstractController::runGameMid(int startingPlayer, bool loadedSave) {
	int currentPlayer = startingPlayer;
	int numOfPlayers = 4;
	int winner = -1;
	while (winner == -1) {
		if (!loadedSave) {
			std::cout << clear() << *board << clear();
			// beginning of turn(accepts commands fair, loaded, roll)
			std::cout << "Builder " << playerColour(currentPlayer) << "'s turn.\n";
			std::string input;
			int rollVal;
			std::cout << "Please enter a command(roll, fair, load)." << std::endl << ">";
			while(true) {
				getline(std::cin, input);
				if (std::cin.eof()) {
					backupGame(currentPlayer, numOfPlayers);
					throw IllegalMoveExn{"EOF marker received."};
				}
				if(input == "fair" || input == "load") {
					board->setDie(currentPlayer, input);
					std::cout << "Set dice to: " << input << std::endl << ">";
				} else if(input == "roll") {
					rollVal = board->getRoll(currentPlayer);
					break;
				}else {
					std::cout << "Please enter a valid command(roll, fair, load)." << std::endl << ">";
				}
			}
			// if roll == 13 the die is loaded
			if(rollVal == 13) {
				while(true) {
					std::cout << "Input a roll between 2 and 12:\n>";
					if(!(std::cin >> rollVal)) {
						if (std::cin.eof()) {
							backupGame(currentPlayer, numOfPlayers);
							throw IllegalMoveExn{"EOF marker received."};
						}
						std::cin.clear();
						std::cin.ignore(10000, '\n'); 
					} else if (rollVal >= 2 && rollVal <= 12) {
						std::cin.ignore();
						break;
					}
			 		std::cout << "Invalid input.\n";
				}
			}

			if (rollVal == 7) {
				std::cout << "A " << std::to_string(rollVal) << " has been rolled.\n";
				std::vector<std::vector<int>> lossReport = board->goosePenalty();
				for (int i = 0; i < lossReport.size(); i++) {
					int bricks = lossReport[i][0];
					int energy = lossReport[i][1];
					int glass = lossReport[i][2];
					int heat = lossReport[i][3];
					int wifi = lossReport[i][4];
					int totalLoss = bricks + energy + glass + heat + wifi;
					if (totalLoss > 0) {
						std::cout << "Builder " << playerColour(i) << " loses " << std::to_string(totalLoss) << " resources to the geese. They lose:" << std::endl;
						if(bricks > 0) { std::cout << std::to_string(bricks) << " bricks" << std::endl; }
						if(energy > 0) { std::cout << std::to_string(energy) << " energy" << std::endl; }
						if(glass > 0) { std::cout << std::to_string(glass) << " glass" << std::endl; }
						if(heat > 0) { std::cout << std::to_string(heat) << " heat" << std::endl; }
						if(wifi > 0) { std::cout << std::to_string(wifi) << " wifi" << std::endl; }				
					}	
				}
				std::cout << "Choose where to place the GEESE.\n>";
				int gooseTile;
				while(true) {
					while (!(std::cin >> gooseTile)) {
						if (std::cin.eof()) { 
							backupGame(currentPlayer, numOfPlayers);
							throw IllegalMoveExn{"EOF marker received."};
						}
						std::cin.clear();
						std::cin.ignore(10000, '\n'); 
						std::cout << "Invalid input. Please try again.\n>";	
					}
					if((gooseTile >= 0) && (gooseTile <= 18)) { 
						board->moveGoose(gooseTile);
						break;
					}
					std::cout << "Please enter a valid tile.\n>";
					
				}
				std::vector<int> theftCandidates = board->getNeighbours(gooseTile);
				for (int i = 0; i < theftCandidates.size(); i++) {
					if (board->getNumOfResources(theftCandidates[i]) == 0) {
						theftCandidates.erase(theftCandidates.begin() + i);
						i--;
					}
					else if (theftCandidates[i] == currentPlayer) {
						theftCandidates.erase(theftCandidates.begin() + i);
						i--;
					}
				}
				if (theftCandidates.size() == 0) {
					std::cout << "Builder " << playerColour(currentPlayer) << " has no builders to steal from." << std::endl;
				}
				else {
					std::cout << "Builder " << playerColour(currentPlayer) << " can choose to steal from:";
					for (int i = 0; i < theftCandidates.size(); i++) {
						std::cout<< " "  << playerColour(theftCandidates[i]);
					}
					std::cout << "." << std::endl;
					std::cout << "Choose a builder to steal from." << std::endl << ">";
					bool stealSucceeded = false;
					while (!stealSucceeded) {
						std::string stealFromName;
						int stealFrom = -1;
						if (std::cin >> stealFromName) {
							if (std::cin.eof()) {
								backupGame(currentPlayer, numOfPlayers);
								throw IllegalMoveExn{"EOF marker received."};
							}
							try {
								stealFrom = playerNumFromColour(stealFromName);
							} catch (IllegalMoveExn &e) {
								std::cout << e.getMessage() << std::endl;
							}
							if (!(std::count(theftCandidates.begin(), theftCandidates.end(), stealFrom))) {
								std::cout << "Please enter a valid player number." << std::endl;
							} else {
								try {
									int resTaken = board->stealResource(stealFrom, currentPlayer);
									stealSucceeded = true;
									std::cout << "Builder " << playerColour(currentPlayer) << " steals " << resourceName(resTaken) << " from builder " << playerColour(stealFrom) << "." << std::endl;
								}
								catch (IllegalMoveExn &e) {
									std::cout << e.getMessage() << std::endl;;
								}
							}
						}
						else {
							if (std::cin.eof()) {
								backupGame(currentPlayer, numOfPlayers);
								throw IllegalMoveExn{"EOF marker received."};
							}
							std::cout << "Please enter a valid player number." << std::endl;
						}
					}
				}
			} else {
				// print roll and reset screen
				std::cout << clear(2) << playerColour(currentPlayer) << col(currentPlayer, "'s Turn...\n") << *board << clear(1);
				std::cout << "A " << std::to_string(rollVal) << " has been rolled." << std::endl;

				//TODO: Add feedback for who received what resources.
				board->rolled(rollVal);
			}
		}
		if (loadedSave) {
			std::cout << "Game successfully loaded." << std::endl;
			std::cout << clear(3) << playerColour(currentPlayer) << col(currentPlayer, "'s Turn...\n") << *board << clear(3);
			loadedSave=false;

		}

		// during turn
		std::cout << "Enter any actions you would like to take. Enter 'next' when you are ready to end your turn." << std::endl;
 		std::string firstWord = "";
		std::string inputLine;

		while(firstWord != "next") {
				std::cout << "\nEnter an action: " << std::endl << ">";
				getline(std::cin, inputLine);
				std::istringstream inputSS (inputLine);
				if (std::cin.eof()) {
					backupGame(currentPlayer, numOfPlayers);
					throw IllegalMoveExn("EOF marker received.");
				}
				if (!(inputSS >> firstWord)) {
					std::cout << "Please enter a valid input. Enter 'help' to see a list of valid commands" << std::endl;
				}
				if(firstWord == "board" || firstWord == "clear") std::cout << clear(3) << playerColour(currentPlayer) << col(currentPlayer, "'s Turn...\n") << *board << clear(3);
				else if(firstWord == "status") {
					std::cout << "Format: bricks/energy/glass/heat/wifi/roads/buildings" << std::endl;
					for (int i = 0; i < numOfPlayers; i++) {
						std::vector<int> resources = board->getResources(i);
						for (int j = 0; j < resources.size(); j++) {
							std::cout << std::to_string(resources[j]) << " ";
						}
						std::vector<int> roads = board->getRoads(i);
						std::cout << "r ";
						for (int j = 0; j < roads.size(); j++) {
							std::cout << std::to_string(roads[j]) << " ";
						}
						std::vector<std::vector<int>> residences = board->getResidences(i);
						std::cout << "h";
						for (int j = 0; j < residences.size(); j++) {
							std::cout << " " << std::to_string(residences[j][0]) << " ";
							int bt = residences[j][1];
							if (bt == 0) { std::cout << col(i, "B"); }
							else if (bt == 1) { std::cout << col(i, "H"); }
							else if (bt == 2) { std::cout << col(i, "T"); }
						}
						std::cout << std::endl;
					}
				}
				else if(firstWord == "residences") {
					std::vector<std::vector<int>> resVec = board->getResidences(currentPlayer);
					std::cout << "Current player's residences: ";
					for (int i = 0; i < resVec.size(); i++) {
						std::cout << std::endl;
						std::cout << std::to_string(resVec[i][0]) << " ";
						int bt = resVec[i][1];
						if (bt == 0) { std::cout << "B"; }
						else if (bt == 1) { std::cout << "H"; }
						else if (bt == 2) { std::cout << "T"; }
					}
					std::cout << std::endl;
				} else if(firstWord == "build-road") {
					try{
						std::string secondWord;
						if (!(inputSS >> secondWord)) {
							std::cout << "Invalid input. Use like: build-road <edge#>" << std::endl;
							continue;
						}
						int edge = stoi(secondWord);
						if (edge >= 0 && edge <= 71) {
						try {
							board->buildRoad(edge, currentPlayer);
							std::cout << clear(2) << playerColour(currentPlayer) << col(currentPlayer, "'s Turn...\n") << *board << clear(1);
						} catch(IllegalMoveExn &e) {
							std::cout << e.getMessage() << "\n";
						}
						} else std::cout << "Invalid road spot.\n";
					} catch(std::invalid_argument &e) {
						std::cout << "Please enter a valid input\n";
					}
			
				} else if(firstWord == "build-res" || firstWord == "improve") {
					try{
						int vertex;
						std::string secondWord;
						if (!(inputSS >> secondWord)) {
							if (firstWord == "build-res") {
								std::cout << "Invalid input. Use like: build-res <housing#>" << std::endl;
							} else {
								std::cout << "Invalid input. Use like: improve <housing#>" << std::endl;
							}
							continue;
						}
						if (firstWord == "build-res") vertex = stoi(secondWord);
					else if (firstWord == "improve") vertex = stoi(secondWord);
						if (vertex >= 0 && vertex <= 53) {
						try {
							if(board->placeRes(vertex, currentPlayer)) {
								winner = currentPlayer;
								break;
							}	
							std::cout << clear(2) << playerColour(currentPlayer) << col(currentPlayer, "'s Turn...\n") << *board << clear(1);
						} catch(IllegalMoveExn &e) {
							std::cout << e.getMessage() << "\n";
						}
						} else std::cout << "Invalid building spot.\n";
					} catch(std::invalid_argument &e) {
						std::cout << "Please enter a valid input\n";
					}
				} else if(firstWord == "trade") {
					try {
						std::string convert;
						int player2, give, take;
						inputSS >> convert; // check for first
						player2 = playerNumFromColour(convert);
						inputSS >>  convert;
						give = resourceNumFromStr(convert);
						inputSS >>  convert;
						take = resourceNumFromStr(convert);
						std::string response = "";
						while(response != "yes" && response != "no") {
							std::cout << playerColour(currentPlayer) << " offers " << playerColour(player2) << " one " << resourceName(give) << " for one " << resourceName(take) << ".\nDoes " << playerColour(player2) << " accept? (yes/no)\n" << ">";
							std::cin >> response;
							if (std::cin.eof()) {
								backupGame(currentPlayer, numOfPlayers);
								throw IllegalMoveExn{"EOF marker received"};
							}
							std::cin.ignore();
							if(response == "yes") {
								try {
									board->trade(currentPlayer, player2, give, take);
									std::cout << "Trade accepted." << std::endl;
								} catch(IllegalMoveExn &e) {
									std::cout << e.getMessage() << "\n";
								}
							} else if (response == "no") {
								std::cout << "Trade declined." << std::endl;
							} else {
								std::cout << "Please enter yes or no." << std::endl;
							}
						} 
					} catch(IllegalMoveExn &e) {
						std::cout << e.getMessage() << "\n";
					}
				} else if(firstWord == "save") {
					std::string fileName;
					inputSS >> fileName;
					std::ofstream outFile;
					outFile.open(fileName);
					if (!outFile) {
						std::cout << "There was an error in writing to the file" << fileName << std::endl;;
						std::cout << "Please ensure that the file name does not contain spaces and that the file is accessible." << std::endl;
						continue;
					}
					outFile << std::to_string(currentPlayer) << "\n";
					for (int i = 0; i < numOfPlayers; i++) {
						std::vector<int> resources = board->getResources(i);
						for (int j = 0; j < resources.size(); j++) {
							outFile << std::to_string(resources[j]) << " ";
						}
						std::vector<int> roads = board->getRoads(i);
						outFile << "r ";
						for (int j = 0; j < roads.size(); j++) {
							outFile << std::to_string(roads[j]) << " ";
						}
						std::vector<std::vector<int>> residences = board->getResidences(i);
						outFile << "h";
						for (int j = 0; j < residences.size(); j++) {
							outFile << " " << std::to_string(residences[j][0]) << " ";
							int bt = residences[j][1];
							if (bt == 0) { outFile << "B"; }
							else if (bt == 1) { outFile << "H"; }
							else if (bt == 2) { outFile << "T"; }
						}
						outFile << std::endl;
					}
					std::vector<std::vector<int>> boardMap = board->getBoard();
					for (int i = 0; i < boardMap.size(); i++) {
						outFile << std::to_string(boardMap[i][0]) << " " << std::to_string(boardMap[i][1]) << " ";
					}
					outFile << std::endl << std::to_string(board->getGoose()) << std::endl;
					std::cout << "File saved to " << fileName << std::endl;
					outFile.close();
				} else if(firstWord == "help") {
					std::cout << "\nValid commands:\nboard\nstatus\nresidences\nbuild-road <edge#>\nbuild-res <housing#>\nimprove <housing#>\n";
					std::cout << "trade <colour> <give> <take>\nnext\nsave <file>\nhelp\nclear\n";
				} else if (firstWord == "devGive") {
					std::string resource;
					int amount;
					if (!(inputSS >> resource) || (!(inputSS >> amount) && resource != "stack")) {
						std::cout << "Invalid use of function. Use like: devGive <resourceName> <amount>" << std::endl;
						continue;
					}
					try {
						if(resource == "stack") {
							for(int i = 0; i < 5; ++i) {
								board->giveResources(currentPlayer, i, 100);
							}
						} else {
							int resourceNum = resourceNumFromStr(resource);
							board->giveResources(currentPlayer, resourceNum, amount);
						}
					} catch (IllegalMoveExn &e) {
						std::cout << e.getMessage() << std::endl;
					}							
				} else if(firstWord == "next") { break; }
				else {
					std::cout << "Please enter a valid command. Enter 'help' to see a list of valid commands." << std::endl;;
				}
		}
		currentPlayer = (currentPlayer + 1) % 4;
		std::cout << "\n";
	}
	std::cout << clear(2) <<  *board << clear(1);
	int previousPlayer = (currentPlayer + 3) % 4;
	std::cout << "Congratulations " << playerColour(previousPlayer) << " wins!!\n";
}



StandardController::StandardController() :
	AbstractController() {}

std::unique_ptr<AbstractBoard> StandardController::getUniquePtrSeeded(std::string seed) {
	return std::make_unique<StandardBoard>(seed);
}

void StandardController::beginSeeded(std::string seed) {
	try {
		auto temp = getUniquePtrSeeded(seed);
		board = std::move(temp);
	}
	catch (IllegalMoveExn &e) {
		std::cout << e.getMessage() << std::endl;
		return;
	}
	runGameStart();
	runGameMid();
}

void StandardController::loadGame(std::string fileName) {
	int numPlayers = 4;
	int numResources = 5;
	int numTiles = 19;
	std::string badFormat = "Invalid load file format";
	std::ifstream input;
	input.open(fileName);
	if (!input) {
		input.close();
		throw IllegalMoveExn("Invalid load file.");
		//return;
	}
	int curTurn;
	// vector of player vectors, containing resources, then roads, then each building type in order
	std::vector<std::vector<std::vector<int>>> playerData;
	std::vector<std::vector<int>> boardData; //ordered pairs of (resource, value)
	std::string currLine;
	std::string readVal;
	// ########################################## First line: Current player ######################################################
	if (!(std::getline(input,currLine))) {
		throw IllegalMoveExn{"Input file is empty"};
	}
	std::stringstream inputSS(currLine);
	if (!(inputSS >> readVal)) { throw IllegalMoveExn{badFormat + ": Current player must be an integer."}; }
	try {
		curTurn = stoi(currLine);
	} catch (std::invalid_argument &e) {
		throw IllegalMoveExn{(badFormat + ": currentTurn is not an integer")};
	}

	// ################################## Next lines: Each line is data for a different player #####################################
	std::vector<int> residenceLocations; // catches duplicate residences
	std::vector<int> roadLocations; // catches duplicate roads
	for (int i = 0; i < numPlayers; i++) {
		std::vector<std::vector<int>> thisPlayerData;
		if (!(getline(input, currLine))) {
			throw IllegalMoveExn{(badFormat + ": Not enough rows in file.")};
		}
		inputSS.clear();
		inputSS.str(std::string());
		inputSS << currLine;
		int readInt;
		std::vector<int> resourceVec;
		for (int j = 0; j < numResources; j++) {
			if (inputSS >> readVal) {
				try {
					readInt = stoi(readVal);
					resourceVec.push_back(readInt);
				} catch(std::invalid_argument &e) {
					throw IllegalMoveExn{(badFormat + ": " + resourceName(j)  + " amount is not an integer for " + playerColour(i))};
				}
			} else {
				throw IllegalMoveExn{(badFormat + ": incorrect number of resources.") };
			}
		}
		thisPlayerData.push_back(resourceVec);
		bool checkingRoads = false;
		bool checkingHousing = false;
		bool roadsChecked = false;
		bool housingChecked = false;
		std::vector<int> roadVec;
		std::vector<int> basementVec;
		std::vector<int> houseVec;
		std::vector<int> towerVec;
		while (inputSS >> readVal) {
			if (readVal == "r") {
				checkingRoads = true;
				roadsChecked = true;
			} else if (checkingRoads) {
				if (readVal =="h") {			
					checkingRoads = false;
					checkingHousing = true;
				} else {
					try {
						readInt = stoi(readVal);
						if (std::count(roadLocations.begin(), roadLocations.end(), readInt)) {
							throw IllegalMoveExn{badFormat + ": Duplicate roads at edge " + std::to_string(readInt)};
						} else if ((readInt < 0) || readInt > 71) {
							throw IllegalMoveExn{badFormat + ": Invalid road location '" + std::to_string(readInt) + "'"};
						}
						roadLocations.push_back(readInt);
						roadVec.push_back(readInt);
					} catch (std::invalid_argument &e) {
						throw IllegalMoveExn{badFormat + ": Road location is not an integer for " + playerColour(i)};
					}
				}
			} else if (checkingHousing) {
				std::string bt;
				if (!(inputSS >> bt)) {
					throw IllegalMoveExn{badFormat + ": Must specify building type for all residences."};
				}
				try {
					readInt = stoi(readVal);
					if (std::count(residenceLocations.begin(), residenceLocations.end(), readInt)) {
						throw IllegalMoveExn{badFormat + ": Duplicate residences at vertex " + std::to_string(readInt)};
					} else if ((readInt < 0) || readInt > 53) {
						throw IllegalMoveExn{badFormat + ": Invalid residence location '" + std::to_string(readInt) + "'"};
					}
					residenceLocations.push_back(readInt);
					if (bt == "B") { basementVec.push_back(readInt); }
					else if (bt == "H") { houseVec.push_back(readInt); }
					else if (bt == "T") { towerVec.push_back(readInt); }
					else { 
						throw IllegalMoveExn{badFormat + ": Invalid building type " + bt};
					}
					housingChecked = true; // each player must have at least one residence
				} catch (std::invalid_argument &e) {
					throw IllegalMoveExn{badFormat + ": Error with adding residence at " + readVal};
				}				
			} else {
				throw IllegalMoveExn{badFormat + ": must follow resources with 'r' then road edges, then 'h' and residences"};
			}
		}
		if (!(housingChecked && roadsChecked)) {
			throw IllegalMoveExn{ badFormat + "Missing housing or roads data for " + playerColour(i)};
		}

		std::vector<std::vector<int>> buildingData;
		thisPlayerData.push_back(roadVec);
		thisPlayerData.push_back(basementVec);
		thisPlayerData.push_back(houseVec);
		thisPlayerData.push_back(towerVec);
		playerData.push_back(thisPlayerData);
	}
	// ###################################### Next Row: Board layout ######################################################
	if (!(std::getline(input, currLine))) { throw IllegalMoveExn{badFormat + ": Missing board data "}; }
	board = getUniquePtrSeeded(currLine);
	// ##################################### Last Row: Goose location #####################################################
	int gooseVal;
	if (!(input >> currLine)) {
		throw IllegalMoveExn{badFormat + ": Missing goose location"};
	}
	try { 
		gooseVal = stoi(currLine);
		if(gooseVal < 0 || gooseVal > 18) throw IllegalMoveExn("Goose location out of bounds");
	} catch (std::invalid_argument &e) {
		throw IllegalMoveExn{badFormat + ": Goose location must be an integer" };
	}
	// ##################################### Add residences and goose #####################################################
	// set goose
	board->moveGoose(gooseVal);
	// set  residences
	for (int player = 0; player < numPlayers; player++) {
		for (int i = 0; i < numResources; i++) { // Load resources
			board->giveResources(player, i, playerData[player][0][i]);
		}
		for (int i = 0; i < playerData[player][1].size(); i++) { // Load roads
			board->giveRoadFree(playerData[player][1][i], player);
		}
		for (int i = 2; i < 5; i++) { // Load residences
			for (int j = 0; j < playerData[player][i].size(); j++) {
				board->giveResidenceFree(playerData[player][i][j], (i - 2), player);
			}
		}
	}
	runGameMid(curTurn, true);
}
void AbstractController::setSeed(std::string seedStr) {
	seed = seedStr;
}





