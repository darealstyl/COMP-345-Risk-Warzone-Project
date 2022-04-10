#include "Player.h"
#include "../PlayerStrategies/PlayerStrategies.h"
#include <stdlib.h>
#include <map>
#include <algorithm>

typedef Player::OrderType OT;
typedef Card::CardType CT;

Player* Player::neutralplayer = new Player("Neutral Player");

void Player::resetNeutralPlayer() {
	delete neutralplayer;
	neutralplayer = new Player("Neutral Player");
}

Player::Player(string n) {
	name = n;
	hand = new Hand();
	orderList = new OrderList();
	reinforcements = 0;
	command = OT::DEPLOY;
	conquered = false;
	endOfOrder = false;
	chosenCard = nullptr;
	advanceordersnb = 0;
	// players are Neutral by default
	strat = new NeutralPlayerStrategy();
	strat->setPlayerLink(this);
}

Player::Player(string n, PlayerStrategy* ps) : Player(n) {
	// link strategy to the player and save strategy
	ps->setPlayerLink(this);
	strat = ps;
}

Player::Player(const Player& player) {
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = player.reinforcements;
}

Player& Player::operator=(const Player& player) {
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = player.reinforcements;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Player& player) {
	out << player.name;
	return out;
}

Player::~Player() {
	delete orderList;
	orderList = NULL;

	delete hand;
	hand = NULL;

	delete strat;

}

vector<Territory*> Player::getAdjacentTerritories() {
	vector<Territory*> toattack;
	for (Territory* territory : territories) {
		for (Territory* adjacentterritory : territory->adjacentTerritories) {
			if (adjacentterritory->owner != this) {
				toattack.push_back(adjacentterritory);
			}
		}
	}
	return toattack;
}

// gets territories owned by the player and sorts by army strength
vector<Territory*> Player::getAtRiskTerritories() {
	// TODO: possibly refactor to only get territories with adjacent enemies?

	vector<Territory*> defend(territories.begin(), territories.end());
	// sort territories to defend by strength (army count)
	sort(defend.begin(), defend.end(), [](const Territory* lhs, const Territory* rhs) {
		return lhs->nbOfArmy > rhs->nbOfArmy;
	});
	return defend;
}

void Player::addTerritory(Territory* territory) {
	
	territory->owner->territories.erase(territory);
	
	territories.insert(territory);
	territory->owner = this;
}

void Player::addTerritory(Territory* territory, int nbOfTroops) {
	addTerritory(territory);
	territory->nbOfArmy = nbOfTroops;
}

void Player::removeTerritory(Territory* territory) {
	
	territories.erase(territory);

	territory->owner = Player::neutralplayer;
	
}

int Player::getNbOfTerritories() {
	return territories.size();
}


vector<Territory*> Player::toAttack() {
	return strat->toAttack();
}

vector<Territory*> Player::toDefend() {
	return strat->toDefend();
}

void Player::issueOrder() {
	strat->issueOrder();
}

