#pragma once
#include <unordered_set>
#include <vector>
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.hpp"

using namespace std;


class Territory;
class OrderList;
class Order;

class Player {
public:
	string name;
	unordered_set<Territory*> territories;
	Hand* hand;
	OrderList* orderList;
	Card* chosenCard;
	int reinforcements;
	bool endOfOrder;

	vector<Territory*> toAttack();
	vector<Territory*> toDefend();

	enum class OrderType {
		DEPLOY = 0, ADVANCE, BOMB, BLOCKADE, AIRLIFT, NEGOTIATE
	};
	Player(string);
	Player(const Player&);
	~Player();

	Player& operator=(const Player&);

	friend std::ostream& operator<<(std::ostream&, const Player&);

	void issueOrder(OrderType, Territory*);
	void issueOrder(OrderType, int, Territory*);
	void issueOrder(OrderType, int, Territory*, Territory*);
	void issueOrder(OrderType, Player*);
	void issueOrder(Deck*);
	void issueOrder(Card::CardType);

	int getNbOfTerritories();
	OrderType cardTypeToOrderType(Card::CardType);

	void addTerritory(Territory*);
	void removeTerritory(Territory*);

	void addReinforcements(int);
	void removeReinforcments(int);

	OrderType command;

	static Player* neutralplayer;
	static void resetNeutralPlayer();

private:
	void chooseNextCommand();
};