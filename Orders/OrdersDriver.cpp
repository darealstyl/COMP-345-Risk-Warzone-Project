#include <iostream>
#include <string>
#include "Orders.hpp"
#include "../Map/Map.h"

using namespace std;

int main()
{
	cout << "--Constructors--" << endl;
	string* na = new string("North America");
	string* ca = new string("Canada");
	string* us = new string("United States");
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");
	Continent* northAmerica = new Continent(*na, 0);
	Territory* canada = new Territory(*ca, northAmerica);
	Territory* unitedStates = new Territory(*us, northAmerica);


	OrderList* o = new OrderList();

	Deploy* deploy = new Deploy(p1, 1, canada);
	Advance* advance = new Advance(p1, 1, canada, unitedStates);
	Bomb* bomb = new Bomb(p1, canada);
	Blockade* blockade = new Blockade(p1, canada);
	Airlift* airlift = new Airlift(p1, 1, canada, unitedStates);
	Negotiate* negotiate = new Negotiate(p1, p2);
	cout << "--Copy constructor--" << endl;
	Deploy* deploy2 = new Deploy(*deploy);

	o->list.push_back(deploy);
	deploy = NULL;
	o->list.push_back(advance);
	advance = NULL;
	o->list.push_back(bomb);
	bomb = NULL;
	o->list.push_back(blockade);
	blockade = NULL;
	o->list.push_back(airlift);
	airlift = NULL;
	o->list.push_back(negotiate);
	negotiate = NULL;
	o->list.push_back(deploy2);
	deploy2 = NULL;
	cout << "--execute() and validate()--" << endl;
	//deploy->execute();
	// I'm assuming the above line shouldve been the below, since deploy is set to null. Was causing issues with logging.
	o->list.front()->execute();

	cout << o->list.size() << endl;
	cout << *o->list.at(0) << endl;
	cout << *o->list.at(1) << endl;
	cout << "--move()--" << endl;
	o->move(0, 1);

	cout << *o->list.at(0) << endl;
	cout << *o->list.at(1) << endl;
	cout << "--remove()--" << endl;
	o->remove(0);

	cout << o->list.size() << endl;
	cout << *o->list.at(0) << endl;
	cout << *o->list.at(1) << endl;

	int size = o->list.size();
	cout << "--Emptying List--" << endl;
	for (int i = 0; i < size; i++)
	{
		o->remove(0);
	}
	
	cout << o->list.size() << endl;

	


	delete (p1);
	delete (p2);
	delete (us);
	delete (na);
	delete (ca);
	delete (northAmerica);
	delete (canada);
	delete (unitedStates);
	delete (o);

	p1 = NULL;
	p2 = NULL;
	us = NULL;
	na = NULL;
	ca = NULL;
	northAmerica = NULL;
	cin.get();
	return 0;
} 