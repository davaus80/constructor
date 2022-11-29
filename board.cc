#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>
#include "tile.h"
#include "edge.h"
#include "vertex.h"
#include "player.h"
#include "illegalmoveexn.h"
#include "board.h"

 
StandardBoard::StandardBoard(std::string boardLayout) :
	AbstractBoard()
	{
	int parkTile = -1;
	int n = 3;
	std::stringstream s(boardLayout);
	int value;
	int num_tiles = 0;
	// Load in tiles
	while (s>>value) {
		int resource = value;
		if (s>>value) {
			int roll = value;
			if (roll == 7) {
				parkTile = num_tiles;
			}
			std::unique_ptr<Tile> temp = std::make_unique<Tile>(this, num_tiles, resource, roll, false);
			tiles.push_back(std::move(temp));
			num_tiles++;
		}
		else {
			throw IllegalMoveExn{"Input file has odd number of values"};
		}
	}
	if (num_tiles != 19) {
		std::string message = "Input file has incorrect number of tiles. Expected: 19 Received: " + std::to_string(num_tiles);		
		throw IllegalMoveExn{message};
		}
	int sumN = 0;
	for (int i = 1; i < n; i++) {
		sumN += n;
	}
	if (parkTile < 0) {
		std::vector<int> tileList;
		for (int i = 0; i < num_tiles; i++) {
			tileList.push_back(i);
		}
		std::random_shuffle(tileList.begin(), tileList.end());
		parkTile = tileList[0];
	}
	tiles[parkTile]->setGoose(true);
	goose = parkTile;
	// Add vertices and edges 
	int num_vertices = 54;
	for (int i = 0; i < num_vertices; i++) {
		std::unique_ptr<Vertex> temp = std::make_unique<Vertex>(this,i, -1, -1);
		vertices.push_back(std::move(temp));
	}
	int num_edges = 72;
	for (int i = 0; i < num_edges; i++) {
		std::unique_ptr<Edge> temp = std::make_unique<Edge>(this, i, false, -1);
		edges.emplace_back(std::move(temp));
	}
	// Now we will connect all tiles, vertices, and edges as needed
	// We will start with connecting everything to its tiles
	
	// For now, we will hard code for the default board.
	// Connect all tiles and vertices to the correct tiles.
	tiles[0]->connectAllVertices(0,1,3,4,8,9);
	tiles[1]->connectAllVertices(2,3,7,8,13,14);
	tiles[2]->connectAllVertices(4,5,9,10,15,16);
	tiles[3]->connectAllVertices(6,7,12,13,18,19);
	tiles[4]->connectAllVertices(8,9,14,15,20,21);
	tiles[5]->connectAllVertices(10,11,16,17,22,23);
	tiles[6]->connectAllVertices(13,14,19,20,25,26);
	tiles[7]->connectAllVertices(15,16,21,22,27,28);
	tiles[8]->connectAllVertices(18,19,24,25,30,31);
	tiles[9]->connectAllVertices(20,21,26,27,32,33);
	tiles[10]->connectAllVertices(22,23,28,29,34,35);
	tiles[11]->connectAllVertices(25,26,31,32,37,38);
	tiles[12]->connectAllVertices(27,28,33,34,39,40);
	tiles[13]->connectAllVertices(30,31,36,37,42,43);
	tiles[14]->connectAllVertices(32,33,38,39,44,45);
	tiles[15]->connectAllVertices(34,35,40,41,46,47);
	tiles[16]->connectAllVertices(37,38,43,44,48,49);
	tiles[17]->connectAllVertices(39,40,45,46,50,51);
	tiles[18]->connectAllVertices(44,45,49,50,52,53);
	tiles[0]->connectAllEdges(0,1,2,6,7,10);
	tiles[1]->connectAllEdges(3,5,6,13,14,18);
	tiles[2]->connectAllEdges(4,7,8,15,16,19);
	tiles[3]->connectAllEdges(9,12,13,20,21,26);
	tiles[4]->connectAllEdges(10,14,15,22,23,27);
	tiles[5]->connectAllEdges(11,16,17,24,25,28);
	tiles[6]->connectAllEdges(18,21,22,30,31,35);
	tiles[7]->connectAllEdges(19,23,24,32,33,36);
	tiles[8]->connectAllEdges(26,29,30,37,38,43);
	tiles[9]->connectAllEdges(27,31,32,39,40,44);
	tiles[10]->connectAllEdges(28,33,34,41,42,45);	
	tiles[11]->connectAllEdges(35,38,39,47,48,52);
	tiles[12]->connectAllEdges(36,40,41,49,50,53);
	tiles[13]->connectAllEdges(43,46,47,54,55,60);
	tiles[14]->connectAllEdges(44,48,49,56,57,61);
	tiles[15]->connectAllEdges(45,50,51,58,59,62);
	tiles[16]->connectAllEdges(52,55,56,63,64,67);
	tiles[17]->connectAllEdges(53,57,58,65,66,68);
	tiles[18]->connectAllEdges(61,64,65,69,70,71);
	for (int i = 0; i < num_tiles; i++) {
		tiles[i]->connectComponents();
	}
	//Add players
	std::string colours[] = {"Blue","Red","Orange","Yellow"};
	for(int i = 0; i < 4; i++) {
		std::unique_ptr<Player> temp = std::make_unique<Player>(colours[i],0,0,0,0,0,0);
		players.push_back(std::move(temp));
	}
}



