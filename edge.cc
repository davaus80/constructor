#include <vector>
#include <algorithm>
#include "illegalmoveexn.h"
#include "board.h"
#include "edge.h"

Edge::Edge(AbstractBoard *board, int thisIndex, bool hasRoad, int player) :
	thisIndex{thisIndex},
	hasRoad{hasRoad},
	player{player},
	homeBoard{board} {}

void Edge::connectVertex(int index) {
	if (std::count(adjVertices.begin(), adjVertices.end(), index)) {
		return;
	}
	else {
		adjVertices.emplace_back(index);
	}
}

int Edge::getPlayer() const{
	return player;
}

bool Edge::getHasRoad() const {
	return hasRoad;
}

// TODO: fix this, I just did it preliminarily
void Edge::buildRoad(int playerNum) {
	if (hasRoad) {
		throw IllegalMoveExn{"Road has already been built at this location."};
	}
	else {
		hasRoad = true;
		player = playerNum;
	}
}


bool Edge::hasAdjBuild(int playerNum) const {
	for (int i = 0; i < adjVertices.size(); i++) {
		if (homeBoard->hasOwnedRes(adjVertices[i], playerNum)) return true;
		else if (homeBoard->roadAdjToVertexExcluding(adjVertices[i], playerNum, thisIndex)) return true;
	}
	return false;
}

	


