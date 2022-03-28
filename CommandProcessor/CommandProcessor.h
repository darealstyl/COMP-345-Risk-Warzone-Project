#pragma once
#include <list>
#include <string>
#include <fstream>
#include "../Game Engine/GameEngine.h"
using namespace std;

class GameEngine;

class Command : public Subject, public ILoggable {
public:
	string command;
	string effect;

	Command(string);

	void saveEffect(string);
	string stringToLog() override;

	Command& operator=(const Command&);
	friend std::ostream& operator<<(std::ostream&, const Command&);

};
class CommandProcessor : public Subject, public ILoggable {
protected:
	virtual string readCommand();
	void saveCommand(Command*);
public:
	enum class CommandType {
		LOADMAP = 0,
		VALIDATEMAP,
		ADDPLAYER,
		GAMESTART,
		REPLAY,
		QUIT
	};
	list<Command*> commands;
	Command* getCommand();

	CommandProcessor();
	CommandProcessor(GameEngine*);
	virtual ~CommandProcessor();

	string stringToLog() override;

	bool validate(Command*);
	CommandType getCommandType(string);

	CommandProcessor& operator=(const CommandProcessor&);
	friend std::ostream& operator<<(std::ostream&, const CommandProcessor&);
protected:
	GameEngine* game;
private:
	static const unordered_map<string, CommandType> commandmap;
	bool commandexists(string);
	

};
class FileCommandProcessorAdapter : public CommandProcessor {
public:
	FileCommandProcessorAdapter(GameEngine*, string);
	~FileCommandProcessorAdapter();

	FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter&);
	friend std::ostream& operator<<(std::ostream&, const FileCommandProcessorAdapter&);
	
protected:
	ifstream inputstream;
	string readCommand();
};