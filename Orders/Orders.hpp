#pragma once
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include "../Map/Map.h"

using namespace std;
class Territory;
// ------------------------------------------------------------
// Declaration of OrdersList, Order and all subclasses of Order
// ------------------------------------------------------------

class Order {
	bool validity = false;
protected:
	string* className;
	friend ostream& operator<<(ostream&, Order&);
public:
	Order();
	~Order();
	Order(const Order& o);
	Order& operator =(const Order& o);
	bool getValidity();
	string* getClassName();
	//void execute();
};

class OrderList {
	// List as a vector for dynamic resizing of List
	// List of Order objects
public:
	vector<Order*> list;
	OrderList();
	void move(int from, int to);
	void remove(int position);
};

class Deploy : public Order {
public:
	Deploy();
	void execute(int numOfArmies, Territory* location);
};

class Advance : public Order {
public:
	Advance();
	void execute(int numOfArmies, Territory* from, Territory* to);
};

class Bomb : public Order {
public:
	Bomb();
	void execute(Territory* location);
};

class Blockade : public Order {
public:
	Blockade();
	void execute(Territory* location);
};

class Airlift : public Order {
public:
	Airlift();
	void execute(int numOfArmies, Territory* from, Territory* to);
};

class Negotiate : public Order {
public:
	Negotiate();
	void execute(string targetPlayer);
};