#pragma once
#include <unordered_set>
#include <vector>
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include "../Orders/Orders.hpp"
#include "../PlayerStrategies/PlayerStrategies.h"

using namespace std;


class Territory;
class OrderList;
class Order;
class PlayerStrategy;

class Player {
public:
	string name;
	unordered_set<Territory*> territories;
	Hand* hand;
	OrderList* orderList;
	Card* chosenCard;
	PlayerStrategy * strat;
	int reinforcements;
	bool endOfOrder;
	bool conquered;
	int advanceordersnb;
	

	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
	vector<Territory*> getAdjacentTerritories();
	vector<Territory*> getAtRiskTerritories();

	enum class OrderType {
		DEPLOY = 0, ADVANCE, BOMB, BLOCKADE, AIRLIFT, NEGOTIATE
	};
	Player(string);
	Player(string, PlayerStrategy*);
	Player(const Player&);
	~Player();

	Player& operator=(const Player&);

	friend std::ostream& operator<<(std::ostream&, const Player&);
		
	void issueOrder(Deck*);

	int getNbOfTerritories();
	OrderType cardTypeToOrderType(Card::CardType);

	void addTerritory(Territory*);
	void addTerritory(Territory*, int);
	void removeTerritory(Territory*);

	void addReinforcements(int);
	void removeReinforcments(int);

	void addFriendlyPlayer(Player*);
	bool isFriendlyPlayer(Player*);
	void clearFriendlyPlayers();
	void setPlayerStrategy(string);

	OrderType command;

	static Player* neutralplayer;
	static void resetNeutralPlayer();
	void chooseNextCommand();
private:
	unordered_set<Player*> friendlyPlayers;
	
};
