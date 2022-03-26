#include <iostream>
#include <filesystem>
#include <random>
#include "GameEngine.h"
#include "../Map/Map.h"
#include "../Utills/warzoneutils.h"

namespace fs = filesystem;
using namespace std;
typedef GameEngine::GameState GS;
typedef CommandProcessor::CommandType CT;

GameEngine::GameEngine() {
    state = new GameState(START);
    map = nullptr;
    commandprocessor = nullptr;
    running = true;
}

GameEngine::GameEngine(const GameEngine &game1){
    
}

GameEngine& GameEngine::operator= (const GameEngine& game1){
    return *this;
}

GameEngine::~GameEngine(){
    delete state;
    activePlayers.clear();
}

ostream & operator << (ostream &out, const GameEngine &g){
    return out;
}

istream & operator >> (istream &in, GameEngine &g){
    return in;
}



void GameEngine::transition(GS state) {
    this->state = new GS(state);
    notify(this);
}

void GameEngine::initializeCommandProcessor() {
    CommandProcessor startprocessor;

    while (commandprocessor == nullptr) {
        cout << "Please select how you want the application to accept commands; from the console (-console) or from a file (-file <filename>)." << endl;
        Command* command = startprocessor.getCommand();
        vector<string> split;
        warzoneutils::splitInput(command->command, split);

        if (split[0] == "-console") {
            this->commandprocessor = new CommandProcessor();
        }
        else if (split[0] == "-file") {
            string path = "CommandFiles/";
            string filename = split[1];
            for (const auto& file : fs::directory_iterator(path)) {
                fs::path map(file.path());

                if (map.filename() == filename) {
                    this->commandprocessor = new FileCommandProcessorAdapter(filename);
                    break;
                }
            }
        }
    }
}

void GameEngine::execute(Command* command) {
    vector<string> split;
    warzoneutils::splitInput(command->command, split);
    string maincommand = split[0];

    CT commandtype = commandprocessor->getCommandType(maincommand);

    switch (commandtype) {
    case CT::LOADMAP: {
        string mapfile = split[1];
        this->map = &MapLoader::createMap(mapfile);
        transition(GS::MAP_LOADED);
        command->saveEffect("Successfully loaded map \"" + mapfile + "\"");
        break;
    }
    case CT::VALIDATEMAP: {
        bool validmap = this->map->validate();
        if (validmap) {
            transition(GS::MAP_VALIDATED);
            cout << "The loaded map is valid, now entering MAP_VALIDATED state." << endl;
            command->saveEffect("Validated the map that is currently loaded successfully.");
        }
        else {
            cout << "Invalid Map" << endl;
            command->saveEffect("Found the map invalid.");
        }
        break;
    }
    case CT::ADDPLAYER: {
        string playername = split[1];
        addPlayer(playername);
        transition(GS::PLAYERS_ADDED);
        cout << "Successfully added player \"" + playername + "\"" << endl;
        command->saveEffect("Successfully added player \"" + playername + "\"");
        break;
    }
    case CT::GAMESTART:
        gamestart();
        transition(GS::ASSIGN_REINFORCEMENT);
        cout << "Done with startup. Starting the game" << endl;
        command->saveEffect("Successfully started the game");
        mainGameLoop();
        break;
    case CT::REPLAY:
        transition(GS::START);
        cout << "Restarting the game" << endl;
        command->saveEffect("Successfully restarted a new game");
    case CT::QUIT:
        running = false;
        cout << "Quitting the game" << endl;
        command->saveEffect("Successfully ended the application");
        break;
    }
}

void GameEngine::getandexecutecommand() {
    Command* command = commandprocessor->getCommand();
    bool valid = commandprocessor->validate(command);
    if (valid) {
        execute(command);
    }
}

void GameEngine::gamestart() {
    distributeterritories();
    randomizeplayerorder();
    distributearmies();
    distributecards();
}

