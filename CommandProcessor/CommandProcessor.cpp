#include "CommandProcessor.h"
#include "../Utills/warzoneutils.h"
#include <filesystem>

namespace fs = filesystem;
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

bool CommandProcessor::validate(Command* command) {

	vector<string> split;
	splitInput(command->command, split);

	string maincommand = split[0];
	auto pair = commandmap.find(maincommand);

	if (pair == commandmap.end()) {
		cout << "This command is not recognized, please try again." << endl;
		return false;
	}

	if (split.size() > 2) {
		cout << "This command contains too many arguments, please try again." << endl;
		return false;
	}

	CT type = pair->second;
	GameEngine::GameState gamestate = game->getState();

	bool validity = false;

	switch (type) {
	case CT::LOADMAP: {
		if (gamestate == GS::START || gamestate == GS::MAP_LOADED) {

			if (split.size() < 2) {
				cout << "Missing filename." << endl;
				break;
			}

			string path = "MapFiles/";
			string filename = split[1];
			for (const auto& file : fs::directory_iterator(path)) {
				fs::path map(file.path());

				if (map.filename() == filename) {
					validity = true;
					break;
				}
			}

			if (!validity) {
				cout << "No such map was found in the directory \"" << path << "\"." << endl;
			}
		}
		break;
	}
	case CT::VALIDATEMAP:
		if (gamestate == GS::MAP_LOADED) {
			validity = true;
		}
		break;
	case CT::ADDPLAYER:
		if (gamestate == GS::MAP_VALIDATED || gamestate == GS::PLAYERS_ADDED) {

			if (split.size() < 2) {
				cout << "Missing playername." << endl;
				break;
			}

			validity = true;
		}
		break;
	case CT::GAMESTART:
		if (gamestate == GS::PLAYERS_ADDED) {
			validity = true;
		}
		break;
	case CT::REPLAY:
	case CT::QUIT:
		if (gamestate == GS::WIN) {
			validity = true;
		}
		break;
	}

	if (validity) {
		cout << "Valid command" << endl;
	}
	else {
		cout << "Invalid command" << endl;
		command->saveEffect("Invalid command");
	}

	return validity;

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

std::string CommandProcessor::stringToLog() {
	return "Logger::CommandProcessor: Command: " + commands.back()->command;
}