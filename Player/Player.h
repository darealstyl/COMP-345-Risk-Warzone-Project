#pragma once
#include <unordered_set>
#include <vector>
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.hpp"

using namespace std;

class Territory;
class OrderList;
class Hand;
class Order;

class Player {
public:
	string name;
	unordered_set<Territory*> territories;
	Hand* hand;
	OrderList* orderList;
	int* reinforcements;
	bool endOfOrder;

	vector<Territory*> toAttack();
	vector<Territory*> toDefend();

	enum orderTypes {
		DEPLOY = 0, ADVANCE, BOMB, BLOCKADE, AIRLIFT, NEGOTIATE
	};
	Player(string);
	Player(const Player&);
	~Player();

	Player& operator=(const Player&);

	friend std::ostream& operator<<(std::ostream&, const Player&);

	void issueOrder(orderTypes, Territory*);
	void issueOrder(orderTypes, int, Territory*);
	void issueOrder(orderTypes, int, Territory*, Territory*);
	void issueOrder(orderTypes, Player*);
	void issueOrder();

	int getNbOfTerritories();

	void addTerritory(Territory*);
	void removeTerritory(Territory*);

	void addReinforcements(int);
	void removeReinforcments(int);

	orderTypes* command;

	static Player* neutralplayer;
	static void resetNeutralPlayer();
};