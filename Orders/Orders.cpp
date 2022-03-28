#include "Orders.hpp"
#include "../Player/Player.h"
#include <string>

// Method, Constructor, and Operator implementation

	class Continent;
	class Territory;

	using std::cout;

Order::Order()		// Constructor								// Order Method Implementations
{
	this->className = new string("Order");
	this->validity = new bool(false);
}
Order::~Order() // Destructor
{
	delete (className);
	delete (validity);
	className = NULL;
	validity = NULL;
}

Order::Order(const Order& o) // Copy Constructor
{
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
	return "Order Issued: Base order ";
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

// OrderList Method Implementations
// Constructor	
OrderList::OrderList() {}

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

void OrderList::normalize(int& index) {
	int size = list.size();
	if (size == 0) {
		index = 0;
	}
	else if (index < 0) {
		index = 0;
	}
	else if (index >= size) {
		index = size - 1;
	}
}

void OrderList::move(int from, int to)
{
	if (from == to) return;

	normalize(from);
	normalize(to);

	Order* order = list[from];

	list.erase(list.begin() + from);
	list.insert(list.begin() + to, order);
}

// Add an order to the players orderlist
// Attach all orderlist observers to the order
void OrderList::add(Order* order) {
	list.push_back(order);
	for (auto observer : *_observers)
		order->attach(observer);
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
	for (auto order : this->list) delete order;
}

OrderList::OrderList(const OrderList& o) // Copy Constructor
{
	this->list = o.list;
}

OrderList& OrderList::operator=(const OrderList& o)
{							// Assignment Operator Overload
	this->list = o.list;
	return *this;
}

std::string OrderList::stringToLog() {
	return "Order Issued: " + list.back()->getClassName();
}

Deploy::Deploy(Player* issuingPlayer, int numOfArmies, Territory* location) : Order(), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), location(location)
{										// Constructor
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
		cout << "Deploy Validated" << endl;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Deploy::execute()
{
	this->validate();
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
	delete (className);
	delete (validity);
	className = NULL;
	validity = NULL;
}

Deploy::Deploy(const Deploy& d) : Order(d), issuingPlayer(d.issuingPlayer), numOfArmies(d.numOfArmies), location(d.location)
{											// Copy Constructor
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
	return "Order Executed: " + issuingPlayer->name + " deployed " + std::to_string(numOfArmies) + " units to " + location->name;
}

Advance::Advance(Player* issuingPlayer, int numOfArmies, Territory* to, Territory* from) : Order(), attacking(false), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), to(to), from(from)
{											// Constructor
	delete (className);
	className = NULL;
	Advance::className = new string("Advance");
}

