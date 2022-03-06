#include "CommandProcessor.h"

// Command
Command::Command(string str) {
	this->command = str;
}

void Command::saveEffect() {
	this->effect = "Effect";
}

// Command Processor
string CommandProcessor::readCommand() {
	string command;
	getline(cin, command);
	return command;
}

Command* CommandProcessor::getCommand() {
	string str = readCommand();
	Command* command = new Command(str);
	saveCommand(command);
}

void CommandProcessor::saveCommand(Command* command) {
	commands.push_back(command);
}

void CommandProcessor::validate(Command* command) {

}

CommandProcessor::~CommandProcessor() {
	for (Command* command : commands)
		delete command;
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

