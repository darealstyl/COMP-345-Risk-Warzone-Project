#include "CommandProcessor.h"
#include "../Utills/warzoneutils.h"
#include "../PlayerStrategies/PlayerStrategies.h"
#include <filesystem>
#include <unordered_set>

namespace fs = filesystem;
using warzoneutils::splitInput;
using std::unordered_set;
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
	return "Command's Effect:" + this->effect;
}

Command& Command::operator=(const Command& cmd)
{
	this->command = cmd.command;
	this->effect = cmd.effect;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Command& cmd)
{
	out << "Command: " << cmd.command << ", Effect: " << cmd.effect << endl;
	return out;
}

// Command Processor


const unordered_map<string, CT> CommandProcessor::commandmap = { 
	{"loadmap" , CT::LOADMAP }, 
	{"validatemap", CT::VALIDATEMAP}, 
	{"addplayer", CT::ADDPLAYER}, 
	{"gamestart", CT::GAMESTART},
	{"replay", CT::REPLAY}, 
	{"quit", CT::QUIT}, 
	{"tournament", CT::TOURNAMENT }};

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
	for (auto observer : *_observers)
		command->attach(observer);
	notify(this);
}

// Command exists. Checks in the command map if the input string matches a valid command.
bool CommandProcessor::commandexists(string str) {
	return commandmap.count(str) == 1;
}
// Returns a command type to the corresponding input. Assumes the command exists
CT CommandProcessor::getCommandType(string command) {
	vector<string> split;
	splitInput(command, split);

	string maincommand = split[0];
	auto pair = commandmap.find(maincommand);

	return pair->second;
}

bool CommandProcessor::isValidTournamentCommand(const vector<string>& split) {
	int size = split.size();
	
	// Check if there are any arguments to the command.
	if (split.size() < 2) {
		cout << "Missing arguments" << endl;
		return false;
	}

	// Check if the first flag is correct.
	if (split[1] != "-M") {
		cout << "Missing the list of map files flag (-M)." << endl;
		return false;
	}

	// Load all of the possible maps into a set to quickly check if the ones in the command exist.
	unordered_set<string> mapfiles;
	string mapdirectory =  warzoneutils::mapdirectory;
	for (const auto& file : fs::directory_iterator(mapdirectory)) {
		fs::path map(file.path());

		mapfiles.insert(map.filename().string());
	}

	int index = 2;

	int mapcount = 0;

	// Verify that all of the map files exist.
	while (index < size && split[index] != "-P") {

		// If we're about to check for a 6th map, exit as that's too many.
		if (mapcount == 5) {
			cout << "Too many map files." << endl;
			return false;
		}

		string mapfileinput = split[index];

		// Check that the current map file exists
		if (mapfiles.find(mapfileinput) == mapfiles.end()) {
			cout << mapfileinput << " does not exist in the " << mapdirectory << " directory." << endl;
			return false;
		}
		mapcount++;
		index++;
	}

	if (mapcount == 0) {
		cout << "Missing map files." << endl;
		return false;
	}

	if (index == size) {
		cout << "Missing player strategies." << endl;
		return false;
	}

	// Skip over the -P
	index++;

	const unordered_set<string>& strategystrings = PlayerStrategy::strategystrings;

	int playercount = 0;
	// Verify that all of the player strategies are valid.
	while (index < size && split[index] != "-G") {

		if (playercount == 4) {
			cout << "Too many player strategies." << endl;
			return false;
		}

		string playstring = split[index];

		if (strategystrings.find(playstring) == strategystrings.end()) {
			cout << playstring << " is not a valid player strategy." << endl;
			return false;
		}
		playercount++;
		index++;
	}

	if (playercount == 0) {
		cout << "Missing player strategies." << endl;
		return false;
	}

	if (index == size || split[index] != "-G") {
		cout << "Missing the flag for the number of games to be played on each map." << endl;
		return false;
	}

	// Skip over the -G
	index++;

	if (index == size) {
		cout << "Missing the number of games to be played on each map." << endl;
		return false;
	}

	int gamenumber = stoi(split[index]);

	if (gamenumber < 1 || gamenumber > 5) {
		cout << "You can only choose between 1 to 5 games to be played on each map." << endl;
		return false;
	}

	index++;

	if (index == size || split[index] != "-D") {
		cout << "Missing the flag for the number of turns to be played on each map." << endl;
		return false;
	}

	// Skip over the -D
	index++;

	if (index == size) {
		cout << "Missing the number of turns to be played on each map." << endl;
		return false;
	}

	int turnnumber = stoi(split[index]);

	if (turnnumber < 10 || turnnumber > 50) {
		cout << "You can only choose between 10 to 50 turns to be played on each map." << endl;
		return false;
	}
	cout << "Valid tournament command!" << endl;
	return true;
}