void Player::issueOrder(OrderType o, Territory* location)
{
	switch (o)
	{
	case OT::BOMB: {
		Bomb* bomb = new Bomb(this, location);
		orderList->list.push_back(bomb);
	}
		break;
	case OT::BLOCKADE: {
		Blockade* blockade = new Blockade(this, location);
		orderList->list.push_back(blockade);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	}
}

void Player::issueOrder(OrderType o, int numOfArmies, Territory* location)
{
	switch (o)
	{
	case OT::DEPLOY: {
		Deploy* deploy = new Deploy(this, numOfArmies, location);
		orderList->list.push_back(deploy);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	}
}

void Player::issueOrder(OrderType o, int numOfArmies, Territory* to, Territory* from)
{

	switch (o)
	{
	case OT::ADVANCE: {
		Advance* advance = new Advance(this, numOfArmies, to, from);
		orderList->list.push_back(advance);
	}
		break;
	case OT::AIRLIFT: {
		Airlift* airlift = new Airlift(this, numOfArmies, to, from);
		orderList->list.push_back(airlift);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	};
}

void Player::issueOrder(OrderType o, Player* targetPlayer)
{
	switch (o)
	{
	case OT::NEGOTIATE: {
		Negotiate* negotiate = new Negotiate(this, targetPlayer);
		orderList->list.push_back(negotiate);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	}
}
// TODO: Missing reinforcement card. Why is there such a card?
void Player::issueOrder(CT cardtype) {
	vector<Territory*> defend = toDefend();
	vector<Territory*> attack = toAttack();

	Order* order = nullptr;

	switch (cardtype) {
	case CT::AIRLIFT: {
		Territory* from = defend.back();
		Territory* to = defend.front();

		int transfer = from->nbOfArmy;
		if (transfer <= 0) {
			transfer = 1;
		}

		int numOfArmies = (rand() % transfer) + 1;

		order = new Airlift(this, numOfArmies, to, from);
		break;
	}
	case CT::BLOCKADE: {
		Territory* location = defend.front();

		order = new Blockade(this, location);
		break;
	}
	case CT::BOMB: {
		Territory* location = attack.front();

		order = new Bomb(this, location);

		break;
	}
	case CT::DIPLOMACY: {
		Territory* territory = attack.front();
		Player* targetPlayer = territory->owner;

		order = new Negotiate(this, targetPlayer);
		break;
	}
	case CT::REINFORCEMENT: {

		// TODO: ADJUST THIS AMOUNT
		reinforcements += 10;
		break;
	}
	}

	if (order != nullptr) {
		orderList->add(order);
	}
}
void Player::addFriendlyPlayer(Player* friendplayer) {
	friendlyPlayers.insert(friendplayer);
}

bool Player::isFriendlyPlayer(Player* friendplayer) {
	auto pair = friendlyPlayers.find(friendplayer);
	return pair != friendlyPlayers.end();
}

void Player::clearFriendlyPlayers() {
	friendlyPlayers.clear();
}

void Player::issueOrder(Deck* deck) {
	chooseNextCommand();
	if (endOfOrder) {
		return;
	}

	vector<Territory*> defend = toDefend();
	vector<Territory*> attack = toAttack();

	Territory* weakestTerritory = defend.front();
	Territory* strongestTerritory = defend.back();
	Territory* vulnerableEnnemy = attack.front();

	endOfOrder = false;
	switch (command)
	{
	case OT::DEPLOY: {
		// Create a deploy order and push it back in the orderlist
		if (chosenCard != nullptr) {
			chosenCard->play(this,deck);
			chosenCard = nullptr;
		}
		if (reinforcements != 0) {
			cout << "Creating deploy order" << endl;
			int nbOfreinforcements = (rand() % reinforcements) + 1;
			reinforcements -= nbOfreinforcements;

			cout << "Deploying " << nbOfreinforcements << " reinforcements to " << *weakestTerritory << endl;
			cout << reinforcements << " reinforcements left." << endl;
			Deploy* deploy = new Deploy(this, nbOfreinforcements, weakestTerritory);
			orderList->add(deploy);
			
		}
		break;
	}
	case OT::ADVANCE:
		{
			
		int advanceChoice = (rand() % 2);
		Territory* to;
		if (advanceChoice == 0) {
			to = weakestTerritory;
		}
		else {
			to = vulnerableEnnemy;
		}
		Territory* from = nullptr;
		for (Territory* neighbor : to->adjacentTerritories) {
			if (neighbor->owner == this) {
				from = neighbor;
				break;
			}
		}
		if (from == nullptr) {
			from = strongestTerritory;
		}
		int number = from->nbOfArmy;
		if (number == 0) {
			number = 1;
		}
		int soldierAmount = (rand() % number/4) + 1;
		Advance* advance = new Advance(this, soldierAmount, from, to);
		orderList->add(advance);
		break;
		}
		
	case OT::AIRLIFT:
	case OT::BOMB:
	case OT::BLOCKADE:
	case OT::NEGOTIATE:
		cout << *hand << endl;
		chosenCard->play(this, deck);
		cout << *hand << endl;
		chosenCard = nullptr;
		break;
	}
	
	
	
}

void Player::chooseNextCommand() {
	if (reinforcements != 0) {
		command = OT::DEPLOY;
	}
	else if (advanceordersnb != 5) {
		command = OT::ADVANCE;
		advanceordersnb++;
	}
	else if (hand->cards.size() != 0) {
		// Could be replaced by a conversion function but oh well
		cout << "Choosing a card to play" << endl;
		Card* card = hand->cards.back();
		chosenCard = card;
		command = cardTypeToOrderType(card->getCardType());
		cout << "The card to play next turn is a " << card->cardTypeToString() << endl;
	}
	else {
		cout << *this << " is done issuing orders." << endl;
		endOfOrder = true;
	}
}

OT Player::cardTypeToOrderType(CT cardtype) {
	switch (cardtype) {
	case CT::AIRLIFT:			return OT::AIRLIFT;
	case CT::BLOCKADE:			return OT::BLOCKADE;
	case CT::BOMB:				return OT::BOMB;
	case CT::DIPLOMACY:			return OT::NEGOTIATE;
	case CT::REINFORCEMENT:		return OT::DEPLOY;
	default: throw - 1;
	}
	
}

void Player::addReinforcements(int armies) {
	cout << *this << " receives " << armies << " armies." << endl;
	reinforcements += armies;
}

void Player::removeReinforcments(int armies) {
	cout << *this << " used " << armies << " armies." << endl;
	reinforcements -= armies;
}
