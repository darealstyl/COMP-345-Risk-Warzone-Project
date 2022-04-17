#include "PlayerStrategies.h"
#include "../Orders/Orders.hpp"
#include <iostream>

using std::cout;
using std::endl;
typedef Player::OrderType OT;
typedef Card::CardType CT;
#pragma region Base
const unordered_set<string> PlayerStrategy::strategystrings = {
	{"aggressive"}, 
	{"benevolent"},
	{"neutral"}, 
	{"cheater"}};
// Default Constructor. Unusable due to being Pure Virtual Class
PlayerStrategy::PlayerStrategy() {
	p = nullptr;
}
// Copy Construtor. Unusable due to being Pure Virtual Class
PlayerStrategy::PlayerStrategy(const PlayerStrategy& s) {
	p = s.p;
}
// Destructor
PlayerStrategy::~PlayerStrategy() {}
// Assignment Operator
PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& s) {
	return *this;
}
// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const PlayerStrategy& s) {
	// debugging only. typeid not recommended for normal usage
	out << typeid(s).name();
	return out;
}
// Connects the strategy to the player using it.
// Used within ConcreteStrategies methods
void PlayerStrategy::setPlayerLink(Player* player) {
	p = player;
}
#pragma endregion

#pragma region Human
// Default Constructor
HumanPlayerStrategy::HumanPlayerStrategy() {};
// Copy Constructor
HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& s) {}
// Destructor
HumanPlayerStrategy::~HumanPlayerStrategy() {};
// Assignment Operator
HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& s) {
	return *this;
}
// Stream Insertion Operator. Doesn't work polymorphically.
std::ostream& operator<<(std::ostream& out, const HumanPlayerStrategy& s) {
	out << "[HUMANPLAYERSTRATEGY]";
	return out;
}

void HumanPlayerStrategy::issueOrder(Deck* deck)
{
	cout << p->name << " - Human issueOrder" << endl;
	// Requires user interaction to make decisions
	p->chooseNextCommand();
	if (p->endOfOrder) {
		return;
	}

	vector<Territory*> defend = toDefend();
	// players are only kicked after order execution phase, but cheater claims territories in issueorder phase
	// ensure the current player is still in the game
	if (defend.empty()) {
		p->endOfOrder = true;
		return;
	}
	vector<Territory*> attack = toAttack();
	Territory* strongestTerritory = p->getAtRiskTerritories().front();

	Territory* vulnerableEnemy = attack.front();

	p->endOfOrder = false;
	switch (p->command)
	{
	case OT::DEPLOY: {
		// Create a deploy order and push it back in the orderlist
		if (p->chosenCard != nullptr) {
			p->chosenCard->play(p, deck);
			p->chosenCard = nullptr;
		}
		if (p->reinforcements != 0) {
			cout << "Creating deploy order" << endl;
			int nbOfreinforcements = 0;
			int nbCountry = 0;
			//user interaction for the number of reinforcements
			cout << "How many reinforcements do you want to deploy? (max: " << p->reinforcements << ") " << endl;
			cin >> nbOfreinforcements;

			//user interaction for the country to defend
			cout << "Which territory do you want to deploy to?" << endl;
			cout << "List of your territories: " << endl;
			for (int i = 0; i < defend.size(); i++) {
				cout << i << ". " << defend[i]->name << " - number of armies: " << defend[i]->nbOfArmy<< endl;
			}
			cout << "Enter the number of the country: ";
			cin >> nbCountry;
			Territory* def = defend[nbCountry];
			cout << "Deploying " << nbOfreinforcements << " reinforcements to " << def->name << endl;
			Deploy* deploy = new Deploy(p, nbOfreinforcements, def);
			p->orderList->add(deploy);
			// all reinforcements were used, set to 0
			p->reinforcements -= nbOfreinforcements;
			cout << p->reinforcements << " reinforcements left." << endl;

		}
		break;
	}
	case OT::ADVANCE:
	{
		// human interaction: number of armies, from territory, to territory

		// verify user wants to do any advances
		char continueAdvancing;
		do {
			cout << endl << "Would you like to issue an advance? (y/n): ";
			cin >> continueAdvancing;
		} while (continueAdvancing != 'y' && continueAdvancing != 'n');

		if (continueAdvancing == 'n') {
			// stop asking the user to issue advances
			// (why are we limited to 5 advances?)
			p->advanceordersnb = 5;
			break;
		}

		int advanceChoice{};
		Territory* from = nullptr;
		Territory* to = nullptr;
		int fromChoice = 0;
		int toChoice = 0;

		//user interaction: defend or attack
		cout << "Do you want to do an advance order on your 1. own territory, 2.enemy territory" << endl;
		cin >> advanceChoice;


		//user interaction to send soldier from defend to defend of choice

		if (advanceChoice == 1) {
			cout << "List of your territories: " << endl;
			for (int i = 0; i < defend.size(); i++) {
				cout << i << ". " << defend[i]->name << " - number of armies: " << defend[i]->nbOfArmy << endl;
			}
			cout << "Enter the number of the territory that will send armies: ";
			cin >> fromChoice;
			cout << endl << "Enter the number of the territory that receives the armies: ";
			cin >> toChoice;

			from = defend[fromChoice];
			to = defend[toChoice];
		}
		//user interaction to send soldiers from defend to attack
		else {
			cout << "List of your territories: " << endl;
			for (int i = 0; i < defend.size(); i++) {
				cout << i << ". " << defend[i]->name << " - number of armies: " << defend[i]->nbOfArmy << endl;
			}
			cout << "Enter the number of the territory that attacks the enemy: ";
			cin >> fromChoice;
			cout << endl << "List of enemies' territories: " << endl;
			for (int i = 0; i < attack.size(); i++) {
				cout << i << ". " << attack[i]->name << " - number of armies: " << attack[i]->nbOfArmy << endl;
			}
			cout << "Enter the number of the enemy territory that's being attacked: ";
			cin >> toChoice;



			from = defend[fromChoice];
			to = attack[toChoice];
		}
	
		int max = from->nbOfArmy;
		int number = 0;
		//user interaction for the number of armies to send 
		cout << "How many soldiers to send (max: " << max << " ): ";
		cin >> number;

		Advance* advance = new Advance(p, number, to, from);
		p->orderList->add(advance);		

		break;
	}

	case OT::AIRLIFT:
	case OT::BOMB:
	case OT::BLOCKADE:
	case OT::NEGOTIATE:
		cout << *(p->hand) << endl;
		p->chosenCard->play(p, deck);
		cout << *(p->hand) << endl;
		p->chosenCard = nullptr;
		break;
	}
}

