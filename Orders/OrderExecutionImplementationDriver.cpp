#include <iostream>
#include <string>
#include "Orders.hpp"
#include "../Map/Map.h"

using namespace std;

int main() {
	cout << "--Constructors--" << endl;
	string* na = new string("North America");
	string* ca = new string("Canada");
	string* us = new string("United States");
	string* mx = new string("Mexico");
	string* ga = new string("Guatemala");
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");
	Continent* northAmerica = new Continent(*na, 0);
	Territory* canada = new Territory(*ca, northAmerica);
	Territory* unitedStates = new Territory(*us, northAmerica);
	Territory* mexico = new Territory(*mx, northAmerica);
	Territory* guatemala = new Territory(*ga, northAmerica);

	canada->adjacentTerritories.push_back(unitedStates);
	unitedStates->adjacentTerritories.push_back(canada);
	unitedStates->adjacentTerritories.push_back(mexico);
	mexico->adjacentTerritories.push_back(unitedStates);
	guatemala->adjacentTerritories.push_back(mexico);
	mexico->adjacentTerritories.push_back(guatemala);
	canada->owner = p2;
	unitedStates->owner = p1;
	mexico->owner = p2;
	guatemala->owner = p2;

	p1->territories.insert(unitedStates);

	p2->territories.insert(canada);
	p2->territories.insert(mexico);
	p2->territories.insert(guatemala);
	canada->nbOfArmy = 10;
	unitedStates->nbOfArmy = 15;
	mexico->nbOfArmy = 3;
	guatemala->nbOfArmy = 1;


	OrderList* o = new OrderList();


	Deploy* dep = new Deploy(p1, 1, unitedStates);
	Advance* adv = new Advance(p1, 7, mexico, unitedStates);
	Bomb* bomb = new Bomb(p1, unitedStates);
	Airlift* air = new Airlift(p2,8,mexico , canada);
	Blockade* block = new Blockade(p2, canada);
	Negotiate* nego = new Negotiate(p2, p1);
	
	cout << endl << "Attempting to execute a Deploy order..." << endl;
	dep->execute();

	cout << endl << "Attempting to execute an Advance order..." << endl;
	cout << "Current Owner of Mexico: " << mexico->owner->name << endl;
	adv->execute();
	cout << "Owner of Mexico After Advance Order: " << mexico->owner->name << endl;

	cout << endl << "Attempting to execute an Airlift order..." << endl;
	air->execute();

	cout << endl << "Attempting to execute a Bomb order..." << endl;
	bomb->execute();

	cout << endl << "Attempting to execute a Blockade order..." << endl;
	cout << "Current Owner of Canada: " << canada->owner->name << endl;
	block->execute();
	cout << "Owner of Canada After Blockade Order: " << canada->owner->name << endl;


	cout << endl << "Attempting to execute a Negotiate order..." << endl;
	nego->execute();

	cout << endl << "Attempting to execute an Advance order on negotiated players..." << endl;
	Advance* advFail = new Advance(p2, 1, mexico, guatemala);
	advFail->execute();

	cin.get();
}