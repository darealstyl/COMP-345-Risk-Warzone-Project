#include <iostream>
#include "GameEngine.h"
using namespace std;

int main()
{
	// To demonstrate in this driver:
	// 1) A player receives the correct number of armies in the reinforcmenet phase (showing multiple cases) ***DONE
	// 2) A player will only issue deploy orders and no other kind of orders if they still have armies in their reinforcement pool ***DONE
	// 3) A player can issue advance orders to either defend or attack, based on the toAttack and toDefend lists ***DONE (will give random number for three
	// options, attack, defend, or both (one after the other)
	// 4) A plaer can play cards to issue orders ***IN PROGRESS
	// 5) A player that does not control any territory is removed from the game. ***DONE
	// 6) The game ends when a single player controls all the territories ***DONE
	string* s1 = new string("america");
	Player* p1 = new Player("ced");
	Player* p2 = new Player("jorge");
	Continent* c1 = new Continent(*s1);
	Territory* t1 = new Territory("georgia", c1);
	Territory* t2 = new Territory("oklaoma", c1);
	Territory* t3 = new Territory("butan", c1);
	Territory* t4 = new Territory("canada", c1);
	//Territory* t5 = new Territory("canada", c1);
	//Territory* t6 = new Territory("canada", c1);
	//Territory* t7 = new Territory("canada", c1);
	//Territory* t8 = new Territory("canada", c1);
	//Territory* t9 = new Territory("canada", c1);
//	Territory* t10 = new Territory("canada", c1);
	//Territory* t11 = new Territory("canada", c1);
	//Territory* t12 = new Territory("canada", c1);
//	Territory* t13 = new Territory("canada", c1);
	t1->adjacentTerritories.push_back(t2);
	t1->adjacentTerritories.push_back(t3);
	t2->adjacentTerritories.push_back(t1);
	t2->adjacentTerritories.push_back(t3);
	t3->adjacentTerritories.push_back(t1);
	t3->adjacentTerritories.push_back(t2);
	//t1->adjacentTerritories.push_back(t4);
	c1->territories.push_back(t1);
	c1->territories.push_back(t2);
	c1->territories.push_back(t3);
	//c1->territories.push_back(t4);

	p1->territories.insert(t1);
	p1->territories.insert(t2);
	p2->territories.insert(t3);
	p2->territories.insert(t4);
	t1->owner = p1;
	t2->owner = p1;
	t3->owner = p2;
	t4->owner = p2;
	/*p1->territories.insert(t3);
	p1->territories.insert(t4);
	p1->territories.insert(t5);
	p1->territories.insert(t6);
	p1->territories.insert(t7);
	p1->territories.insert(t8);
	p1->territories.insert(t9);
	p1->territories.insert(t10);
	p1->territories.insert(t11);
	p1->territories.insert(t12);*/
	//p2->territories.insert(t13);
	

	GameEngine* g1 = new GameEngine();
	Map* m1 = new Map();
	m1->continents.push_back(c1);
	m1->territories.push_back(t1);
	m1->territories.push_back(t2);
	m1->territories.push_back(t3);
	//m1->territories.push_back(t4);
	//m1->territories.push_back(t5);

	//m1->territories.push_back(t6);
	//m1->territories.push_back(t7);
	//m1->territories.push_back(t8);
	//m1->territories.push_back(t9);
	//m1->territories.push_back(t10);
	//m1->territories.push_back(t11);
	//m1->territories.push_back(t12);
	//m1->territories.push_back(t12);
	//m1->territories.push_back(t13);
	g1->theMap = m1;

	g1->addPlayer(p1);
	g1->addPlayer(p2);

	
	int i = 0;
	g1->reinforcementPhase();
	srand(time(0));
	while (i <= 3) {
		g1->issueOrdersPhase();
		i++;
				   }
	p2->territories.clear();
	g1->executeOrdersPhase();
	g1->executeOrdersPhase();
	g1->executeOrdersPhase();
	g1->executeOrdersPhase();

		/*g1->issueOrdersPhase();
		g1->issueOrdersPhase();
		g1->issueOrdersPhase();
		g1->issueOrdersPhase();
		g1->issueOrdersPhase();
		g1->issueOrdersPhase();
		g1->issueOrdersPhase();*/
		
	delete g1;
	g1 = nullptr;
	
}
