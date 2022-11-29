#ifndef _EDGE_H_
#define _EDGE_H_

#include <vector>

class AbstractBoard;

// #####################################################################################################################################
// The Edge class represents an edge on the board, at the intersection of two tiles. The class stores if it has a road on it
// and who owns the road, as well as which vertices are at its ends.
// #####################################################################################################################################

class Edge {
	int thisIndex; // index of this road in the board's road vector
	AbstractBoard *homeBoard;
	bool hasRoad;
	int player;
	std::vector<int> adjVertices; // vertives at the end of the edge
       public:
	Edge(AbstractBoard *board, int index, bool hasRoad, int player);
	void connectVertex(int index); // adds index to adjVertices
	bool getHasRoad() const; 
	int getPlayer() const;
	void buildRoad(int playerNum); //sets player to playerNum and hasRoad to true if road isn't already built
	bool hasAdjBuild(int playerNum)const; // returns true if playerNum has a building adjacent to this road
};

#endif