std::string StandardBoard::getBoardType() {
	return "Standard Board";
}

AbstractBoard::AbstractBoard()
	: goose{0} {}

bool AbstractBoard::placeRes(int vertexNum, int playerNum, bool isBeginning) {
	// check if spot already has a Tower
	if(vertices[vertexNum]->getBuildingType() == 2) throw IllegalMoveExn("You cannot build here");
        // check if space has a building but you not owned by player building
	else if(vertices[vertexNum]->getBuildingType() >= 0 && vertices[vertexNum]->getPlayer() != playerNum) throw IllegalMoveExn("You cannot build here");
	// check to make sure there are no adjacent verticies
	else if(vertices[vertexNum]->hasAdjRes(playerNum)) throw IllegalMoveExn("You cannot build here");
	// if it's not the beginning check if there is an ajacent road owned by the player
	else if(!isBeginning && !vertices[vertexNum]->hasAdjRoad(playerNum)) throw IllegalMoveExn("You cannot build here");
	// if the player has sufficent resources
	else if(!isBeginning && !players[playerNum]->sufficientResources(vertices[vertexNum]->getBuildingType() + 1)) throw IllegalMoveExn("You do not have enough resources.");
	// build or ugrade the residence
	else {
		vertices[vertexNum]->upgrade(playerNum);
		players[playerNum]->build(vertices[vertexNum]->getBuildingType(), vertexNum, isBeginning);
	}
	if(players[playerNum]->getPoints() == 10) return true;
	return false;
}

void AbstractBoard::buildRoad(int edgeNum, int playerNum) {
	if(edges[edgeNum]->getHasRoad()) throw IllegalMoveExn("You cannot build here");
	else if(!edges[edgeNum]->hasAdjBuild(playerNum)) throw IllegalMoveExn("You cannot build here");
	else if(!players[playerNum]->sufficientResources(3)) throw IllegalMoveExn("You do not have enough resources.");
	else {
		players[playerNum]->addRoad(edgeNum);
		edges[edgeNum]->buildRoad(playerNum);
	}
}

int AbstractBoard::getRoll(int playerNum) const {
	return players[playerNum]->roll();
}

void AbstractBoard::setDie(int player, std::string type) {
	players[player]->setDie(type);
}

void AbstractBoard::rolled(int num) {
	//this assumes that the goose has already been dealt with
	//notify tiles that they've been rolled
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i]->getValue() == num) {
			tiles[i]->rolled();
		}
	}
}

void AbstractBoard::residenceRolled(int vertex, int resource) {
	int player = vertices[vertex]->getPlayer();
	int bt = vertices[vertex]->getBuildingType();
	if (player >= 0) {
		players[player]->addResources(resource, (bt + 1));
	}
}

bool AbstractBoard::roadAdjToVertexExcluding(int vertexNum, int playerNum, int excludingEdge) const {
	if(vertices[vertexNum]->hasAdjRoad(playerNum, excludingEdge) && vertices[vertexNum]->getBuildingType() == -1) return true;
	return false;
}
	

// TODO: add these methods being called to vertex class
bool AbstractBoard::hasAdjRes(int vertexNum) const {
	if(vertices[vertexNum]->getBuildingType() >= 0) return true;
	return false;
}

bool AbstractBoard::hasOwnedRoad(int edgeNum, int playerNum) const {
	if (edges[edgeNum]->getHasRoad() && edges[edgeNum]->getPlayer() == playerNum) {
		return true;
	}
	return false;
}

bool AbstractBoard::hasOwnedRes(int vertexNum, int playerNum) const {
	if(vertices[vertexNum]->getBuildingType() >= 0 && vertices[vertexNum]->getPlayer() == playerNum) {
		return true;
	}
	return false;
}

bool AbstractBoard::roadAdjToVertex(int vertex, int player) const {
	return vertices[vertex]->hasAdjRoad(player);
}

void AbstractBoard::trade(int player1, int player2, int resource1, int resource2) {
	if(players[player1]->getNumResource(resource1) < 1 || players[player2]->getNumResource(resource2) < 1) throw IllegalMoveExn("Insufficient resources for trade.");
	players[player1]->removeResources(resource1, 1);
	players[player2]->addResources(resource1, 1);
	players[player2]->removeResources(resource2, 1);
	players[player1]->addResources(resource2, 1);
}
	
