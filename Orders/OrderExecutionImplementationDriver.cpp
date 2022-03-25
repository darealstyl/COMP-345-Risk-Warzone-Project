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
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");
	Continent* northAmerica = new Continent(*na);
	Territory* canada = new Territory(*ca, northAmerica);
	Territory* unitedStates = new Territory(*us, northAmerica);

	canada->adjacentTerritories.push_back(unitedStates);
	unitedStates->adjacentTerritories.push_back(canada);

	p1->territories.insert(unitedStates);
	p2->territories.insert(canada);
	canada->nbOfArmy = 10;


	OrderList* o = new OrderList();

	Bomb* bomb = new Bomb(p1, canada);
	bomb->execute();
}