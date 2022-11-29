#include <vector>
#include <algorithm>
#include "vertex.h"
#include "board.h"
#include "edge.h"
#include "tile.h"

Tile::Tile(AbstractBoard *board, int num, int res, int val, bool goose) :
	mainBoard{board},
	number{num},
	resource{res},
	value{val},
	goose{goose} {}

Tile::~Tile() {}

void Tile::connectVertex(int index) {
	adjVertices.emplace_back(index);
}

void Tile::connectEdge(int index) {
	adjEdges.emplace_back(index);
}

void Tile::connectAllVertices(int v0, int v1, int v2, int v3, int v4, int v5) {
	connectVertex(v0);
	connectVertex(v1);
	connectVertex(v2);
	connectVertex(v3);
	connectVertex(v4);
	connectVertex(v5);
}

void Tile::connectAllEdges(int e0, int e1, int e2, int e3, int e4, int e5) {
	connectEdge(e0);
	connectEdge(e1);
	connectEdge(e2);
	connectEdge(e3);
	connectEdge(e4);
	connectEdge(e5);
}

void Tile::connectComponents() {
	std::sort(adjVertices.begin(), adjVertices.end());
	std::sort(adjEdges.begin(), adjEdges.end());
	mainBoard->connectVerticesWithEdge(adjVertices[0],adjVertices[1], adjEdges[0]);
	mainBoard->connectVerticesWithEdge(adjVertices[0],adjVertices[2], adjEdges[1]);
	mainBoard->connectVerticesWithEdge(adjVertices[1],adjVertices[3], adjEdges[2]);
	mainBoard->connectVerticesWithEdge(adjVertices[2],adjVertices[4], adjEdges[3]);
	mainBoard->connectVerticesWithEdge(adjVertices[3],adjVertices[5], adjEdges[4]);
	mainBoard->connectVerticesWithEdge(adjVertices[4],adjVertices[5], adjEdges[5]);
}

void Tile::rolled() {
	if (goose) {
		return;
	}
	for (int i = 0; i < adjVertices.size(); i++) {
		mainBoard->residenceRolled(adjVertices[i], resource);
	}
}

std::vector<int> Tile::getBorderingPlayers() const {
	std::vector<int> retVal;
	for (int i = 0; i < adjVertices.size(); i++) {
		int vertexOwner = mainBoard->getVertexOwner(adjVertices[i]);
		if (vertexOwner != -1) {
			if (!(std::count(retVal.begin(), retVal.end(), vertexOwner))) {
				retVal.push_back(vertexOwner);
			}
		}
	}
	return retVal;
}

int Tile::getValue() const {
	return value;
}

int Tile::getResource() const {
	return resource;
}

int Tile::setGoose(bool value) {
	goose = value;
	int adjResNum = 0;
	for (unsigned int i = 0; i < adjVertices.size(); i++) {
		if (mainBoard->getBuildingType(adjVertices[i])) {
			adjResNum++;
		}
	}
	return adjResNum;
}

bool Tile::getGoose() const {
	return goose;
}