int AbstractBoard::moveGoose(int newTile) {
	tiles[goose]->setGoose(false);
	goose = newTile;
	return tiles[goose]->setGoose(true);
}

void AbstractBoard::giveResidenceFree(int vertex, int bt, int player) {
	for (int i = 0; i <= bt; i++) {
		vertices[vertex]->upgrade(player);
	}
	players[player]->addResFree(bt, vertex);
}

void AbstractBoard::giveRoadFree(int edge, int player) {
	edges[edge]->buildRoad(player);
	players[player]->addRoadFree(edge);
}

std::vector<std::vector<int>> AbstractBoard::goosePenalty() {
	std::vector<std::vector<int>> retVec;
	for (int i = 0; i < players.size(); i++) {
		retVec.push_back(players[i]->goosePenaltyPlayer());
	}
	return retVec;
}

std::vector<int> AbstractBoard::getNeighbours (int tile) const {
	return tiles[tile]->getBorderingPlayers();
}

int AbstractBoard::stealResource(int stealFrom, int thief) {
	int resourceStolen = players[stealFrom]->loseRandomResource();
	if (resourceStolen == -1) { throw IllegalMoveExn{"That player has no resources."}; }
	else {
		players[thief]->addResources( resourceStolen, 1);
	}
	return resourceStolen;
}

int AbstractBoard::getNumOfResources(int player) const {
	return players[player]->getNumOfResources();
}

std::vector<std::vector<int>> AbstractBoard::getBoard() const {
	std::vector<std::vector<int>> boardMap;
	for (int i = 0; i < tiles.size(); i++) {
		std::vector<int> tileVec;
		tileVec.push_back(tiles[i]->getResource());
		tileVec.push_back(tiles[i]->getValue());
		boardMap.push_back(tileVec);
	}
	return boardMap;
}

int AbstractBoard::getGoose() const {
	return goose;
}

int AbstractBoard::getBuildingType(int vertex) const {
	return vertices[vertex]->getBuildingType();
}

int AbstractBoard::getVertexOwner(int vertex) const {
	return vertices[vertex]->getPlayer();
}

std::vector<int> AbstractBoard::getResources(int player) const {
	return players[player]->getResources();
}

std::vector<int> AbstractBoard::getRoads(int player) const {
	return players[player]->getRoads();
}

void AbstractBoard::giveResources(int player, int resource, int number) {
	players[player]->addResources(resource, number);
}

std::vector<std::vector<int>> AbstractBoard::getResidences(int player) const {
	return players[player]->getResidences();
}

// print methods and functions

// Returns the string for that color number
std::string col(int colNum) {
	if(colNum == -1) return "\033[0m";
	else if(colNum == 0) return "\033[36m";
	else if(colNum == 1) return "\033[31m";
	else if(colNum == 2) return "\033[38;5;202m";
	else if(colNum == 3) return "\033[33m";
	return "";
}
 
// Returns a string with n spaces
std::string spaces(int n) {
	std::string spaces;
	for(int i = 0; i < n; ++i) {
		spaces += " ";
	}
	return spaces;
}

// Creates a string from an integer, adding a space if it is single digit
std::string numSpace(int n) {
	if(n < 10) return " " + std::to_string(n);
	return std::to_string(n);
}

std::string AbstractBoard::printEdge(int edgeNum, bool horz) const {
	std::string mark;
	if(edges[edgeNum]->getHasRoad()) {
		if(edges[edgeNum]->getPlayer() == 0) mark = "BR";
		else if(edges[edgeNum]->getPlayer() == 1) mark = "RR";
		else if(edges[edgeNum]->getPlayer() == 2) mark = "OR";
		else if(edges[edgeNum]->getPlayer() == 3) mark = "YR";
		if(horz) mark = "--" + mark + "--";
		return col(edges[edgeNum]->getPlayer()) + mark + col(-1);
	}
	else mark = numSpace(edgeNum);
	if(horz) return "--" + mark + "--";
	return mark;
}

std::string AbstractBoard::printVertex(int vertexNum) const {
	std::string mark;
	if(vertices[vertexNum]->getBuildingType() != -1) {
		if(vertices[vertexNum]->getPlayer() == 0) mark += "B";
		if(vertices[vertexNum]->getPlayer() == 1) mark += "R";
		if(vertices[vertexNum]->getPlayer() == 2) mark += "O";
		if(vertices[vertexNum]->getPlayer() == 3) mark += "Y";
		if(vertices[vertexNum]->getBuildingType() == 0) mark += "B";
		if(vertices[vertexNum]->getBuildingType() == 1) mark += "H";
		if(vertices[vertexNum]->getBuildingType() == 2) mark += "T";
		return col(vertices[vertexNum]->getPlayer()) + "|" + mark + "|" + col(-1); 
	} else mark = numSpace(vertexNum);
	return "|" + mark + "|";
}

