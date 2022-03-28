#pragma once
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include "../Map/Map.h"
#include "../LoggingObserver/LoggingObserver.h"

using namespace std;

class Player;
class Territory;
class Continent;

// ------------------------------------------------------------
// Declaration of OrderList, Order and all subclasses of Order
// ------------------------------------------------------------

class Order : public Subject, public ILoggable {
protected:
	bool validity;
	string className;
	friend ostream& operator<<(ostream&, Order&);
public:
	Order();
	virtual ~Order();
	Order(const Order& o);
	Order& operator =(const Order& o);
	bool getValidity();
	string getClassName();
	virtual void validate() = 0;
	virtual void execute() = 0;
	
	std::string stringToLog() override;
};

class OrderList : public Subject, public ILoggable {
								// List as a vector for dynamic resizing of List
								// List of Order objects
public:
	vector<Order*> list;
	OrderList();
	~OrderList();
	OrderList(const OrderList& o);

	friend ostream& operator<<(ostream&, OrderList&);
	OrderList& operator=(const OrderList& o);
	void move(int from, int to);
	void add(Order*);
	void remove(int position);

	std::string stringToLog() override;

private:
	void normalize(int&);
};

class Deploy : public Order {
	Player* issuingPlayer;
	int numOfArmies;
	Territory* location;
public:
	Deploy(Player*, int, Territory*);
	virtual ~Deploy();
	Deploy(const Deploy& d);
	Deploy& operator=(const Deploy& d);
	void validate();
	void execute();

	std::string stringToLog() override;
};

class Advance : public Order {
	Player* issuingPlayer;
	int numOfArmies;
	bool attacking;
	Territory* to;
	Territory* from;
public:
	Advance(Player*, int, Territory*, Territory*);
	virtual ~Advance();
	Advance(const Advance& a);
	Advance& operator=(const Advance& a);
	void validate();
	void execute();

	std::string stringToLog() override;
};

class Bomb : public Order {
	Player* issuingPlayer;
	Territory* location;
public:
	Bomb(Player*, Territory*);
	virtual ~Bomb();
	Bomb(const Bomb& b);
	Bomb& operator=(const Bomb b);
	void validate();
	void execute();

	std::string stringToLog() override;
};

class Blockade : public Order {
	Player* issuingPlayer;
	Territory* location;
public:
	Blockade(Player*, Territory*);
	virtual ~Blockade();
	Blockade(const Blockade& b);
	Blockade& operator=(const Blockade& b);
	void validate();
	void execute();

	std::string stringToLog() override;
};

class Airlift : public Order {
	Player* issuingPlayer;
	int numOfArmies;
	Territory* to;
	Territory* from;
public:
	Airlift(Player*, int, Territory*, Territory*);
	virtual ~Airlift();
	Airlift(const Airlift& a);
	Airlift& operator=(const Airlift& a);
	void validate();
	void execute();

	std::string stringToLog() override;
};

class Negotiate : public Order {
	Player* issuingPlayer;
	Player* targetPlayer;
public:
	Negotiate(Player*, Player*);
	virtual ~Negotiate();
	Negotiate(const Negotiate& n);
	Negotiate& operator=(const Negotiate& n);
	void validate();
	void execute();

	std::string stringToLog() override;
};