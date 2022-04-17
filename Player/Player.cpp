#include "Player.h"
#include "../PlayerStrategies/PlayerStrategies.h"
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <set>

typedef Player::OrderType OT;
typedef Card::CardType CT;

Player* Player::neutralplayer = new Player("Neutral Player", new NeutralPlayerStrategy());

void Player::resetNeutralPlayer() {
	delete neutralplayer;
	neutralplayer = new Player("Neutral Player", new NeutralPlayerStrategy());
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
	// players are Human by default
	strat = new HumanPlayerStrategy();
	strat->setPlayerLink(this);
}

Player::Player(string n, PlayerStrategy* ps) : Player(n) {
	// link strategy to the player and save strategy
	ps->setPlayerLink(this);
	// other constructor sets strat to Human, need to overwrite it for this constructor
	delete strat;
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
	set<Territory*> temp(toattack.begin(), toattack.end());
	toattack.assign(temp.begin(), temp.end());
	return toattack;
}

// gets territories owned by the player and sorts by army strength
vector<Territory*> Player::getAtRiskTerritories() {

	vector<Territory*> defend(territories.begin(), territories.end());
	// sort territories to defend by strength decreasing (army count)
	sort(defend.begin(), defend.end(), [](const Territory* lhs, const Territory* rhs) {
		return lhs->nbOfArmy > rhs->nbOfArmy;
	});
	return defend;
}

void Player::setPlayerStrategy(string stringstrategy) {

	PlayerStrategy* playerstrategy = nullptr;

	if (stringstrategy == "aggressive") {
		playerstrategy = new AggressivePlayerStrategy();
	}
	else if (stringstrategy == "benevolent") {
		playerstrategy = new BenevolentPlayerStrategy();
	}
	else if (stringstrategy == "neutral") {
		playerstrategy = new NeutralPlayerStrategy();
	}
	else if (stringstrategy == "cheater") {
		playerstrategy = new CheaterPlayerStrategy();
	}
	else if (stringstrategy == "human") {
		playerstrategy = new HumanPlayerStrategy();
	}

	if (this->strat) {
		delete strat;
	}

	this->strat = playerstrategy;
	playerstrategy->setPlayerLink(this);
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

void Player::issueOrder(Deck* deck) {
	strat->issueOrder(deck);
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
