#include "LoggingObserver.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
const std::string logFilename = "gamelog.txt";

#pragma region ILoggable
ILoggable::~ILoggable() {}

ILoggable& ILoggable::operator=(const ILoggable&)
{
	return *this;
}

// Stream Insertion Operator
std::ostream& operator<<(std::ostream& out, const ILoggable& s) {
	out << "[ILOGGABLE]";
	return out;
}
#pragma endregion

#pragma region Subject
Subject::Subject() {
	_observers = new std::list<Observer*>;
}
Subject::Subject(const Subject& s) {
	_observers = s._observers;
}
Subject::~Subject() {
	delete _observers;
}
void Subject::attach(Observer* o) {
	_observers->push_back(o);
}
void Subject::detach(Observer* o) {
	_observers->remove(o);
}
void Subject::notify(ILoggable* il) {
	cout << "Inside Subject::notify for: " << typeid(*il).name() << endl;
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
Observer::Observer() {}
Observer::Observer(const Observer& o) {}
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
LogObserver::LogObserver() {}
LogObserver::LogObserver(const LogObserver& o) {}
LogObserver::~LogObserver() {}

void LogObserver::update(ILoggable* il) {
	cout << "Inside LogObserver::update for: " << typeid(*il).name() << endl;
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