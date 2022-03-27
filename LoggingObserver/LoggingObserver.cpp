#include "LoggingObserver.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
// Filename for log data
const std::string logFilename = "gamelog.txt";

#pragma region ILoggable
// Default Constructor (can't be used, ILoggable is abstract)
ILoggable::ILoggable() {}
// Destructor
ILoggable::~ILoggable() {}

// Assignment Operator Overload
ILoggable& ILoggable::operator=(const ILoggable&) {
	return *this;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const ILoggable& s) {
	out << "[ILOGGABLE]";
	return out;
}
#pragma endregion

#pragma region Subject
// Default Constructor
Subject::Subject() {
	_observers = new std::list<Observer*>;
}
// Copy Constructor
Subject::Subject(const Subject& s) {
	_observers = s._observers;
}
// Destructor
Subject::~Subject() {
	delete _observers;
}

// Subscribes an Observer class to changes within the Subject class
void Subject::attach(Observer* o) {
	_observers->push_back(o);
}
// Unsubscribes an Observer class from changes within the Subject class
void Subject::detach(Observer* o) {
	_observers->remove(o);
}

// A object of type Subject calls this function to notify all observers that a change occurred
void Subject::notify(ILoggable* il) {
	cout << "	Inside Subject::notify for: " << typeid(*il).name() << endl;
	// Tell all Observer Subscribed to this Subject class
	for (Observer* s : *_observers)
		s->update(il);
}
// Assignment Operator
Subject& Subject:: operator=(const Subject& s) {
	this->_observers = s._observers;
	return *this;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const Subject& s) {
	out << "[SUBJECT]";
	return out;
}
#pragma endregion

#pragma region Observer
// Default Constructor
Observer::Observer() {}
// Copy Constructor
Observer::Observer(const Observer& o) {}
// Destructor
Observer::~Observer() {}

// Assignment Operator
Observer& Observer:: operator=(const Observer& o) {
	return *this;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const Observer& o) {
	out << "[OBSERVER]";
	return out;
}
#pragma endregion

#pragma region LogObserver
// Default Constructor
LogObserver::LogObserver() {}
// Copy Constructor
LogObserver::LogObserver(const LogObserver& o) {}
// Destructor
LogObserver::~LogObserver() {}

/*
* Called from Subject::notify
* Calls the stringToLog implementation of an ILoggable class to write relevant data to the log file.
*/ 
void LogObserver::update(ILoggable* il) {
	cout << "	Inside LogObserver::update for: " << typeid(*il).name() << endl;
	std::ofstream file(logFilename, std::ios_base::app);
	file << il->stringToLog() << std::endl;
	file.close();
}
// Assignment Operator
LogObserver& LogObserver:: operator=(const LogObserver& o) {
	return *this;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const LogObserver& o) {
	out << "[LOGOBSERVER]";
	return out;
}
#pragma endregion