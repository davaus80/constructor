#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "die.h"

// #####################################################################################################################################
// Player represents a user playing the game. The class stores their resources and building/road locations.
// #####################################################################################################################################
class Player {
	std::string colour;
	int points;
	int bricks;
	int energy;
	int glass;
	int heat;
	int wifi;
	// Vectors of integers representing indices player's buildings are at
	std::vector<int> roads;
	std::vector<int> basements;
	std::vector<int> houses;
	std::vector<int> towers;
	std::unique_ptr<AbstractDie> die;
	std::string dieType = "loaded";
       public:
	Player (std::string colour, int points, int bricks, int energy,int glass, int heat, int wifi);
	bool sufficientResources(int buildingType) const; // returns true if player has sufficient resources to build the given building/road
	void addResources(int resource, int num); // gives player specified number of selected resource
	void removeResources(int resource, int num); // removes specified number of selected resource
	// build adds vertexNum to buildingType vector, subtracts cost of building (if isBeginning=false)
	void build(int buildingType, int vertexNum, bool isBeginning = false);
	void addRoad(int edgeNum); // Charges player for road and adds its vertex to roadVector
	void addResFree(int bt, int vertex); // Gives player residence of type bt at vertex for no cost
	void addRoadFree(int edge); // Gives player road at edge free of charge
	// goosePenaltyPlayer removes half of a player's resources if they have more than ten and returns an vector indicating which resources were lost
	std::vector<int> goosePenaltyPlayer(); 
	int loseRandomResource(); //Player loses a random resource, returns an integer indicating resource type
	int getNumOfResources() const; // Returns the total number of resources a player has.
	int roll() const; // Gets the value for a roll of the player's die.
	std::vector<int> getResources() const; // Returns a vector of the player's amount of each type of resource
	std::vector<int> getRoads() const; // Returns a vector of the player's road locations
	std::vector<std::vector<int>> getResidences() const; // Returns a vector of vectors which are pairs of ints indicating vertex and type for each residence
	void setDie(std::string type); //sets die to the given type
	int getNumResource(int resourceNum) const; // returns the number the player has of a given resource
	int getPoints() const; // gets the number of builder points the player has
	friend std::ostream &operator<<(std::ostream &out, const Player &player);
};

// output operator for Player
std::ostream& operator<<(std::ostream &out, const Player &player);

#endif
