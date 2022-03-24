#include "Player.h"
#include <stdlib.h>
#include <map>
#include <algorithm>

Player::Player(string n)
{
	name = n;
	hand = new Hand();
	orderList = new OrderList();
	reinforcements = new int(0);
	command = new orderTypes(DEPLOY);
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
	cout << "Destroying a Player" << endl;
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

void Player::issueOrder(orderTypes o, Territory* location)
{
	switch (o)
	{
	case BOMB: {
		Bomb* bomb = new Bomb(this, location);
		orderList->list.push_back(bomb);
	}
		break;
	case BLOCKADE: {
		Blockade* blockade = new Blockade(this, location);
		orderList->list.push_back(blockade);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	}
}

void Player::issueOrder(orderTypes o, int numOfArmies, Territory* location)
{
	switch (o)
	{
	case DEPLOY: {
		Deploy* deploy = new Deploy(this, numOfArmies, location);
		orderList->list.push_back(deploy);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	}
}

void Player::issueOrder(orderTypes o, int numOfArmies, Territory* to, Territory* from)
{

	switch (o)
	{
	case ADVANCE: {
		Advance* advance = new Advance(this, numOfArmies, to, from);
		orderList->list.push_back(advance);
	}
		break;
	case AIRLIFT: {
		Airlift* airlift = new Airlift(this, numOfArmies, to, from);
		orderList->list.push_back(airlift);
	}
		break;
	default: cout << "Invalid Order issued";
		break;
	};
}

void Player::issueOrder(orderTypes o, Player* targetPlayer)
{
	switch (o)
	{
	case NEGOTIATE: {
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
	switch (*command)
	{
	case DEPLOY:
		if (*reinforcements != 0) {
			//toAttack();
			//toDefend(); //return the toAttack() and toDefend() vector which will arrange priority

			// Create a deploy order and push it back in the orderlist
			cout << "Creating deploy order" << endl;
			Deploy* deploy = new Deploy(this, *reinforcements, toDefend().at(0));
			orderList->list.push_back(deploy);

			//Put this in execution phase

			srand(time(NULL)); //initialize random seed
			int iarmy;
			iarmy = rand() % *reinforcements + 1; //number of reinforcements between 1 - nb of reinforcements

			// Deploy Soldiers to toDefend(), the first index as it is priority one
			toDefend().at(0)->nbOfArmy += iarmy;
			cout << "Deployement of " << iarmy << " soldiers" << endl;
			removeReinforcments(iarmy);
			cout << "Total of reinforcement: " << *reinforcements << endl;
		}
		else {
			cout << "No more reinforcements available" << endl;
			cout << "Moving to the ADVANCE order" << endl;
			*command = ADVANCE;
		}
		break;
	case ADVANCE:
		//toAttack();
		//toDefend();
		{
			cout << "Making an advance order" << endl;
			Advance* advance = new Advance(this, *reinforcements, toDefend().at(0), toDefend().at(1));
			orderList->list.push_back(advance);

			cout << "Moving to the special orders";
			if (hand->cards.size() == 0) {
				cout << "No more cards in hand";
				endOfOrder = true;
				break;
			}
			srand(time(NULL));
			int randCard = rand() % hand->cards.size() - 1 + 1; //choose a random card to issue order
			*command = (orderTypes)hand->cards.at(randCard)->getCardType(); //the command will corresponds to the type of card
		}
		break;
	case AIRLIFT:
		toAttack();
		toDefend();
		{
			Airlift* airlift = new Airlift(this, toDefend().at(0)->nbOfArmy, toDefend().at(0), toAttack().at(0));
			orderList->list.push_back(airlift);
		}
		break; 
	case BOMB:
	{
		Bomb* bomb = new Bomb(this, toAttack().at(0));
		orderList->list.push_back(bomb);
	}
		break;
	case BLOCKADE:
	{
		Blockade* blockade = new Blockade(this, toAttack().at(0));
		orderList->list.push_back(blockade);
	}
		break;
	case NEGOTIATE:
	{
		Negotiate* negotiate = new Negotiate(this, toAttack().at(0)->owner);
		orderList->list.push_back(negotiate);
	}
	}
	

	
}

void Player::addReinforcements(int armies) {
	*reinforcements += armies;
}

void Player::removeReinforcments(int armies) {
	*reinforcements -= armies;
}
