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
	string* dn = new string("Denmark");
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");
	Continent* northAmerica = new Continent(*na, 0);
	Territory* canada = new Territory(*ca, northAmerica);
	Territory* unitedStates = new Territory(*us, northAmerica);
	Territory* mexico = new Territory(*mx, northAmerica);
	Territory* denmark = new Territory(*mx, northAmerica);

	canada->adjacentTerritories.push_back(unitedStates);
	unitedStates->adjacentTerritories.push_back(canada);
	unitedStates->adjacentTerritories.push_back(mexico);
	mexico->adjacentTerritories.push_back(unitedStates);
	canada->owner = p2;
	unitedStates->owner = p1;
	mexico->owner = p2;

	p1->territories.insert(unitedStates);

	p2->territories.insert(canada);
	p2->territories.insert(mexico);
	canada->nbOfArmy = 10;
	unitedStates->nbOfArmy = 15;
	mexico->nbOfArmy = 3;


	OrderList* o = new OrderList();


	Deploy* dep = new Deploy(p1, 1, unitedStates);
	Advance* adv = new Advance(p1, 7, mexico, unitedStates);
	Bomb* bomb = new Bomb(p1, unitedStates);
	Airlift* air = new Airlift(p2,8,mexico , canada);
	Blockade* block = new Blockade(p2, canada);
	Negotiate* nego = new Negotiate(p2, p1);
	
	dep->execute();
	adv->execute();	
	air->execute();
	bomb->execute();
	block->execute();
	nego->execute();
	cin.get();
}