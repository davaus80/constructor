#ifndef _TILE_H_
#define _TILE_H_

#include <vector>

class AbstractBoard; // forward declaration

// #####################################################################################################################################
// The Tile class represents one hexagonal tile on the game board. It keeps track of the vertices and edges bordering it as well as
// if it has a goose on it, its resource type, and its roll number.
// #####################################################################################################################################
class Tile {
	AbstractBoard *mainBoard;
	int number;
	int resource;
	int value;
	bool goose;
	std::vector<int> adjVertices; // vertices bordering the tile
	std::vector<int> adjEdges; // edges bordering the tile
       public:
	Tile(AbstractBoard *board, int num, int res, int val, bool goose);
	~Tile();
	void connectVertex(int index); // adds index to adjVertices
	void connectEdge(int index); // adds index to adjEdges
	// connects all vertices with one function call
	void connectAllVertices(int v0, int v1, int v2, int v3, int v4, int v5);
	// connects all edges with one function call
	void connectAllEdges(int e0, int e1, int e2, int e3, int e4, int e5);
	// Connects vertices to the correct adjacent vertices via the correct edge
	// For example, the top two vertices are connected using the top edge.
	void connectComponents(); 
	// Returns players that border the given tile
	std::vector<int> getBorderingPlayers() const;
	// Indicates the tile has been rolled. Tells the adjacent vertices they have been rolled
	void rolled();
	int getValue() const;
	int getResource() const;
	int setGoose(bool value);
	bool getGoose() const;
};

#endif