void GameEngine::distributeterritories() {
    vector<Territory*> territories(map->territories);
    while (!territories.empty()) {
        for (Player* player : activePlayers) {
            if (territories.empty()) {
                break;
            }
            else {
                player->addTerritory(territories.back());
                territories.pop_back();
            }
        }
    }
}

void GameEngine::randomizeplayerorder() {
    auto rng = std::default_random_engine{};
    shuffle(begin(activePlayers), end(activePlayers), rng);
}

void GameEngine::distributearmies() {
    for (Player* player : activePlayers) {
        player->addReinforcements(50);
    }
}

void GameEngine::distributecards() {
    for (Player* player : activePlayers) {
        player->hand->addCard(deck->draw());
    }
}

void GameEngine::checkforwin() {
    int currentNbOfPlayers = this->activePlayers.size();

    // If there isn't one player left, there is no winner yet.
    if (currentNbOfPlayers != 1) {
        return;
    }

    Player* lastplayer = this->activePlayers.front();
    
    // If the last player does not have all the territories, the game isn't won yet.
    if (lastplayer->territories.size() != map->territories.size()) {
        return;
    }

    // The last player has all the territories and is the winner.
    cout << lastplayer << " has won!" << endl;

    transition(GS::WIN);

}

void GameEngine::removelosers() {

}

void GameEngine::mainGameLoop() {
    while (GS::WIN != *state) {
        switch (*state) {
        case ASSIGN_REINFORCEMENT:
            cout << "\nYou are in the assignment reinforcement phase" << endl;
            reinforcementPhase();
            break;
        case ISSUE_ORDERS:
            cout << "\nYou are in the issue order phase" << endl;
            issueOrdersPhase();
            break;
        case EXECUTE_ORDERS:
            cout << "\nYou are in the execute order phase" << endl;
            executeOrdersPhase();
            break;
        }

        removelosers();
        checkforwin();
    }
}

void GameEngine::startupPhase() {

    initializeCommandProcessor();

    while (running) {
        switch (*state)
        {
        case START: {
            cout << "Starting the startup phase" << endl;
            cout << "Please choose one of the following maps that are available in your MapFiles directory using the \"loadmap <mapfile>\" command.\n" << endl;
            string path = "MapFiles/";
            for (const auto& file : fs::directory_iterator(path)) {
                fs::path map(file.path());
                cout << "\t" << map.filename() << endl;
            }
            break;
        }
        case MAP_LOADED:
            cout << "\nYou are in the map loaded phase." << endl;
            break;
        case MAP_VALIDATED:
            cout << "\nYou are in the map validated phase." << endl;
            break;
        case PLAYERS_ADDED:
            cout << "\nYou are in the player added phase." << endl;
            break;
        case WIN:
            cout << "\nYou are in the win phase" << endl;
            break;
        }


        getandexecutecommand();
    }
}

std::string GameEngine::stateToString() {
    switch (*state) {
        case GameState::START:                  return "START";
        case GameState::MAP_LOADED:             return "MAP_LOADED";
        case GameState::MAP_VALIDATED:          return "MAP_VALIDATED";
        case GameState::PLAYERS_ADDED:          return "PLAYERS_ADDED";
        case GameState::ASSIGN_REINFORCEMENT:   return "ASSIGN_REINFORCEMENT";
        case GameState::ISSUE_ORDERS:           return "ISSUE_ORDERS";
        case GameState::EXECUTE_ORDERS:         return "EXECUTE_ORDERS";
        case GameState::WIN:                    return "WIN";    
        default:                                    return "Error reading state.";
    }
}

std::string GameEngine::stringToLog() {
    return "GameEngine: Current state: " + stateToString();
}