bool CommandProcessor::validate(Command* command) {

	// Split up the input command
	vector<string> split;
	splitInput(command->command, split);

	// If it's "", the command is invalid
	if (split.size() == 0) {
		return false;
	}

	

	string maincommand = split[0];

	if (!commandexists(maincommand)) {
		cout << "This command is not recognized, please try again." << endl;
		return false;
	}

	CT type = getCommandType(command->command);
	GameEngine::GameState gamestate = game->getState();

	bool validity = false;

	switch (type) {
	case CT::TOURNAMENT: {
		if (gamestate == GS::START) {
			validity = isValidTournamentCommand(split);
		}
		break;
	}
	case CT::LOADMAP: {
		if (gamestate == GS::START || gamestate == GS::MAP_LOADED) {

			if (split.size() < 2) {
				cout << "Missing filename." << endl;
				break;
			}

			string mapdirectory = warzoneutils::mapdirectory;
			string filename = split[1];

			// Checks if the input filename exists in the correct directory, if it does, then it is valid.
			for (const auto& file : fs::directory_iterator(mapdirectory)) {
				fs::path map(file.path());

				if (map.filename() == filename) {
					validity = true;
					break;
				}
			}

			if (!validity) {
				cout << "No such map was found in the directory \"" << mapdirectory << "\"." << endl;
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
			}
			else if (game->getPlayerCount() == 6) {
				cout << "There is already the max amount of players (6)." << endl;
			}
			else {
				validity = true;
			}
		}
		break;
	case CT::GAMESTART:
		if (gamestate == GS::PLAYERS_ADDED) {
			if (game->getPlayerCount() < 2) {
				cout << "Cannot start the game with less than 2 players." << endl;
			}
			else {
				validity = true;
			}
			
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


CommandProcessor::CommandProcessor()
{
	this->game = nullptr;
}

CommandProcessor::CommandProcessor(GameEngine* game)
{
	this->game = game;
}

CommandProcessor::~CommandProcessor() {
	cout << "Deleting saved commands of CommandProcessor" << endl;
	for (Command* command : commands)
		delete command;
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& cmdProc)
{
	this->commands = cmdProc.commands;
	this->game = cmdProc.game;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const CommandProcessor& cmdProc)
{
	out << "CommandProcessor Commands: " << endl;
	for (Command* cmd : cmdProc.commands)
		out << *cmd;
	return out;
}

// FileCommandProcessor

FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine* game, string filename) : CommandProcessor(game) {
	inputstream.open("CommandFiles/" + filename);

}
// If it's the end of the stream, forcefully stop the engine.
string FileCommandProcessorAdapter::readCommand() {
	if (inputstream.eof()) {
		this->game->running = false;
		return "";
	}
	string command;
	getline(inputstream, command);
	if (command == "") {
		this->game->running = false;
		return "";
	}
	return command;
}

std::string CommandProcessor::stringToLog() {
	return "Command: " + commands.back()->command;
}


FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {

	cout << "Closing the file input stream for FileCommandProcessorAdapter." << endl;
	inputstream.close();
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& fCmdProc)
{
	// 'ifstream' has a deleted copy constructor. Nothing to do here.	
	return *this;
}

std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& fCmdProc)
{
	out << "FileCommandProcessorAdapter Instance" << endl;
	return out;
}
