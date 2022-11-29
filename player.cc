#include <string>
#include <vector> 
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include "player.h"
#include "die.h"
#include <memory>

Player::Player(std::string colour, int points, int bricks, int energy, int glass, int heat, int wifi) :
	colour{colour},
	points{points},
	bricks{bricks},
	energy{energy},
	glass{glass},
	heat{heat},
	wifi{wifi},
	die{std::make_unique<LoadedDie>()} {}

bool Player::sufficientResources(int buildingType) const {
	if(buildingType == 0) {
		if(bricks >= 1 && energy >= 1 && glass >= 1 && wifi >= 1) return true;
	} else if(buildingType == 1) {
		if(glass >= 2 && heat >= 3) return true;
	} else if(buildingType == 2) { 
		if(bricks >= 3 && energy >= 2 && glass >= 2 && wifi >= 1 && heat >= 2) return true;
	} else if(buildingType == 3) { // road
		if(heat >= 1 && wifi >= 1) return true;
	}
	return false;
}

void Player::addResources(int resource, int num) {
	if (resource == 0) {
		bricks+=num;
	}
	else if (resource == 1) {
		energy+=num;
	}
	else if (resource == 2) {
		glass+=num;
	}
	else if (resource == 3) {
		heat+=num;
	}
	else if (resource == 4) {
		wifi+=num;
	}
}

void Player::addRoadFree(int edge) {
	roads.emplace_back(edge);
}

void Player::addResFree(int bt, int vertex) {
	if (bt == 0) {
		basements.emplace_back(vertex);
		points += 1;
	} else if (bt == 1) {
		houses.emplace_back(vertex);
		points += 2;
	} else {
		towers.emplace_back(vertex);
		points += 3;
	}
}


void Player::removeResources(int resource, int num) {
	if (resource == 0) {
		bricks-=num;
	}
	else if (resource == 1) {
		energy-=num;
	}
	else if (resource == 2) {
		glass-=num;
	}
	else if (resource == 3) {
		heat-=num;
	}
	else if (resource == 4) {
		wifi-=num;
	}
}

void Player::build(int buildingType, int vertexNum, bool isBeginning) {
	if(isBeginning || (buildingType == 0)) {
		basements.emplace_back(vertexNum);
		if (!isBeginning) {
			this->removeResources(0, 1);
			this->removeResources(1, 1);
			this->removeResources(2, 1);
			this->removeResources(4, 1);
		}
	} else if(buildingType == 1) {
		for(int i = 0; i < basements.size(); ++i) {
			if(basements[i] == vertexNum) basements.erase(basements.begin() + i);
		}
		houses.emplace_back(vertexNum);
		this->removeResources(2, 2);
		this->removeResources(3, 3);
	} else {
		for(int i = 0; i < houses.size(); ++i) {
			if(houses[i] == vertexNum) houses.erase(houses.begin() + i);
		}
		towers.emplace_back(vertexNum);
		this->removeResources(0, 3);
		this->removeResources(1, 2);
		this->removeResources(2, 2);
		this->removeResources(3, 2);
		this->removeResources(4, 1);
	} 
	++points;
}

void Player::addRoad(int edgeNum) {
	roads.emplace_back(edgeNum);
	this->removeResources(3, 1);
	this->removeResources(4, 1);
}

int Player::loseRandomResource() {
	std::vector<int> totalResources;
	for (int i = 0; i < bricks; i++) { totalResources.push_back(0); }
	for (int i = 0; i < energy; i++) { totalResources.push_back(1); }
	for (int i = 0; i < glass; i++) { totalResources.push_back(2); }
	for (int i = 0; i < heat; i++) { totalResources.push_back(3); }
	for (int i = 0; i < wifi; i++) { totalResources.push_back(4); }
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::random_shuffle(totalResources.begin(), totalResources.end()/*, std::default_random_engine(seed)*/);
	if (totalResources.size() == 0) {
		return -1;
	}
	int removedVal = -1;
	if (totalResources[0] == 0) { 
		bricks-=1; 
		removedVal = 0;
	} else if (totalResources[0] == 1) { 
		energy-=1;
	        removedVal = 1;
	} else if (totalResources[0] == 2) { 
		glass-=1;
		removedVal = 2;
       	} else if (totalResources[0] == 3) { 
		heat-=1; 
		removedVal = 3;
	} else if (totalResources[0] == 4) { 
		wifi-=1; 
		removedVal = 4;
	}
	return removedVal;
}

