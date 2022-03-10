#include "Orders.hpp"
#include <string>

// Method, Constructor, and Operator implementation

	class Continent;
	class Territory;

	using std::cout;

Order::Order()		// Constructor								// Order Method Implementations
{
	cout << "Making an Order\n";
	this->className = new string("Order");
	this->validity = new bool(false);
}
Order::~Order() // Destructor
{
	cout << "Destroying an Order" << endl;
	delete (className);
	delete (validity);
	className = NULL;
	validity = NULL;
}

Order::Order(const Order& o) // Copy Constructor
{
	cout << "Copying an Order" << endl;
	this->validity = new bool(*(o.validity));
	this->className = new string(*(o.className));
}

Order& Order::operator =(const Order& o) // Assignment Operator Overload
{
	this->validity = new bool(*(o.validity));
	this->className = new string(*(o.className));
	return *this;
}

std::string Order::stringToLog() {
	return "Order: Base order ";
}

bool& Order::getValidity()		// Accessor method				
{
	return *validity;
}

string& Order::getClassName()		//accessor method				
{
	return *className;
}

ostream& operator<<(ostream& strm, Order& o)			// Free stream insertion implementation
{
	return strm << "Order Type: " << *o.className;
}

OrderList::OrderList()	// Constructor				// OrderList Method Implementations
{
	cout << "Making an OrderList\n";
}

ostream& operator<<(ostream& out, OrderList& olist) {
	out << "OrderList: [";
	bool first = true;
	for (Order* order : olist.list) {
		if (!first) {
			out << ", ";
		}
		first = false;
		out << *order;
	}
	out << "]" << endl;
	return out;
}

void OrderList::move(int from, int to)
{
	if (from < list.size() && from >= 0 && to < list.size() && to >= 0) {
		Order* temp = list.at(from);
		if (from < to)
		{
			for (int i = from; i <= to; i++)
			{
				list.at(i) = list.at(i + 1);
			}
			list.at(to) = temp;
			temp = NULL;
		}
		else if (from > to)
		{
			for (int i = from; i >= to; i--)
			{
				list.at(i) = list.at(i - 1);
			}
			list.at(to) = temp;
		}
		else
		{
			cout << "Invalid Move" << endl;
		}
	}
}
void OrderList::add(Order* order) {
	list.push_back(order);
	notify(this);
}

void OrderList::remove(int position)	// The remove function removes the Order object from the orderList 
										// as well as deletes the data stored in the heap to avoid memory leaks
{
	if (position < list.size() && position >= 0) {
		Order* temp = list.at(position);
		delete (temp);
		temp = NULL;
		list.erase(list.begin() + position);
	}
	else
	{
		cout << "Invalid Move" << endl;
	}
}

OrderList::~OrderList() // Destructor
{
	cout << "Destroying an OrderList" << endl;
	for (auto order : this->list) delete order;
}

OrderList::OrderList(const OrderList& o) // Copy Constructor
{
	cout << "Copying an OrderList" << endl;
	this->list = o.list;
}

OrderList& OrderList::operator=(const OrderList& o)
{							// Assignment Operator Overload
	this->list = o.list;
	return *this;
}

std::string OrderList::stringToLog() {
	return "OrderList: Added a " + list.back()->getClassName() + " order";
}

Deploy::Deploy(Player* issuingPlayer, int numOfArmies, Territory* location) : Order(), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), location(location)
{										// Constructor
	cout << "Making a Deploy" << endl;
	delete (className);
	className = NULL;
	Deploy::className = new string("Deploy");
}

void Deploy::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Deploy Order..." << endl;
	if (issuingPlayer->territories.count(this->location) > 0)
	{
		*validity = true;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Deploy::execute()
{
	Deploy::validate();
	if (getValidity())
	{
		cout << "Executing Deploy..." << endl;

		this->location->nbOfArmy += numOfArmies;
	}
	else 
		cout << "Deploy Order not valid." << endl;
	
	notify(this);
}

Deploy::~Deploy() // Destructor
{
	cout << "Destroying a Deploy" << endl;
	delete (className);
	delete (validity);
	className = NULL;
	validity = NULL;
}

Deploy::Deploy(const Deploy& d) : Order(d), issuingPlayer(d.issuingPlayer), numOfArmies(d.numOfArmies), location(d.location)
{											// Copy Constructor
	cout << "Copying a Deploy" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(d.className));
}

