#ifndef _BOARD_H_
#define _BOARD_H_

#include <vector>
#include <memory>
#include <string>
#include "vertex.h"
#include "tile.h"
#include "edge.h"
#include "player.h"

// #####################################################################################################################################
// AbstractBoard serves as the abstract base class for all board subclasses. It
// represents a game board and contains all methods and data fields present in
// all types of boards.
// #####################################################################################################################################
class AbstractBoard {
       protected:
	std::vector<std::unique_ptr<Tile>> tiles;
	std::vector<std::unique_ptr<Vertex>> vertices;
	std::vector<std::unique_ptr<Edge>> edges;
	std::vector<std::unique_ptr<Player>> players;
	int goose;
       public:
	AbstractBoard();
	// Returns a string of the type of board.
	virtual std::string getBoardType() = 0;
	//places a res owned by player at vertex vertexNum. If isBeginning is true, residences do not need a
	// road owned by the player next to them to be place. Returns true if build brings player to 10 points.
	bool placeRes(int vertexNum, int player, bool isBeginning = false);
	// builds road for inputed player on inputed edge if spot is valid and player has enough resources
	void buildRoad(int edgeNum, int playerNum);
	// returns true if the player passed owns a road on the passed edge
	bool hasOwnedRoad(int edgeNum, int playerNum) const;
	// returns true id inputed player owns a res on inputed vertex
	bool hasOwnedRes(int vertexNum, int playerNum) const;
	// returns true if there is an adjacent residence to the passes vertex
	bool hasAdjRes(int vertexNum) const;
	// returns the roll if the dice is fair, otherwise returns 13
	int getRoll(int playerNum) const;
	// calls a function on players die to chanage the instance of the die to the new type
	void setDie(int playerNum, std::string type);
	//upgrades residence at vertexNum to the next level (if this is a legal move). Not legal if player does
	//not have a path to that vertex, vertex is occupied, there is a residence at an adjacent vertex, or
	//the player has insufficient resources. If move is legal, residence is places and resources subtracted.
	void upgrade(int vertexNum, int player);
	// player1 swaps one resource1 for one of player2's resource2. Throws if either player
	// does not have the resource.
	void trade(int player1, int player2, int resource1, int resource2);
	// Acts as a dice roll. All tiles with the rolled number give one of their resources to the owner of
	// each residence that borders them.
	void rolled(int num);
	// Tells a vertex that it has been rolled. This allows it to indicate which player to give how many of the resource
	void residenceRolled(int vertex, int resource);
	int moveGoose(int newTile); //Returns an integer of the number of players on that tile. Moves goose to newTile
	// a random resource from the stealFrom player is given to the thief
	int stealResource(int stealFrom, int thief);
	// Removes half of each player's resources if they have over ten. Each inner vector returned indicates resources lost for each player.
	std::vector<std::vector<int>> goosePenalty();
	// gives player a residence of type bt at vertex, free of charge, if the space is available. Used for loading games
	void giveResidenceFree(int vertex, int bt, int player);
	// gives player a road at edge, if the space is available. Used for loading games
	void giveRoadFree(int edge, int player);
	// gives player the specified number of the specified resource for free.
	void giveResources(int player, int resource, int number);
	// Tells these vertices and edges that they are adjacent to each other
	void connectVerticesWithEdge(int v1, int v2, int edge);
	// #############################################################################################################################
	// Accessors and mutators
	// #############################################################################################################################
	std::vector<std::vector<int>> getBoard() const; // returns a vector of vectors, where the inner vectors are integer pairs of tile resource and roll
	std::vector<int> getNeighbours(int tile) const;
	int getGoose() const;
	int getBuildingType(int vertex) const;
	int getVertexOwner(int vertex) const;
	int getNumOfResources(int player) const;
	std::vector<int> getResources(int player) const; // returns a vector of the player's current resources
	std::vector<int> getRoads(int player) const;
	std::vector<std::vector<int>> getResidences(int player) const; // returns a vector of vectors, where the inner vectors are integer pairs of vertex and building type
	bool roadAdjToVertex(int vertex, int player) const; // returns true is there is one of the player's roads adjacent to the vertex
	bool roadAdjToVertexExcluding(int vertexNum, int playerNum, int excluding) const; 
	// #############################################################################################################################
	// Methods used for printing
	// #############################################################################################################################
	// returns a string for edge for board print, marks road with <colour initial>R if owned 
	// marks with number otherwise
	std::string printEdge(int edgeNum, bool horz) const;
	// returns a string of vertex for board print, marks with <colour inital><type initial> if owned, number otherwise
	std::string printVertex(int vertexNum) const;
	// returns a string of resource on a tile for board print
	std::string printResource(int tileNum) const;
	// returns a string of value of a tile for board print
	// returns a string of empty spot or GEESE for board print
	std::string checkGeese(int tileNum) const;
	// returns a string of the value of the tile for board print
	std::string printValue(int tileNum) const;
	// resturns a colored road if the road is owned for a vertical segment for board print, otherwise white
	std::string vertEdge(int edgeNum) const;
	friend std::ostream &operator<< (std::ostream &out, const AbstractBoard &board);
};

// output operator for AbstractBoard.
std::ostream &operator<< (std::ostream &out, const AbstractBoard &board);

// StandardBoard is a concrete subclass of AbstractBoard that represents a board with nineteen hexagonal tiles
class StandardBoard: public AbstractBoard {
       public:
	std::string getBoardType() override;
	StandardBoard(std::string boardLayout); // Constructs a StandardBoard with a layout specified by resources and  rolls in boardLayout
	friend std::ostream &operator<< (std::ostream &out, const StandardBoard &board);
};

// output operator for StandardBoard. Outputs the board as specified in the assignment file.
std::ostream &operator<< (std::ostream &out, const StandardBoard &board);

#endif
