#include "PlayerStrategies.h"

#pragma region Base
// Default Constructor. Unusable due to being Pure Virtual Class
PlayerStrategy::PlayerStrategy() {}
// Copy Construtor. Unusable due to being Pure Virtual Class
PlayerStrategy::PlayerStrategy(const PlayerStrategy& s) {}
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

vector<Territory*> PlayerStrategy::getAdjacentTerritories() {
	vector<Territory*> toattack;
	for (Territory* territory : p->territories) {
		for (Territory* adjacentterritory : territory->adjacentTerritories) {
			if (adjacentterritory->owner != p) {
				toattack.push_back(adjacentterritory);
			}
		}
	}
	return toattack;
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
	// Requires user interaction to make decisions
}

vector<Territory*> HumanPlayerStrategy::toAttack()
{
	return vector<Territory*>();
}

vector<Territory*> HumanPlayerStrategy::toDefend()
{
	return vector<Territory*>();
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
	// (Computer player)
	// Focuses on attack
	// Deploys/Advances armies on its strongest country then always advances to enemy territories until it can't
}

vector<Territory*> AggressivePlayerStrategy::toAttack()
{
	return vector<Territory*>();
}

vector<Territory*> AggressivePlayerStrategy::toDefend()
{
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
	return vector<Territory*>();
}

vector<Territory*> BenevolentPlayerStrategy::toDefend()
{
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
	return vector<Territory*>();
}

vector<Territory*> NeutralPlayerStrategy::toDefend()
{
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
}

vector<Territory*> CheaterPlayerStrategy::toAttack()
{
	return getAdjacentTerritories();
}

vector<Territory*> CheaterPlayerStrategy::toDefend()
{
	return vector<Territory*>();
}
#pragma endregion