void Advance::validate() // Will validate the circumstances of the object before executing
{	
	cout << "Validating Advance Order..." << endl;
	// TODO FIX NB OF ARMIES SENT
	if (issuingPlayer == from->owner && Territory::territoriesAreAdjacent(from, to)) {
		numOfArmies = from->nbOfArmy;
		Player* targetPlayer = to->owner;
		if (issuingPlayer != targetPlayer) {
			if (!issuingPlayer->isFriendlyPlayer(targetPlayer)) {
				cout << "Valid order from " << *issuingPlayer << " to attack " << *targetPlayer << endl;
				attacking = true;
				*validity = true;
			}
			else {
				cout << issuingPlayer->name << " tried to attack friendly player " << targetPlayer->name << ". Invalid order" << endl;
			}
		}
		else {
			*validity = true;
		}
		
	}
	else {
		cout << "Invalid advance order." << endl;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Advance::execute()
{
	this->validate();
	if (getValidity())
	{
		cout << "Executing Advance..." << endl;

		if (attacking)
		{
			srand((unsigned)time(NULL));
			cout << "Doing Battle..." << endl;
			int numOfArmyCopy = this->numOfArmies;
			float rand_num = 0.0;

			while (to->nbOfArmy != 0 || numOfArmyCopy != 0)
			{
				if (to->nbOfArmy != 0)
				{
					rand_num = (float)rand() / RAND_MAX;
					cout << rand_num;

					if (rand_num >= 0.3)
					{
						numOfArmyCopy -= 1;
						cout << "Attacking army unit destroyed" << endl;
					}
				}
				if (to->nbOfArmy == 0) 
				{
					cout << "Attacking army destroyed, "+to->name+" succesfully defended." << endl;
					cout << to->name + " has " + to_string(to->nbOfArmy) + " remaining on the territory." << endl;
				}
				if (numOfArmyCopy != 0) 
				{
					rand_num = (float)rand() / RAND_MAX;
					cout << rand_num;

					if (rand_num >= 0.4)
					{
						to->nbOfArmy -= 1;
						cout << "Defending army unit destroyed" << endl;
					}
				}
				if (numOfArmyCopy == 0)
				{
					cout << "Defending army destroyed, " + to->name + " succesfully conquered by " + issuingPlayer->name + "." << endl;
					to->owner = issuingPlayer;
					to->nbOfArmy = numOfArmyCopy;
					issuingPlayer->conquered = true;
					cout << issuingPlayer->name + " now has " + to_string(to->nbOfArmy) + " armies on the territory." <<endl;
				}
			}
		}
		else {
			from->nbOfArmy -= numOfArmies;
			to->nbOfArmy += numOfArmies;
			cout << "Moved " << numOfArmies << " soldiers from " << *from << " to " << *to << endl;
		}
	}
	else
		cout << "Advance Order not valid." << endl;
	attacking = false;
	notify(this);
}

Advance::~Advance() {}// Destructor

Advance::Advance(const Advance& a) : Order(a), attacking(a.attacking), issuingPlayer(a.issuingPlayer), numOfArmies(a.numOfArmies), to(a.to), from(a.from)
{											// Copy Constructor
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
	return "Order Executed: " + issuingPlayer->name + "advanced " + std::to_string(numOfArmies) + " from " + from->name + " to " + to->name;
}

Bomb::Bomb(Player* issuingPlayer, Territory* location) : Order(), issuingPlayer(issuingPlayer), location(location)
{										// Constructor
	delete (className);
	className = NULL;
	Bomb::className = new string("Bomb");
}

Bomb::~Bomb() {}// Destructor

Bomb::Bomb(const Bomb& b) : Order(b), issuingPlayer(b.issuingPlayer), location(b.location)
{										// Copy Constructor
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
	//check if the territory does not belong to the player
	if (location->owner != issuingPlayer) {
		// check if the territory to attack is adjacent to the territories owned by the player
		for (Territory* t : issuingPlayer->territories) {
			for (Territory* adjacentT : t->adjacentTerritories) {
				if (adjacentT == location) {
					cout << "Validation complete" << endl;
					*validity = true;
				}
				else {
					*validity = false;
					cout << "Invalid order...Territory is not within reach" << endl;
				}
			}
		}
	}
	else {
		*validity = false;
		cout << "Invalid order...Cannot attack itself" << endl;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Bomb::execute()
{
	this->validate();
	if (getValidity()) {
		cout << "Executing Bomb..." << endl;
		location->nbOfArmy /= 2; //half the army is wiped
		cout << "Bombed territory has: " << location->nbOfArmy << " soldiers" << endl;
		notify(this);
	}
}

std::string Bomb::stringToLog() {
	return "Order Executed: " + issuingPlayer->name + " bombed " + location->name;
}

Blockade::Blockade(Player* issuingPlayer, Territory* location) : Order(), issuingPlayer(issuingPlayer), location(location)
{											// Constructor
	delete (className);
	className = NULL;
	Blockade::className = new string("Blockade");
}

// Destructor
Blockade::~Blockade() {}

void Blockade::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Blockade Order..." << endl;
	if (location->owner == issuingPlayer) {
		*validity = true;
		cout << "Blockade Order validated" << endl;
	}
	else {
		*validity = false;
		cout << "Cannot do a blockade on an unknown/enemy" << endl;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Blockade::execute()
{
	this->validate();
	if (getValidity()) {
		cout << "Executing Blockade..." << endl;
		location->nbOfArmy *= 2; // double number of players
		location->owner = Player::neutralplayer; //transfer ownership to neutral player
		issuingPlayer->territories.erase(issuingPlayer->territories.find(location)); //delete the territory from the issuing player
		cout << "Blockade executed on " << location->name << ". " << location->name << " is now owned by the Neutral Player" << endl;
		cout << location->name << " now has: " << location->nbOfArmy << " soldiers" << endl;
		notify(this);
	}
}

Blockade::Blockade(const Blockade& b) : Order(b), issuingPlayer(b.issuingPlayer), location(b.location)
{											// Copy Constructor
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
	return "Order Executed: " + issuingPlayer->name + " blockaded " + location->name;
}

Airlift::Airlift(Player* issuingPlayer, int numOfArmies, Territory* to, Territory* from) : Order(), issuingPlayer(issuingPlayer), numOfArmies(numOfArmies), to(to), from(from)
{											// Constructor
	delete (className);
	className = NULL;
	Airlift::className = new string("Airlift");
}

void Airlift::validate() // Will validate the circumstances of the object before executing
{
	cout << "Validating Airlift Order..." << endl;

	//validate if the source and target are owned by the same player
	if (to->owner == issuingPlayer && from->owner == issuingPlayer) {
		if (numOfArmies > from->nbOfArmy) {
			*validity = false;
			cout << "Cannot airlift more soldiers than in the territory" << endl;
		}
		else {
			*validity = true;
			cout << "Validation completed" << endl;
		}
	}
	else {
		*validity = false;
		cout << "Invalid order...Cannot airlift to a unknown/enemy territory " << endl;
	}

}
// the execute function will check validation before implementing the functionality of the order
void Airlift::execute()
{
	this->validate();
	if (getValidity()) {
		cout << "Executing Airlift..." << endl;
		
		from->nbOfArmy -= numOfArmies; //substract number of soldiers sent
		to->nbOfArmy += numOfArmies; //add number of soldiers 
		cout << to->name << " has received " << numOfArmies << " soldiers by plane from " << from->name << endl;
		cout << from->name << " has: " << from->nbOfArmy << " soldiers" << endl;
		cout << to->name << " has: " << to->nbOfArmy << " soldiers" << endl;
		notify(this);
	}
}

Airlift::~Airlift() // Destructor
{
}

Airlift::Airlift(const Airlift& a) : Order(a), issuingPlayer(a.issuingPlayer), numOfArmies(a.numOfArmies), to(a.to), from(a.from)
{											// Copy Constructor
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
	return "Order Executed: " + issuingPlayer->name + " airlifted " + std::to_string(numOfArmies) + " from " + from->name + " to " + to->name;
}

Negotiate::Negotiate(Player* issuingPlayer, Player* targetPlayer) : Order(), issuingPlayer(issuingPlayer), targetPlayer(targetPlayer)
{											// Constructor
	delete (className);
	className = NULL;
	Negotiate::className = new string("Negotiate");
}

Negotiate::~Negotiate() // Destructor
{
}

Negotiate::Negotiate(const Negotiate& n) : Order(n), issuingPlayer(n.issuingPlayer), targetPlayer(n.targetPlayer)
{											// Copy Constructor
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
	if (issuingPlayer != targetPlayer) {
		*validity = true;
		cout << "Negotiate Order validated" << endl;
	}
	else {
		*validity = false;
		cout << "Cannot attack yourself" << endl;
	}
}
// the execute function will check validation before implementing the functionality of the order
void Negotiate::execute()
{
	this->validate();
	if (getValidity()) {
		issuingPlayer->addFriendlyPlayer(targetPlayer);
		targetPlayer->addFriendlyPlayer(issuingPlayer);

		cout << *issuingPlayer << " cannot attack " << *targetPlayer << " until the end of the round." << endl;
			

		notify(this);
	}
}

std::string Negotiate::stringToLog() {
	return "Order Executed: " + issuingPlayer->name + " negotiated with " + targetPlayer->name;
}