vector<Territory*> HumanPlayerStrategy::toAttack()
{
	cout << p->name << " - Human toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> HumanPlayerStrategy::toDefend()
{
	cout << p->name << " - Human toDefend" << endl;
	return p->getAtRiskTerritories();
}
#pragma endregion

#pragma region Aggressive
// Default Constructor
AggressivePlayerStrategy::AggressivePlayerStrategy() {}
// Copy Constructor
AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& s) {}
// Destructor
AggressivePlayerStrategy::~AggressivePlayerStrategy() {}
// Assignment Operator
AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& s){
	return *this;
}
// Stream Insertion Operator. Doesn't work polymorphically.
std::ostream& operator<<(std::ostream& out, const AggressivePlayerStrategy& s){
	out << "[AGGRESSIVEPLAYERSTRATEGY]";
	return out;
}

void AggressivePlayerStrategy::issueOrder(Deck* deck)
{
	cout << p->name << " - Aggressive issueOrder" << endl;
	// (Computer player)
	// Focuses on attack
	// Deploys/Advances armies on its strongest country then always advances to enemy territories until it can't
	p->chooseNextCommand();
	if (p->endOfOrder) {
		return;
	}

	vector<Territory*> defend = toDefend();
	// players are only kicked after order execution phase, but cheater claims territories in issueorder phase
	// ensure the current player is still in the game
	if (defend.empty()) {
		p->endOfOrder = true;
		return;
	}
	vector<Territory*> attack = toAttack();
	Territory* strongestTerritory = p->getAtRiskTerritories().front();
	Territory* vulnerableEnemy = attack.front();

	p->endOfOrder = false;
	switch (p->command)
	{
	case OT::DEPLOY: {
		// Create a deploy order and push it back in the orderlist
		if (p->chosenCard != nullptr) {
			p->chosenCard->play(p, deck);
			p->chosenCard = nullptr;
		}
		if (p->reinforcements != 0) {
			cout << "Creating deploy order" << endl;
			int nbOfreinforcements = p->reinforcements;


			cout << "Deploying " << nbOfreinforcements << " reinforcements to " << *strongestTerritory << endl;
			Deploy* deploy = new Deploy(p, p->reinforcements, strongestTerritory);
			p->orderList->add(deploy);
			// all reinforcements were used, set to 0
			p->reinforcements = 0;
			cout << p->reinforcements << " reinforcements left." << endl;

		}
		break;
	}
	case OT::ADVANCE:
	{
		// aggressive players advance player into its strongest territories until all soldiers are there 

		//TODO: check if all armies are in its strongest territory
		int advanceChoice{};
		Territory* from = nullptr;
		//check if any territory except strongest has > 0 nbOfArmies
		for (Territory* territory : p->territories) {
			if ((territory->nbOfArmy != 0) && (territory != strongestTerritory)) {
				from = territory; //sending from territory that still has soldiers
				advanceChoice = 0; //go to the defense option
				break;
			}
		}



		
		Territory* to = nullptr;
		//if 0, we send soldier to strongest territory
		if (advanceChoice == 0) {
			to = strongestTerritory;
		}
		//attack from strongest to ennemy
		else {
			from = strongestTerritory;
			to = vulnerableEnemy;
		}
		/*for (Territory* neighbor : to->adjacentTerritories) {
			if (neighbor->owner == p) {
				from = neighbor;
				break;
			}
		}*/
		if (from == nullptr) {
			from = strongestTerritory;
		}
		int number = from->nbOfArmy;
		if (number == 0) {
			number = 1;
		}
		int soldierAmount = (rand() % number / 4) + 1;
		Advance* advance = new Advance(p, soldierAmount, from, to);
		p->orderList->add(advance);
		break;
	}

	case OT::AIRLIFT:
	case OT::BOMB:
	case OT::BLOCKADE:
	case OT::NEGOTIATE:
		cout <<  *(p->hand) << endl;
		p->chosenCard->play(p, deck);
		cout << *(p->hand) << endl;
		p->chosenCard = nullptr;
		break;
	}
	
	
}

