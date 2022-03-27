#pragma once

#include <ostream>
#include <list>
#include <string>

class ILoggable;
class Subject;
class Observer;
class LogObserver;

/*
	All classes subjected to observation should inherit ILoggable for logging purposes.
	stringToLog() is to be defined by inheriters to provide relevant info to the logger.
*/
class ILoggable {
public:
	virtual ~ILoggable();

	virtual std::string stringToLog() = 0;

	friend std::ostream& operator<<(std::ostream&, const ILoggable&);
};

/*
	All classes subjected to observation should inherit Subject to be able to attach observers.
	Many observers can observe a subject
*/
class Subject {
public:
	Subject();
	Subject(const Subject&);
	~Subject();

	virtual void attach(Observer*);
	virtual void detach(Observer*);
	virtual void notify(ILoggable*);

	Subject& operator=(const Subject&);
	friend std::ostream& operator<<(std::ostream&, const Subject&);
protected:
	std::list<Observer*>* _observers;
};

/*
	Observer model to be inherited by the LogObserver
*/
class Observer {
public:
	Observer();
	Observer(const Observer&);
	~Observer();
	
	virtual void update(ILoggable*) = 0;

	Observer& operator=(const Observer&);
	friend std::ostream& operator<<(std::ostream&, const Observer&);
};

/*
	LogObserver inherits Observer and implements the pure abstract update method
	update() writes all updates received from Subjects to the game log
*/
class LogObserver : public Observer {
public:
	LogObserver();
	LogObserver(const LogObserver&);
	~LogObserver();

	void update(ILoggable*);
	LogObserver& operator=(const LogObserver&);
	friend std::ostream& operator<<(std::ostream&, const LogObserver&);
};