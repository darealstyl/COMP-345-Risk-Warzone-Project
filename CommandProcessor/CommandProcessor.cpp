#include "CommandProcessor.h"
#include "../Utills/warzoneutils.h"
using warzoneutils::splitInput;
// Command
Command::Command(string str) {
	this->command = str;
}

void Command::saveEffect(string str) {
	this->effect = str;
}

// Command Processor
const unordered_map<string, CommandType> CommandProcessor::commandmap = { {"loadmap" , LOADMAP }, {"validatemap", VALIDATEMAP}, {"addplayer", ADDPLAYER}, {"gamestart", GAMESTART},
	{"replay", REPLAY}, {"quit", QUIT} };

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

	vector<string> split;
	splitInput(command->command, split);

	string maincommand = split[0];
	auto pair = commandmap.find(maincommand);

	if (pair == commandmap.end()) {
		cout << "This command is not recognized, please try again." << endl;
		return;
	}

	CommandType type = pair->second;
	GameEngine::GameState gamestate = game->getState();

	switch (type) {
	case LOADMAP:
		if (gamestate == GameEngine::GameState::START || gamestate == GameEngine::GameState::MAP_LOADED) {
			cout << "Valid command" << endl;
			command->saveEffect("Successfully loaded map \"" + split[1] + "\"");
		}
		else {
			cout << "Invalid command" << endl;
			command->saveEffect("Successfully loaded map \"" + split[1] + "\"");
		}
	
		break;
	case VALIDATEMAP:
		break;
	case ADDPLAYER:
		break;
	case GAMESTART:
		break;
	case REPLAY:
		break;
	case QUIT:
		break;
	}

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