void GameEngine::reinforcementPhase() {
    *state = ASSIGN_REINFORCEMENT;
    for (Player* p : activePlayers) {
        int reinforcements = floor(p->territories.size() / 3);
        if (reinforcements < 3)
            reinforcements = 3;
        p->addReinforcements(reinforcements);
        cout << *(p->reinforcements) << " reinforcements has been added" << endl;
        int count = 0; //to check if players own the territories
        // TODO: if the player owns all the territories of a continent, player is given continents control bonus.
        for (Continent* c : map->continents) {  //get the list of continent
            for (Territory* t : c->territories) {  // get the list of territories in a continent
                for (Territory* l : p->territories) {  // get the list of the player's territories
                    if (l == t ) {  //if the territories are the same, increment the count
                        count++;
                    }
                }
            }
            if (count == c->territories.size()) {  //if the count is the same as the size of the list in continent, give the bonus
                cout << "A bonus is given" << endl;
                int bonus = 5; //temporary
                p->addReinforcements(bonus);
                cout << *(p->reinforcements) << " total soldiers" << endl;
                count = 0; //initialize to 0 for the next continent
            }
        }
    }    
}

void GameEngine::issueOrdersPhase() {
    // Players issue orders and place them in their order list through Player::issueOrder()
    // This method is called round robin by game engine
    *state = ISSUE_ORDERS;
    string order;
    for (Player* p : activePlayers) {
        cout << p->name + "'s turn" << endl;
        p->issueOrder();
       
        }
    for (Player* p : activePlayers) {  //checks if all players are finished 
        if (p->endOfOrder == false) {
         break;
        }
        else {
            cout << "Players are done issuing orders" << endl;
         //   *state = EXECUTE_ORDERS;
        }
    }
}

void GameEngine::executeOrdersPhase() {
    // Players are done issuing orders.
    // Proceed to execute the top order on the list of orders of each player in a round-robin fashion
    // see Order Execution Phase
    // Once all player orders have been executed, the main game loop returns to reinforcement phase.
    cout << "inside executeOrderPhase" << endl;
    for (Player* p : activePlayers) {
        cout << p->name + "'s turn" << endl;
        p->orderList->list.at(0)->execute(); 
        p->orderList->list.erase(p->orderList->list.begin()); //pop the order from the list when done 
    }

    //check if a player doesn't have any territories
 
    for (auto it = activePlayers.begin(); it != activePlayers.end(); it++) {
        if ((*it)->territories.size() == 0) {
            cout << (*it)->name << " does not own any territories...he will be executed";
            cout << R"( ____________
  |____________|_
   ||--------|| | _________
   ||- _     || |(HA ha ha!)
   ||    - _ || | ---------
   ||       -|| |     //
   ||        || O\    __
   ||        ||  \\  (..)
   ||        ||   \\_|  |_
   ||        ||    \  \/  )
   ||        ||     :    :|
   ||        ||     :    :|
   ||        ||     :====:O
   ||        ||     (    )
   ||__@@@@__||     | `' |
   || @|..|@ ||     | || |
   ||O@`=='@O||     | || |
   ||_@\/\/@_||     |_||_|
 ----------------   '_'`_`
/________________\----------\
|   GUILLOTINE   |-----------|
|  OF CASTLE DE SADE         |
|____________________________|)"; //source: https://www.asciiart.eu/weapons/guillotines
            activePlayers.erase(it--); //erasing the object first then decrementing the iterator so the next player will not be an invalid iterator
           
        }
    }

    // check if all the order list are empty, if so, go back to reinforcement
    int list_count = 0;
    for (Player* p : activePlayers) {  
        if (p->orderList->list.size() == 0) {
            list_count++;
        }
        if (list_count == activePlayers.size()) {
            cout << "All players have executed their orders" << endl;
            *state = ASSIGN_REINFORCEMENT;
        }
    }

}
int GameEngine::getPlayerCount() {
    return activePlayers.size();
}
void GameEngine::addPlayer(string name) {
    Player* p = new Player(name);
    activePlayers.push_back(p);
}

void GameEngine::addPlayer(Player* p) {
    activePlayers.push_back(p);
}

GameEngine::GameState GameEngine::getState()
{
    return *state;
}