Deploy& Deploy::operator=(const Deploy& d) // Assignment Operator Overload
{
	Order::operator =(d);
	delete (className);
	className = NULL;
	this->className = new string(*(d.className));
	this->issuingPlayer = d.issuingPlayer;
	this->numOfArmies = d.numOfArmies;
	this->location = d.location;
	return *this;
}

std::string Deploy::stringToLog() {
	return "Order: " + issuingPlayer->name + " deployed " + std::to_string(numOfArmies) + " units to " + location->name;
}

Advance::Advance(Player* issuingPlayer, int numOfArmies, Territory* to, Territory* from) : Order(), attacking(false), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), to(to), from(from)
{											// Constructor
	cout << "Making an Advance" << endl;
	delete (className);
	className = NULL;
	Advance::className = new string("Advance");
}

void Advance::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Advance Order..." << endl;
	if (issuingPlayer->territories.count(this->from) > 0)
	{
		if (issuingPlayer->territories.count(this->to) > 0)
		{
			for (Territory* var : to->adjacentTerritories)
			{
				if (var == from)
				{
					*validity = true;
				}
			}
		}
		else if (issuingPlayer->territories.count(this->to) <= 0)
		{
			
			for (Territory* var : to->adjacentTerritories)
			{
				if (var == from)
				{
					*validity = true;
					attacking = true;
				}
			}
		}
	}
}
// the execute function will check validation before implementing the functionality of the order
void Advance::execute()
{
	Advance::validate();
	if (getValidity())
	{
		cout << "Executing Advance..." << endl;

		if (attacking)
		{

		}

		
	}
	else
		cout << "Advance Order not valid." << endl;
	
	notify(this);
}

Advance::~Advance() // Destructor
{
	cout << "Destroying an Advance" << endl;
}

Advance::Advance(const Advance& a) : Order(a), attacking(a.attacking), issuingPlayer(a.issuingPlayer), numOfArmies(a.numOfArmies), to(a.to), from(a.from)
{											// Copy Constructor
	cout << "Copying an Advance" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(a.className));
}

Advance& Advance::operator=(const Advance& a) // Assignment Operator Overload
{
	Order::operator =(a);
	delete (className);
	className = NULL;
	this->className = new string(*(a.className));
	this->issuingPlayer = a.issuingPlayer;
	this->numOfArmies = a.numOfArmies;
	this->to = a.to;
	this->from = a.from;
	return *this;
}

std::string Advance::stringToLog() {
	return "Order: " + issuingPlayer->name + "advanced " + std::to_string(numOfArmies) + " from " + from->name + " to " + to->name;
}

Bomb::Bomb(Player* issuingPlayer, Territory* location) : Order(), issuingPlayer(issuingPlayer), location(location)
{										// Constructor
	cout << "Making a Bomb" << endl;
	delete (className);
	className = NULL;
	Bomb::className = new string("Bomb");
}

Bomb::~Bomb() // Destructor
{
	cout << "Destroying a Bomb" << endl;
}

Bomb::Bomb(const Bomb& b) : Order(b), issuingPlayer(b.issuingPlayer), location(b.location)
{										// Copy Constructor
	cout << "Copying an Bomb" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(b.className));
}

Bomb& Bomb::operator=(const Bomb b) // Assignment Operator Overload
{
	Order::operator =(b);
	delete (className);
	className = NULL;
	this->className = new string(*(b.className));
	this->issuingPlayer = b.issuingPlayer;
	this->location = b.location;
	return *this;
}

void Bomb::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Bomb Order..." << endl;
}
// the execute function will check validation before implementing the functionality of the order
void Bomb::execute()
{
	Bomb::validate();
	cout << "Executing Bomb..." << endl;
	notify(this);
}

std::string Bomb::stringToLog() {
	return "Order: " + issuingPlayer->name + " bombed " + location->name;
}

