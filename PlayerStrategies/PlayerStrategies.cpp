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

void HumanPlayerStrategy::issueOrder()
{
	cout << p->name << " - Human issueOrder" << endl;
    // Requires user interaction to make decisions
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

void AggressivePlayerStrategy::issueOrder()
{
	cout << p->name << " - Aggressive issueOrder" << endl;
	// (Computer player)
	// Focuses on attack
	// Deploys/Advances armies on its strongest country then always advances to enemy territories until it can't
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

void BenevolentPlayerStrategy::issueOrder()
{
    cout << p->name << " - Benevolent issueOrder" << endl;
	// (Computer player)
	// Focuses on protecting its weak countries
	// Deploys/Advances on its weakest countries, never advances to enemy territories
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

void NeutralPlayerStrategy::issueOrder()
{
    cout << p->name << " - Neutral issueOrder" << endl;
	// (Computer player)
	// Never issues orders
	// Becomes an Aggressive Player if attacked
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

void CheaterPlayerStrategy::issueOrder()
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
