#pragma once
#include <list>
#include <string>
#include <fstream>
#include "../Game Engine/GameEngine.h"
using namespace std;
class Command {
public:
	string command;
	string effect;

	Command(string);

	void saveEffect();
};
class CommandProcessor {
private:
	GameEngine* game;
protected:
	virtual string readCommand();
	void saveCommand(Command*);
public:
	list<Command*> commands;
	Command* getCommand();

	~CommandProcessor();

	void validate(Command*);

};
class FileCommandProcessorAdapter : public CommandProcessor {
public:
	FileCommandProcessorAdapter(string);
protected:
	ifstream inputstream;
	string readCommand();
};