std::vector<int> Player::goosePenaltyPlayer() {
	std::vector<int> totalResources;
	for (int i = 0; i < bricks; i++) { totalResources.push_back(0); }
	for (int i = 0; i < energy; i++) { totalResources.push_back(1); }
	for (int i = 0; i < glass; i++) { totalResources.push_back(2); }
	for (int i = 0; i < heat; i++) { totalResources.push_back(3); }
	for (int i = 0; i < wifi; i++) { totalResources.push_back(4); }
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::random_shuffle(totalResources.begin(), totalResources.end()/*, std::default_random_engine(seed)*/);
	int numToRemove = totalResources.size() / 2;
	std::vector<int> removedVals;
	for (int i = 0; i < 5; i++) {
		removedVals.push_back(0);
	}
	if (totalResources.size() < 10) {
		return removedVals;
	}
	for (int i = 0; i < numToRemove; i++) {
		if (totalResources[i] == 0) { 
			bricks-=1; 
			removedVals[0] += 1;
		} else if (totalResources[i] == 1) { 
			energy-=1;
		        removedVals[1] += 1;
		} else if (totalResources[i] == 2) { 
			glass-=1;
			removedVals[2] += 1;
	       	} else if (totalResources[i] == 3) { 
			heat-=1; 
			removedVals[3] += 1;
		} else if (totalResources[i] == 4) { 
			wifi-=1; 
			removedVals[4] += 1;
		}
	}
	return removedVals;
}

int Player::getNumOfResources() const {
	return (bricks + energy + glass + heat + wifi);
}

int Player::roll() const {
	return die->roll();
}

std::vector<int> Player::getResources() const {
	std::vector<int> retVec;
	retVec.push_back(bricks);
	retVec.push_back(energy);
	retVec.push_back(glass);
	retVec.push_back(heat);
	retVec.push_back(wifi);
	return retVec;
}

std::vector<int> Player::getRoads() const {
	return roads;
}

std::vector<std::vector<int>> Player::getResidences() const {
	std::vector<std::vector<int>> residences;
	for (int i = 0; i < basements.size(); i++) {
		std::vector<int> resPair;
		resPair.push_back(basements[i]);
		resPair.push_back(0);
		residences.push_back(resPair);
	}
	for (int i = 0; i < houses.size(); i++) {
		std::vector<int> resPair;
		resPair.push_back(houses[i]);
		resPair.push_back(1);
		residences.push_back(resPair);
	}
	for (int i = 0; i < towers.size(); i++) {
		std::vector<int> resPair;
		resPair.push_back(towers[i]);
		resPair.push_back(2);
		residences.push_back(resPair);
	}
	return residences;
}

void Player::setDie(std::string type) {
	if(dieType != type) {
		if(type == "fair") {
			die = std::move(std::make_unique<FairDie>());
		} else die = std::move(std::make_unique<LoadedDie>());
	}
}

int Player::getNumResource(int resourceNum) const {
	if(resourceNum == 0 && bricks >= 1) return true;
	if(resourceNum == 1 && energy >= 1) return true;
	if(resourceNum == 2 && glass >= 1) return true;
	if(resourceNum == 3 && heat >= 1) return true;
	if(resourceNum == 4 && wifi >= 1) return true;
	return false;
}

int Player::getPoints() const {
	return points;
}

std::ostream &operator<<(std::ostream &out, const Player &player) {
	return out;
}

