#include "CommandProcessor.h"
#include "../Utills/warzoneutils.h"
using warzoneutils::splitInput;
typedef CommandProcessor::CommandType CT;
typedef GameEngine::GameState GS;
// Command
Command::Command(string str) {
	this->command = str;
}

void Command::saveEffect(string str) {
	this->effect = str;
	notify(this);
}

std::string Command::stringToLog() {
	return "Logger::Command: Effect:" + this->effect;
}

// Command Processor
const unordered_map<string, CT> CommandProcessor::commandmap = { {"loadmap" , CT::LOADMAP }, {"validatemap", CT::VALIDATEMAP}, {"addplayer", CT::ADDPLAYER}, {"gamestart", CT::GAMESTART},
	{"replay", CT::REPLAY}, {"quit", CT::QUIT} };

string CommandProcessor::readCommand() {
	string command;
	getline(cin, command);
	return command;
}

Command* CommandProcessor::getCommand() {
	string str = readCommand();
	Command* command = new Command(str);
	saveCommand(command);
	return command;
}

void CommandProcessor::saveCommand(Command* command) {
	commands.push_back(command);
	notify(this);
}


CommandProcessor::~CommandProcessor() {
	for (Command* command : commands)
		delete command;
}

std::string CommandProcessor::stringToLog() {
	return "Logger::CommandProcessor: Command: " + commands.back()->command;
}

// FileCommandProcessor

FileCommandProcessorAdapter::FileCommandProcessorAdapter(string filename) {
	inputstream.open(filename);
}

string FileCommandProcessorAdapter::readCommand() {
	string command;
	getline(inputstream, command);
	return command;
}