vector<Territory*> AggressivePlayerStrategy::toAttack()
{
	cout << p->name << " - Aggressive toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> AggressivePlayerStrategy::toDefend()
{
	cout << p->name << " - Aggressive toDefend" << endl;
	return p->getAtRiskTerritories();
}
#pragma endregion

#pragma region Benevolent
// Default Constructor
BenevolentPlayerStrategy::BenevolentPlayerStrategy() {}
// Copy Constructor
BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& s) {}
// Destructor
BenevolentPlayerStrategy::~BenevolentPlayerStrategy() {}
// Assignment Operator
BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& s) {
	return *this;
}
// Stream Insertion Operator. Doesn't work polymorphically.
std::ostream& operator<<(std::ostream& out, const BenevolentPlayerStrategy& s) {
	out << "[BENEVOLENTPLAYERSTRATEGY]";
	return out;
}

void BenevolentPlayerStrategy::issueOrder(Deck* deck)
{
	cout << p->name << " - Benevolent issueOrder" << endl;
	// (Computer player)
	// Focuses on protecting its weak countries
	// Deploys/Advances on its weakest countries, never advances to enemy territories
	
	p->chooseNextCommand();
	if (p->endOfOrder) {
		return;
	}

	vector<Territory*> defend = toDefend();
	// players are only kicked after order execution phase, but cheater claims territories in issueorder phase
	// ensure the current player is still in the game
	if (defend.empty()) {
		p->endOfOrder = true;
		return;
	}
	vector<Territory*> attack = toAttack();
	Territory* strongestTerritory = p->getAtRiskTerritories().front();

	Territory* weakestTerritory = p->getAtRiskTerritories().back();
	Territory* vulnerableEnemy = attack.front();

	p->endOfOrder = false;
	switch (p->command)
	{
	case OT::DEPLOY: {
		// Create a deploy order and push it back in the orderlist
		if (p->chosenCard != nullptr) {
			p->chosenCard->play(p, deck);
			p->chosenCard = nullptr;
		}
		if (p->reinforcements != 0) {
			cout << "Creating deploy order" << endl;
			int nbOfreinforcements = p->reinforcements;


			cout << "Deploying " << nbOfreinforcements << " reinforcements to " << *strongestTerritory << endl;
			//deploy all reinforcements on weak countries
			Deploy* deploy = new Deploy(p,nbOfreinforcements, weakestTerritory);
			p->orderList->add(deploy);
			// all reinforcements were used, set to 0
			p->reinforcements = 0;
			cout << p->reinforcements << " reinforcements left." << endl;

		}
		break;
	}
	case OT::ADVANCE:
	{
		// benevolent players advance soldiers into its weakest territories, it cannot do attack advance order

		
		int advanceChoice{};
		Territory* from = nullptr;
		Territory* to = nullptr;
		//advance can only be defense
		advanceChoice = 0;
		
		//we send soldier to weakest territory
		if (advanceChoice == 0) {
			//from will be its strongest territory
			from = strongestTerritory;

			//to will be its weakest territory
			to = weakestTerritory;

		}
		
		int number = from->nbOfArmy;
		if (number == 0) {
			number = 1;
		}
		int soldierAmount = (rand() % number / 4) + 1;
		Advance* advance = new Advance(p, soldierAmount, from, to);
		p->orderList->add(advance);
		break;
	}

	case OT::AIRLIFT:
	case OT::BOMB:
	case OT::BLOCKADE:
	case OT::NEGOTIATE:
		cout << *(p->hand) << endl;
		p->chosenCard->play(p, deck);
		cout << *(p->hand) << endl;
		p->chosenCard = nullptr;
		break;
	}

	
}

