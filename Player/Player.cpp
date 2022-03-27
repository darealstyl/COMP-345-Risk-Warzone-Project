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
	reinforcements = new int(0);
	command = OT::DEPLOY;
}

Player::Player(const Player& player)
{
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = new int(*(player.reinforcements));
}

Player& Player::operator=(const Player& player){
	territories = player.territories;

	hand = new Hand(*(player.hand));
	orderList = new OrderList(*(player.orderList));
	reinforcements = new int(*(player.reinforcements));
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

	delete reinforcements;

}

void Player::addTerritory(Territory* territory) {
	territory->owner->territories.erase(territory);
	territories.insert(territory);
	territory->owner = this;
}

void Player::removeTerritory(Territory* territory) {
	
	territories.erase(territory);

	if (territory->owner == this) {
		Player::neutralplayer->addTerritory(territory);
	}
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

void Player::issueOrder() {
	//*command = DEPLOY;
	endOfOrder = false;
	toAttack();
	toDefend();
	switch (command)
	{
	case OT::DEPLOY:
		if (*reinforcements != 0) {
			//toAttack();
			//toDefend(); //return the toAttack() and toDefend() vector which will arrange priority

			// Create a deploy order and push it back in the orderlist
			cout << "Creating deploy order" << endl;
			Deploy* deploy = new Deploy(this, *reinforcements, toDefend().at(0));
			orderList->list.push_back(deploy);

			//Put this in execution phase
			
			int iarmy;
			iarmy = rand() %*reinforcements + 1; //number of reinforcements between 1 - nb of reinforcements

			// Deploy Soldiers to toDefend(), the first index as it is priority one
			toDefend().at(0)->nbOfArmy += iarmy;
			cout << "Deployement of " << iarmy << " soldiers" << endl;
			removeReinforcments(iarmy);
			cout << "Total of reinforcement: " << *reinforcements << endl;
		}
		else {
			cout << "No more reinforcements available" << endl;
			cout << "Moving to the ADVANCE order" << endl;
			command = OT::ADVANCE;
		}
		break;
	case OT::ADVANCE:
		//toAttack();
		//toDefend();
		{
			
		int advanceChoice;
		advanceChoice = (rand() % 3) + 1; // three choices (attack, defend or both)
		cout << "Choice: " << advanceChoice << endl;

		if (advanceChoice == 1) {
			cout << "Making an advance order to defend a territory" << endl;
			Advance* advanceD = new Advance(this, *reinforcements, toDefend().at(0), toDefend().at(1));
			orderList->list.push_back(advanceD);
		}

		else if (advanceChoice == 2) {
			cout << "Making an advance order to attack a territory" << endl;
			Advance* advanceA = new Advance(this, *reinforcements, toDefend().at(0), toAttack().at(0));
			orderList->list.push_back(advanceA);
		}
		else if (advanceChoice == 3) {
			cout << "Making both an advance order to defend and to attack a territory";
			Advance* advanceBD = new Advance(this, *reinforcements, toDefend().at(0), toDefend().at(1));
			orderList->list.push_back(advanceBD);
			Advance* advanceBA = new Advance(this, *reinforcements, toDefend().at(0), toAttack().at(0));
			orderList->list.push_back(advanceBA);
		}
		
		}
		break;
	case OT::AIRLIFT:
		
		{
		cout << "The player is playing the Airlift card" << endl;
			Airlift* airlift = new Airlift(this, toDefend().at(0)->nbOfArmy, toDefend().at(0), toAttack().at(0));
			orderList->list.push_back(airlift);
		}
		break; 
	case OT::BOMB:
	{
		cout << "The player is playing the Bomb card" << endl;
		Bomb* bomb = new Bomb(this, toAttack().at(0));
		orderList->list.push_back(bomb);
	}
		break;
	case OT::BLOCKADE:
	{
		cout << "The player is playing the BLockade card" << endl;
		Blockade* blockade = new Blockade(this, toAttack().at(0));
		orderList->list.push_back(blockade);
	}
		break;
	case OT::NEGOTIATE:
	{
		cout << "The player is playing the Diplomacy card" << endl;
		Negotiate* negotiate = new Negotiate(this, toAttack().at(0)->owner);
		orderList->list.push_back(negotiate);
	}

	}
	
	chooseNextCommand();
	
}

void Player::chooseNextCommand() {
	if (*reinforcements != 0) {
		command = OT::DEPLOY;
	}
	else if (hand->cards.size() != 0) {
		// Could be replaced by a conversion function but oh well
		cout << "Choosing a card to play" << endl;
		Card* card = hand->cards.back();
		hand->cards.pop_back();
		command = cardTypeToOrderType(card->getCardType());
		cout << "The card to play next turn is a " << card->cardTypeToString() << endl;
	}
	else {
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
	*reinforcements += armies;
}

void Player::removeReinforcments(int armies) {
	cout << *this << " used " << armies << " armies." << endl;
	*reinforcements -= armies;
}