std::string AbstractBoard::checkGeese(int tileNum) const {
	if(tiles[tileNum]->getGoose()) return spaces(2)  + "GEESE" + spaces(2);
	return spaces(9);
}

void AbstractBoard::connectVerticesWithEdge(int v1, int v2, int edge) {
	vertices[v1]->connectVertex(v2);
	vertices[v1]->connectEdge(edge);
	vertices[v2]->connectVertex(v1);
	vertices[v2]->connectEdge(edge);
	edges[edge]->connectVertex(v1);
	edges[edge]->connectVertex(v2);
}

std::string AbstractBoard::printValue(int tileNum) const {
	if(tiles[tileNum]->getValue() == 7) return spaces(6);
	return spaces(2) + numSpace(tiles[tileNum]->getValue()) + spaces(2);
}

std::string AbstractBoard::printResource(int tileNum) const {
	if(tiles[tileNum]->getResource() == 0) return "BRICK ";
	if(tiles[tileNum]->getResource() == 1) return "ENERGY";
	if(tiles[tileNum]->getResource() == 2) return "GLASS ";
	if(tiles[tileNum]->getResource() == 3) return "HEAT  ";
	if(tiles[tileNum]->getResource() == 4) return "WIFI  ";
	return "PARK  ";
}

std::string AbstractBoard::vertEdge(int edgeNum) const {
	if(edges[edgeNum]->getHasRoad()) return col(edges[edgeNum]->getPlayer()) + "|" + col(-1);
	return "|";	
}


std::string preSpaces(int numEdges) {
	if(numEdges == 1) return spaces(20);
	else if(numEdges == 2) return spaces(10);
	return "";
}

std::ostream& operator<<(std::ostream &out, const AbstractBoard &board) {
	int v = 0, e = 0, t = 0, numEdges, resetT = 0;
	for(int i = 0; i < 11; ++i) {
		if(i == 0 || i == 10) {
			numEdges = 1;
		} else if(i == 1 || i == 9) {
			numEdges = 2;
		} else numEdges = 3;
		// print first row of set of tiles (verticies, edges, and values)
		out << preSpaces(numEdges);
		for(int j = 0; j < numEdges * 2; ++j) {
			out << board.printVertex(v);
			++v;
			if(j != numEdges * 2 - 1) {
				if((numEdges < 3 && j % 2 == 0) || (numEdges == 3 && (i + j) % 2 == 0)) {
					out << board.printEdge(e, true);
					++e;
				}
				else {
					out << board.printValue(t);
					++t;
					++resetT;						
				}
			}
		}
		// resest tile number
		t -= resetT;
		resetT = 0;
		out << "\n";

		// if not the last row print the rest of the tile
		if(i != 10) {
			if(i == 8) numEdges = 2;
			if(i == 9) numEdges = 1;
			// print second row of set of tiles (spacing and |)
			out << preSpaces(numEdges);
			out << spaces(2);
			for(int j = 0; j < 2 * numEdges; ++j) {
				out << board.vertEdge(e + j);
				if(j != 2 * numEdges - 1) {
					if((i < 3 && j % 2 != 0) || (numEdges == 3 && (i + j) % 2 != 0) || (i > 7 && j % 2 == 0)) {
						out << board.checkGeese(t);
						++t;
					} else out << spaces(9);
				}
			}
			out << "\n";

			//print third row of set
			out << preSpaces(numEdges);
			out << spaces(1);
			for(int j = 0; j < 2 * numEdges; ++j) {
				out << board.printEdge(e, false);
				++e;
				if(j != 2 * numEdges - 1) {
					out << spaces(3);
					if((i < 3 && j % 2 == 0) || (numEdges == 3 && (i + j) % 2 == 0) || (i > 7 && j % 2 != 0)) {
						out << numSpace(t);
						++t;
						++resetT;
					} else out << spaces(2);
					out << spaces(3);
				}
			}
			// resest tile number
			t -= resetT;
			resetT = 0;
			out << "\n";
			//print fourth row of set
			out << preSpaces(numEdges);
			out << spaces(2);
			for(int j = 0; j < 2 * numEdges; ++j) {
				out << board.vertEdge(e - (2*numEdges) + j);
				if(j != 2 * numEdges - 1) {
					out << spaces(2);
					if((i < 3 && j % 2 == 0) || (numEdges == 3 && (i + j) % 2 == 0) || (i > 7 && j % 2 != 0)) {
						out << board.printResource(t);
						++t;
						++resetT;
					} else out << spaces(6);
					out << spaces(1);
				}
			}
			// resest tile number
			t -= resetT;
			resetT = 0;
			out << "\n";
	
		}
	}
	return out;
}