vector<Territory*> BenevolentPlayerStrategy::toAttack()
{
	cout << p->name << " - Benevolent toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> BenevolentPlayerStrategy::toDefend()
{
	cout << p->name << " - Benevolent toDefend" << endl;
	return p->getAtRiskTerritories();
}
#pragma endregion

#pragma region Neutral
// Default Constructor
NeutralPlayerStrategy::NeutralPlayerStrategy() {}
// Copy Constructor
NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& s) {}
// Destructor
NeutralPlayerStrategy::~NeutralPlayerStrategy() {}
// Assignment Operator
NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& s) {
	return *this;
}
// Stream Insertion Operator. Doesn't work polymorphically.
std::ostream& operator<<(std::ostream& out, const NeutralPlayerStrategy& s) {
	out << "[NEUTRALPLAYERSTRATEGY]";
	return out;
}

void NeutralPlayerStrategy::issueOrder(Deck* deck)
{
	// (Computer player)
	// Never issues orders
	// TODO: Becomes an Aggressive Player if attacked
	cout << p->name << " - Neutral issueOrder" << endl;
	cout << "Neutral player does not issue any order" << endl;
	p->endOfOrder = true;
	
}

vector<Territory*> NeutralPlayerStrategy::toAttack()
{
	cout << p->name << " - Neutral toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> NeutralPlayerStrategy::toDefend()
{
	cout << p->name << " - Neutral toDefend" << endl;
	return p->getAtRiskTerritories();
}
#pragma endregion

#pragma region Cheater
// Default Constructor
CheaterPlayerStrategy::CheaterPlayerStrategy() {}
// Copy Constructor
CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& s) {}
// Destructor
CheaterPlayerStrategy::~CheaterPlayerStrategy() {}
// Assignment Operator
CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& s) {
	return *this;
}
// Stream Insertion Operator. Doesn't work polymorphically.
std::ostream& operator<<(std::ostream& out, const CheaterPlayerStrategy& s) {
	out << "[CHEATERPLAYERSTRATEGY]";
	return out;
}

void CheaterPlayerStrategy::issueOrder(Deck* deck)
{
	cout << p->name << " - Cheater issueOrder" << endl;
	// (Computer player)
	// Automatically conquers all territories that are adjacent to its own territories 
	// (once per turn)
	if (p->endOfOrder) return;

	vector<Territory*> toattack = toAttack();
	for (Territory* adjacentterritory : toattack) {
		p->addTerritory(adjacentterritory);
	}
	p->endOfOrder = true;
}

vector<Territory*> CheaterPlayerStrategy::toAttack()
{
	cout << p->name << " - Cheater toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> CheaterPlayerStrategy::toDefend()
{
	cout << p->name << " - Cheater toDefend" << endl;
	return p->getAtRiskTerritories();
}
#pragma endregion
