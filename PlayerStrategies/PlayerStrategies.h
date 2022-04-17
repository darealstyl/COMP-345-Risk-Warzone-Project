 #pragma once
#include "../Map/Map.h"
#include "../Player/Player.h"

#include <vector>
#include <unordered_set>
#include <string>

using std::vector;
using std::string;
using std::unordered_set;

class Territory;
class Player;

class PlayerStrategy {
public:	
	static const unordered_set<std::string> strategystrings;
	virtual ~PlayerStrategy();
	
	PlayerStrategy& operator=(const PlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const PlayerStrategy&);
	
	void setPlayerLink(Player* p);

	virtual void issueOrder(Deck*) = 0;
	virtual vector<Territory*> toAttack() = 0;
	virtual vector<Territory*> toDefend() = 0;
protected:
	Player* p;

	PlayerStrategy();
	PlayerStrategy(const PlayerStrategy&);
};

class HumanPlayerStrategy : public PlayerStrategy {
public:
	HumanPlayerStrategy();
	HumanPlayerStrategy(const HumanPlayerStrategy&);
	~HumanPlayerStrategy();
	HumanPlayerStrategy& operator=(const HumanPlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const HumanPlayerStrategy&);

	void issueOrder(Deck*);
	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
};

class AggressivePlayerStrategy : public PlayerStrategy {
public:
	AggressivePlayerStrategy();
	AggressivePlayerStrategy(const AggressivePlayerStrategy&);
	~AggressivePlayerStrategy();
	AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const AggressivePlayerStrategy&);

	void issueOrder(Deck*);
	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
};

class BenevolentPlayerStrategy : public PlayerStrategy {
public:
	BenevolentPlayerStrategy();
	BenevolentPlayerStrategy(const BenevolentPlayerStrategy&);
	~BenevolentPlayerStrategy();
	BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const BenevolentPlayerStrategy&);

	void issueOrder(Deck*);
	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
};

class NeutralPlayerStrategy : public PlayerStrategy {
public:
	NeutralPlayerStrategy();
	NeutralPlayerStrategy(const NeutralPlayerStrategy&);
	~NeutralPlayerStrategy();
	NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const NeutralPlayerStrategy&);

	void issueOrder(Deck*);
	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
};

class CheaterPlayerStrategy : public PlayerStrategy {
public:
	CheaterPlayerStrategy();
	CheaterPlayerStrategy(const CheaterPlayerStrategy&);
	~CheaterPlayerStrategy();
	CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy&);
	friend std::ostream& operator<<(std::ostream&, const CheaterPlayerStrategy&);

	void issueOrder(Deck*);
	vector<Territory*> toAttack();
	vector<Territory*> toDefend();
};