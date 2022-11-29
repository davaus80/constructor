#include <vector>
#include <algorithm>
#include "illegalmoveexn.h"
#include "board.h"
#include "vertex.h"


Vertex::Vertex(AbstractBoard *board, int num, int bt, int player) :
	mainBoard {board},
	number{num},
	buildingType{bt},
	player{player} {}

void Vertex::connectEdge(int index) {
	if (!(std::count(adjEdges.begin(), adjEdges.end(), index))) {
		adjEdges.emplace_back(index);
	}
}

void Vertex::connectVertex(int index) {
	if (!(std::count(adjVertices.begin(), adjVertices.end(), index))) {
		adjVertices.emplace_back(index);
	}
}

int Vertex::getPlayer() const {
	return player;
}

int Vertex::getBuildingType() const {
	return buildingType;
}

void Vertex::upgrade(int playerNum) {
	if (buildingType == 3) {
		throw IllegalMoveExn{"Cannot upgrade this building further"};
	}
	else {
		buildingType++;
		player = playerNum;
	}
}

bool Vertex::hasAdjRoad(int playerNum, int excludingEdge) const {
	int checkEdge;
	for (int i = 0; i < adjEdges.size(); i++) {
		checkEdge = adjEdges[i];
		if (checkEdge != excludingEdge && mainBoard->hasOwnedRoad(checkEdge, playerNum)) {
			return true;
		}
	}
	return false;
}

bool Vertex::hasAdjRes(int playerNum) const {
	for(int i = 0; i < adjVertices.size(); ++i) {
		if(mainBoard->hasAdjRes(adjVertices[i])) return true;
	}
	return false;
}


