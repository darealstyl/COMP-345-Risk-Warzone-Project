#include <iostream>
#include "GameEngine.h"
using namespace std;

int main()
{
	string* s1 = new string("america");
	Player* p1 = new Player("ced");
	Player* p2 = new Player("jorge");
	Continent* c1 = new Continent(*s1, 0);
	Territory* t1 = new Territory("georgia", c1);
	Territory* t2 = new Territory("oklaoma", c1);
	Territory* t3 = new Territory("butan", c1);
	Territory* t4 = new Territory("canada", c1);
	t1->adjacentTerritories.push_back(t2);
	t1->adjacentTerritories.push_back(t3);
	t2->adjacentTerritories.push_back(t1);
	t2->adjacentTerritories.push_back(t3);
	t3->adjacentTerritories.push_back(t1);
	t3->adjacentTerritories.push_back(t2);
	c1->territories.push_back(t1);
	c1->territories.push_back(t2);
	c1->territories.push_back(t3);

	p1->territories.insert(t1);
	p1->territories.insert(t2);
	p2->territories.insert(t3);
	p2->territories.insert(t4);
	t1->owner = p1;
	t2->owner = p1;
	t3->owner = p2;
	t4->owner = p2;
	

	GameEngine* g1 = new GameEngine();
	Map* m1 = new Map();
	m1->continents.push_back(c1);
	m1->territories.push_back(t1);
	m1->territories.push_back(t2);
	m1->territories.push_back(t3);

	g1->map = m1;

	Card* card = new Card(Card::CardType::DIPLOMACY);

	p1->hand->addCard(card);

	g1->addPlayer(p1);
	g1->addPlayer(p2);

	
	int i = 0;
	g1->reinforcementPhase();
	srand(time(0));
	while (i <= 4) {
		g1->issueOrdersPhase();
		i++;
				   }
	p2->territories.clear();
	g1->executeOrdersPhase();
	g1->executeOrdersPhase();
			
	delete g1;
	g1 = nullptr;
	cin.get();
}
