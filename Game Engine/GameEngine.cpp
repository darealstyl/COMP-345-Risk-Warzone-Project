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
    this->state = new GS(GS::START);
    this->map = nullptr;
    this->commandprocessor = nullptr;
    this->running = true;
    this->deck = new Deck();
}

GameEngine::GameEngine(const GameEngine &game1){
    
}

GameEngine& GameEngine::operator= (const GameEngine& game1){
    return *this;
}

GameEngine::~GameEngine(){
    delete state;
    state = nullptr;
    activePlayers.clear();
}

ostream & operator << (ostream &out, const GameEngine &g){
    return out;
}

istream & operator >> (istream &in, GameEngine &g){
    return in;
}



void GameEngine::transition(GS state) {
    delete this->state;
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
            this->commandprocessor = new CommandProcessor(this);
        }
        else if (split[0] == "-file") {
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
    vector<Player*> remainingplayers;
    for (Player* player : activePlayers) {
        if (player->getNbOfTerritories() == 0) {
            cout << player << " is out of the game. LOSER!";
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
                        |____________________________|)";
        }
        else {
            remainingplayers.push_back(player);
        }
    }

    activePlayers = remainingplayers;
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
    transition(ASSIGN_REINFORCEMENT);
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
    transition(GS::ISSUE_ORDERS);
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
            transition(GS::EXECUTE_ORDERS);
        }
    }
}

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
                cout << player << "'s turn" << endl;
                Order& order = *list[orderindex];
                order.execute();

                atleastOneExecution = true;
            }
        }

        orderindex++;
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
    return *state;
}


