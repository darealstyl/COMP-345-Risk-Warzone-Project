#include <iostream>
#include <string>
#include "Orders.hpp"
#include "../Map/Map.h"
#include "../Game Engine/GameEngine.h"

using namespace std;

int main() {

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

	GameEngine* engine = new GameEngine();
	engine->addPlayer(p1);
	engine->addPlayer(p2);

	Deploy* dep = new Deploy(p1, 1, unitedStates);
	Advance* adv = new Advance(p1, 7, mexico, unitedStates);
	Bomb* bomb = new Bomb(p1, unitedStates);
	Airlift* air = new Airlift(p2,8,mexico , canada);
	Blockade* block = new Blockade(p2, canada);
	Negotiate* nego = new Negotiate(p2, p1);
	Advance* advFail = new Advance(p2, 1, mexico, guatemala);

	p1->orderList->add(dep);
	p1->orderList->add(adv);
	p1->orderList->add(bomb);
	p2->orderList->add(air);
	p2->orderList->add(block);
	p2->orderList->add(nego);
	p2->orderList->add(advFail);

	engine->executeOrdersPhase();

	cout << "Player " << p1->name << " Card Count: " << p1->hand->cards.size() << endl;
	cout << "Player " << p2->name << " Card Count: " << p2->hand->cards.size() << endl;

	delete na;
	delete ca;
	delete us;
	delete mx;
	delete ga;
	delete p1;
	delete p2;
	delete northAmerica;
	delete canada;
	delete unitedStates;
	delete mexico;
	delete guatemala;
	delete engine;

	cin.get();
}