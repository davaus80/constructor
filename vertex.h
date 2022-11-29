#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <vector>

class AbstractBoard; // Forward declaration

// #####################################################################################################################################
// The Vertex class represents a vertex, at the intersection of Tile corners. It stores the type of building on it and the
// player that owns the building. It also stores the edges bordering it and the vertices one edge away.
// #####################################################################################################################################
class Vertex {
	AbstractBoard *mainBoard;
	int number;
	int buildingType;
	int player;
	std::vector<int> adjEdges; // adjacent edges
	std::vector<int> adjVertices; // vertices one edge away
       public:
	Vertex(AbstractBoard *board, int num, int bt, int player);
	void connectEdge(int index); // adds index to adjEdges
	void connectVertex(int index); // adds index to adjVertices
	void upgrade(int playerNum); // increments buildingType if less than three
	bool hasAdjRoad(int playerNum, int excludingEdge = -1) const; // returns true if residence has an adjacent road belonging to playerNum
	bool hasAdjRes(int playerNum) const; // returns true if playerNum has a residence adjacent
	int getPlayer() const; 
	int getBuildingType() const;
};

#endif
