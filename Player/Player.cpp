#include "Player.h"
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

Player::Player(string n)
{
	name = n;
	hand = new Hand();
	orderList = new OrderList();
	reinforcements = 0;
	command = OT::DEPLOY;
	conquered = false;
	endOfOrder = false;
	chosenCard = nullptr;
}

Player::Player(const Player& player)
{
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = player.reinforcements;
}

Player& Player::operator=(const Player& player){
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = player.reinforcements;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Player& player) {
	return out << player.name;
}

Player::~Player()
{
	delete orderList;
	orderList = NULL;

	delete hand;
	hand = NULL;


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
	vector<Territory*> territoriesToAttack;
	map<Territory*, int> territoriestotroops;

	// Loops on every territory that the current player owns, and checks the adjacent territories to each of them,
	// if the two aren't owned by the same player that means it is not owned by the current player and a territory you could attack.
	for (Territory* territory : territories) {
		int currentNbOfArmy = territory->nbOfArmy;
		for (Territory* adjacentTerritory : territory->adjacentTerritories) {
			if (territory->owner != adjacentTerritory->owner && territoriestotroops[adjacentTerritory] < currentNbOfArmy) {
				territoriestotroops[adjacentTerritory] = currentNbOfArmy;
			}
		}
	}

	for (auto it = territoriestotroops.rbegin(); it != territoriestotroops.rend(); it++)
		territoriesToAttack.push_back(it->first);

	return territoriesToAttack;
}

vector<Territory*> Player::toDefend() {
	vector<Territory*> toDefend(territories.begin(), territories.end());
	sort(toDefend.begin(), toDefend.end());
	return toDefend;
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
			
		int advanceChoice;
		advanceChoice = (rand() % 3) + 1; // three choices (attack, defend or both)
		cout << "Choice: " << advanceChoice << endl;

		if (advanceChoice == 1) {
			cout << "Making an advance order to defend a territory" << endl;
			Advance* advanceD = new Advance(this, reinforcements, strongestTerritory, weakestTerritory);
			orderList->add(advanceD);
		}

		else if (advanceChoice == 2) {
			cout << "Making an advance order to attack a territory" << endl;
			Advance* advanceA = new Advance(this, reinforcements, strongestTerritory, vulnerableEnnemy);
			orderList->add(advanceA);
		}
		else if (advanceChoice == 3) {
			cout << "Making both an advance order to defend and to attack a territory";
			Advance* advanceD = new Advance(this, reinforcements, strongestTerritory, weakestTerritory);
			orderList->add(advanceD);
			Advance* advanceA = new Advance(this, reinforcements, strongestTerritory, vulnerableEnnemy);
			orderList->add(advanceA);
		}
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
