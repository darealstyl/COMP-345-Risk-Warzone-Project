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
    this->state = GS::START;
    this->map = nullptr;
    this->commandprocessor = nullptr;
    this->running = true;
    this->deck = new Deck();
    FORCEWIN = false;
}

GameEngine::GameEngine(const GameEngine &game1){
    
}

GameEngine& GameEngine::operator= (const GameEngine& game1){
    return *this;
}

GameEngine::~GameEngine(){
    delete deck;
}

ostream & operator << (ostream &out, const GameEngine &g){
    return out;
}

istream & operator >> (istream &in, GameEngine &g){
    return in;
}



void GameEngine::transition(GS state) {
    this->state = state;
    notify(this);
}

void GameEngine::initializeCommandProcessor() {
    CommandProcessor startprocessor;

    while (commandprocessor == nullptr) {
        cout << "Please select how you want the application to accept commands; from the console (-console) or from a file (-file <filename>)." << endl;
        Command* command = startprocessor.getCommand();
        vector<string> split;
        warzoneutils::splitInput(command->command, split);
        if (split.size() == 0) {
            continue;
        }

        if (split[0] == "-console" && split.size() == 1) {
            this->commandprocessor = new CommandProcessor(this);
        }
        else if (split[0] == "-file" && split.size() == 2) {
            string path = "CommandFiles/";
            string filename = split[1];
            auto it = fs::directory_iterator(path);
            for (const auto& file : it) {
                fs::path map(file.path());

                if (map.filename() == filename) {
                    this->commandprocessor = new FileCommandProcessorAdapter(this, filename);
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
        resetgameengine();
        cout << "Restarting the game" << endl;
        command->saveEffect("Successfully restarted a new game");
        break;
    case CT::QUIT:
        running = false;
        cout << "Quitting the game" << endl;
        command->saveEffect("Successfully ended the application");
        break;
    }
}

void GameEngine::resetgameengine() {
    map = nullptr;
    
    for (Player* player : activePlayers) {
        delete player;
    }
    activePlayers.clear();

    delete deck;
    deck = new Deck();
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
    if (FORCEWIN) {
        Player* winner = activePlayers.front();
        for (Territory* t : map->territories) {
            winner->addTerritory(t);
        }
    }
    else {
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

void GameEngine::makePlayerDrawCard(Player* player) {
    player->hand->addCard(deck->draw());
}


void GameEngine::distributecards() {
    for (Player* player : activePlayers) {

        cout << "Before distributing cards to " << *player << ": " << endl;
        cout << *(player->hand) << endl;

        makePlayerDrawCard(player);
        makePlayerDrawCard(player);

        cout << "After:" << endl;
        cout << *(player->hand) << endl;
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
    cout << *lastplayer << " has won!" << endl;

    transition(GS::WIN);
}

//Remove players that possess no territory

void GameEngine::removelosers() {
    vector<Player*> remainingplayers;
    for (Player* player : activePlayers) { //loop over all remaining player and check if their nb of territories is 0
        if (player->getNbOfTerritories() == 0) {
            cout << *player << " is out of the game." << endl;
            cout << R"( 
                           ____________
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
                        |____________________________|)" << endl;
        }
        else {
            remainingplayers.push_back(player);
        }
    }

    activePlayers = remainingplayers;
}

//Sequence of the main game loop using enum state
void GameEngine::mainGameLoop() {
    while (GS::WIN != state) {  //loop until a player has won which moves to state to the win 
        switch (state) {
        case GS::ASSIGN_REINFORCEMENT:
            cout << "\nYou are in the assignment reinforcement phase" << endl;
            reinforcementPhase();
            break;
        case GS::ISSUE_ORDERS:
            cout << "\nYou are in the issue order phase" << endl;
            issueOrdersPhase();
            break;
        case GS::EXECUTE_ORDERS:
            cout << "\nYou are in the execute order phase" << endl;
            executeOrdersPhase();
            break;
        }
        //after the three phases, check for players with no territories and winner
        removelosers();
        checkforwin();
    }
}

void GameEngine::startupPhase() {

    initializeCommandProcessor();

    while (running) {
        switch (state)
        {
        case GS::START: {
            cout << "Starting the startup phase" << endl;
            cout << "Please choose one of the following maps that are available in your MapFiles directory using the \"loadmap <mapfile>\" command.\n" << endl;
            string path = "MapFiles/";
            for (const auto& file : fs::directory_iterator(path)) {
                fs::path map(file.path());
                cout << "\t" << map.filename() << endl;
            }
            break;
        }
        case GS::MAP_LOADED:
            cout << "\nYou are in the map loaded phase." << endl;
            break;
        case GS::MAP_VALIDATED:
            cout << "\nYou are in the map validated phase." << endl;
            break;
        case GS::PLAYERS_ADDED:
            cout << "\nYou are in the player added phase." << endl;
            break;
        case GS::WIN:
            cout << "\nYou are in the win phase" << endl;
            break;
        }


        getandexecutecommand();
    }
}

std::string GameEngine::stateToString() {
    switch (state) {
        case GameState::START:                  return "START";
        case GameState::MAP_LOADED:             return "MAP_LOADED"; 
        case GameState::MAP_VALIDATED:          return "MAP_VALIDATED";
        case GameState::PLAYERS_ADDED:          return "PLAYERS_ADDED";
        case GameState::ASSIGN_REINFORCEMENT:   return "ASSIGN_REINFORCEMENT";
        case GameState::ISSUE_ORDERS:           return "ISSUE_ORDERS";
        case GameState::EXECUTE_ORDERS:         return "EXECUTE_ORDERS";
        case GameState::WIN:                    return "WIN";    
        default:                                return "Error reading state.";
    }
}

std::string GameEngine::stringToLog() {
    return "GameEngine New State: " + stateToString();
}

void GameEngine::getcontrolledcontinents(unordered_map<Player*, vector<Continent*>*>& controlledcontinents) {
    for (Continent* continent : map->continents) {
        // Take the owner of the first continent, and if it's not the same owner for the entire continent, move on.
        Player* owner = continent->territories[0]->owner;
        bool ownsContinent = true;

        for (Territory* territory : continent->territories) {
            if (owner != territory->owner) {
                ownsContinent = false;
                break;
            }
        }

        if (ownsContinent) {
            auto pair = controlledcontinents.find(owner);
            // If the player is not alread in the map, add the key-value pair with a new vector
            if (pair == controlledcontinents.end()) {
                vector<Continent*>* continentlist = new vector<Continent*>();
                continentlist->push_back(continent);
                controlledcontinents.insert(make_pair(owner, continentlist));
            }
            else {
                vector<Continent*>& controlledContinentsByOwner = *(pair->second);
                controlledContinentsByOwner.push_back(continent);
            }
        }
    }
}

void GameEngine::cleanupcontrolledcontinents(unordered_map<Player*, vector<Continent*>*>& controlledcontinents) {
    for (auto& pair : controlledcontinents) {
        delete pair.second;
    }
}

void GameEngine::reinforcementPhase() {
    unordered_map<Player*, vector<Continent*>*> controlledcontinents;
    getcontrolledcontinents(controlledcontinents);

    for (Player* player : activePlayers) {
        int reinforcements = player->territories.size() / 3;

        auto pair = controlledcontinents.find(player);

        if (pair != controlledcontinents.end()) {
            vector<Continent*>& controlledcontinentbyplayer = *(pair->second);
            for (Continent* controlledcontinent : controlledcontinentbyplayer) {
                reinforcements += controlledcontinent->armyValue;
            }
        }

        if (reinforcements < 3) {
            reinforcements = 3;
        }

        player->addReinforcements(reinforcements);
    }

    cleanupcontrolledcontinents(controlledcontinents);
    transition(GS::ISSUE_ORDERS);
}

void GameEngine::issueOrdersPhase() {
    for (Player* player : activePlayers) {
        player->endOfOrder = false;
    }
    // Players issue orders and place them in their order list through Player::issueOrder()
    // This method is called round robin by game engine
    bool atleastOneExecution = true;
    while (atleastOneExecution) {
        atleastOneExecution = false;
        for (Player* player : activePlayers) {
            cout << player->name + "'s turn" << endl;
            if (!player->endOfOrder) {
                player->issueOrder(this->deck);
                atleastOneExecution = true;
            }
        }
    }
    cout << "Players are done issuing orders" << endl;
    transition(GS::EXECUTE_ORDERS);
        
}
// TODO : Execute all deploy orders before any other type of orders
void GameEngine::executeOrdersPhase() {
    // Players are done issuing orders.
    // Proceed to execute the top order on the list of orders of each player in a round-robin fashion
    // see Order Execution Phase
    // Once all player orders have been executed, the main game loop returns to reinforcement phase.
    
    bool atleastOneExecution = true;
    int orderindex = 0;

    while (atleastOneExecution) {
        atleastOneExecution = false;
        for (Player* player : activePlayers) {
            
            vector<Order*>& list = player->orderList->list;

            if (list.size() < orderindex) {

                cout << endl << player << "'s turn" << endl;
                Order& order = *list[orderindex];
                order.execute();

                atleastOneExecution = true;
            }
        }

        orderindex++;
    }

    // Cleaning up orders and drawing cards for players that conquered at least one territory. Also clearing the friendly players through negotiation.
    for (Player* player : activePlayers) {

        player->clearFriendlyPlayers();

        if (player->conquered) {
            player->conquered = false;
            makePlayerDrawCard(player);
        }

        vector<Order*>& list = player->orderList->list;
        
        for (Order* order : list) {
            delete order;
        }

        list.clear();
    }

    cout << "All players have executed their orders" << endl;
    transition(GS::ASSIGN_REINFORCEMENT);
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
    return state;
}