Blockade::Blockade(Player* issuingPlayer, Territory* location) : Order(), issuingPlayer(issuingPlayer), location(location)
{											// Constructor
	cout << "Making a Blockade" << endl;
	delete (className);
	className = NULL;
	Blockade::className = new string("Blockade");
}

Blockade::~Blockade() // Destructor
{
	cout << "Destroying a Blockade" << endl;
}

void Blockade::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Blockade Order..." << endl;
}
// the execute function will check validation before implementing the functionality of the order
void Blockade::execute()
{
	Blockade::validate();
	cout << "Executing Blockade..." << endl;
	notify(this);
}

Blockade::Blockade(const Blockade& b) : Order(b), issuingPlayer(b.issuingPlayer), location(b.location)
{											// Copy Constructor
	cout << "Copying a Blockade" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(b.className));
}

Blockade& Blockade::operator=(const Blockade& b) // Assignment Operator Overload
{
	Order::operator =(b);
	delete (className);
	className = NULL;
	this->className = new string(*(b.className));
	this->issuingPlayer = b.issuingPlayer;
	this->location = b.location;
	return *this;
}

std::string Blockade::stringToLog() {
	return "Order: " + issuingPlayer->name + " blockaded " + location->name;
}

Airlift::Airlift(Player* issuingPlayer, int numOfArmies, Territory* to, Territory* from) : Order(), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), to(to), from(from)
{											// Constructor
	cout << "Making an Airlift" << endl;
	delete (className);
	className = NULL;
	Airlift::className = new string("Airlift");
}

void Airlift::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Airlift Order..." << endl;
}
// the execute function will check validation before implementing the functionality of the order
void Airlift::execute()
{
	Airlift::validate();
	cout << "Executing Airlift..." << endl;
	notify(this);
}

Airlift::~Airlift() // Destructor
{
	cout << "Destroying an Airlift" << endl;
}

Airlift::Airlift(const Airlift& a) : Order(a), issuingPlayer(a.issuingPlayer), numOfArmies(a.numOfArmies), to(a.to), from(a.from)
{											// Copy Constructor
	cout << "Copying an Airlift" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(a.className));
}

Airlift& Airlift::operator=(const Airlift& a) // Assignment Operator Overload
{
	Order::operator =(a);
	delete (className);
	className = NULL;
	this->className = new string(*(a.className));
	this->issuingPlayer = a.issuingPlayer;
	this->numOfArmies = a.numOfArmies;
	this->to = a.to;
	this->from = a.from;
	return *this;
}

std::string Airlift::stringToLog() {
	return "Order: " + issuingPlayer->name + " airlifted " + std::to_string(numOfArmies) + " from " + from->name + " to " + to->name;
}

Negotiate::Negotiate(Player* issuingPlayer, Player* targetPlayer) : Order(), issuingPlayer(issuingPlayer), targetPlayer(targetPlayer)
{											// Constructor
	cout << "Making a Negotiate" << endl;
	delete (className);
	className = NULL;
	Negotiate::className = new string("Negotiate");
}

Negotiate::~Negotiate() // Destructor
{
	cout << "Destroying a Negotiate" << endl;
}

Negotiate::Negotiate(const Negotiate& n) : Order(n), issuingPlayer(n.issuingPlayer), targetPlayer(n.targetPlayer)
{											// Copy Constructor
	cout << "Copying a Negotiate" << endl;
	delete (className);
	className = NULL;
	this->className = new string(*(n.className));
}

Negotiate& Negotiate::operator=(const Negotiate& n) // Assignment Operator Overload
{
	Order::operator =(n);
	delete (className);
	className = NULL;
	this->className = new string(*(n.className));
	this->issuingPlayer = n.issuingPlayer;
	this->targetPlayer = n.targetPlayer;
	return *this;
}

void Negotiate::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Negotiate Order..." << endl;
}
// the execute function will check validation before implementing the functionality of the order
void Negotiate::execute()
{
	Negotiate::validate();
	cout << "Executing Negotiate..." << endl;
	notify(this);
}

std::string Negotiate::stringToLog() {
	return "Order: " + issuingPlayer->name + " negotiated with " + targetPlayer->name;
}
