#include "PlayerStrategies.h"
#include <iostream>

using std::cout;
using std::endl;

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

void HumanPlayerStrategy::issueOrder()
{
	cout << "HumanPlayer issueOrder" << endl;
    // Requires user interaction to make decisions
}

vector<Territory*> HumanPlayerStrategy::toAttack()
{
    cout << "HumanPlayer toAttack()" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> HumanPlayerStrategy::toDefend()
{
    cout << "HumanPlayer toDefend()" << endl;
    vector<Territory*> toDefend(p->territories.begin(), p->territories.end());
	return toDefend;
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

void AggressivePlayerStrategy::issueOrder()
{
    cout << "AggressivePlayer issueOrder" << endl;
	// (Computer player)
	// Focuses on attack
	// Deploys/Advances armies on its strongest country then always advances to enemy territories until it can't
}

vector<Territory*> AggressivePlayerStrategy::toAttack()
{
    cout << "AggressivePlayer toAttack" << endl;
	return vector<Territory*>();
}

vector<Territory*> AggressivePlayerStrategy::toDefend()
{
    cout << "AggressivePlayer toDefend" << endl;
	return vector<Territory*>();
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

void BenevolentPlayerStrategy::issueOrder()
{
	// (Computer player)
	// Focuses on protecting its weak countries
	// Deploys/Advances on its weakest countries, never advances to enemy territories
}

vector<Territory*> BenevolentPlayerStrategy::toAttack()
{
    cout << "BenevolentPlayer toAttack" << endl;
    return p->getAdjacentTerritories();
}

vector<Territory*> BenevolentPlayerStrategy::toDefend()
{
    cout << "BenevolentPlayer toDefend" << endl;
	return vector<Territory*>();
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

void NeutralPlayerStrategy::issueOrder()
{

	// (Computer player)
	// Never issues orders
	// Becomes an Aggressive Player if attacked
}

vector<Territory*> NeutralPlayerStrategy::toAttack()
{
    cout << "NeutralPlayer toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> NeutralPlayerStrategy::toDefend()
{
    cout << "NeutralPlayer toDefend" << endl;
	return vector<Territory*>();
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

void CheaterPlayerStrategy::issueOrder()
{
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
    cout << "CheaterPlayer toAttack" << endl;
	return p->getAdjacentTerritories();
}

vector<Territory*> CheaterPlayerStrategy::toDefend()
{
    cout << "CheaterPlayer toDefend" << endl;
	return vector<Territory*>();
}
#pragma endregion
