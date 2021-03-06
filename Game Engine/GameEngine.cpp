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
    this->tournamenthandler = nullptr;
    this->nbOfTurns = 0;
}

GameEngine::GameEngine(const GameEngine &game1){
    this->state = game1.state;
    this->map = game1.map;
    this->commandprocessor = game1.commandprocessor;
    this->running = game1.running;
    this->deck = game1.deck;
    this->tournamenthandler = game1.tournamenthandler;
}

GameEngine& GameEngine::operator= (const GameEngine& game1){
    return *this;
}

GameEngine::~GameEngine(){
    delete commandprocessor;
    delete deck;
    delete map;
    delete tournamenthandler;
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
            cout << "Chose to accept command from the console. Instanciated command processor to a new CommandProcessor()." << endl;
            this->commandprocessor = new CommandProcessor(this);
            for (auto observer : *_observers)
                this->commandprocessor->attach(observer);
        }
        else if (split[0] == "-file" && split.size() == 2) {
            string path = "CommandFiles/";
            string filename = split[1];
            auto it = fs::directory_iterator(path);
            for (const auto& file : it) {
                fs::path map(file.path());

                if (map.filename() == filename) {
                    cout << "Chose to accept command from the saved file " << filename << ". Instanciated command processor to a new FileCommandProcessorAdapter()." << endl;
                    
                    this->commandprocessor = new FileCommandProcessorAdapter(this, filename);
                    for (auto observer : *_observers)
                        this->commandprocessor->attach(observer);
                    break;
                }
            }
        }
    }
    cout << commandprocessor << endl;

}
void GameEngine::createTournament(string str) {
    vector<string> split;
    warzoneutils::splitInput(str, split);

    int size = split.size();

    int index = 2;

    vector<Map*> maps;

    while (index < size && split[index] != "-P") {
        string mapfileinput = split[index];

        // Create and validate each map files
        Map* map = MapLoader::createMap(mapfileinput);
        if (!map->validate()) {
            cout << "The map that was loaded from this file: " << mapfileinput << " is invalid. Cannot start the tournament." << endl;
            return;
        }
        maps.push_back(map);
        index++;
    }

    // Skip over the -P
    index++;

    vector<string> playerstrategies;

    while (index < size && split[index] != "-G") {

        string playstring = split[index];

        playerstrategies.push_back(playstring);
        
        index++;
    }
    // Skip over the -G
    index++;

    int gamenumber = stoi(split[index]);
    index++;

    // Skip over the -D
    index++;

    int turnnumber = stoi(split[index]);

    TournamentHandler* tournamenthandler = new TournamentHandler(maps, playerstrategies, gamenumber, turnnumber, this);
    this->tournamenthandler = tournamenthandler;
}

void GameEngine::execute(Command* command) {
    vector<string> split;
    warzoneutils::splitInput(command->command, split);
    string maincommand = split[0];

    CT commandtype = commandprocessor->getCommandType(maincommand);

    switch (commandtype) {
    case CT::TOURNAMENT: {
        createTournament(command->command);
        break;
    }
    case CT::LOADMAP: {
        string mapfile = split[1];
        this->map = MapLoader::createMap(mapfile);
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
        command->saveEffect("Successfully started the game");
        startgame();
        
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
void GameEngine::startgame() {
    gamestart();
    transition(GS::ASSIGN_REINFORCEMENT);
    cout << "Done with startup. Starting the game" << endl;
    mainGameLoop();
}

void GameEngine::clearActivePlayers() {
    for (Player* player : activePlayers) {
        delete player;
    }
    activePlayers.clear();
}

void GameEngine::resetgameengine() {
    delete map;
    map = nullptr;
    
    clearActivePlayers();
    nbOfTurns = 0;

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
        if (tournamenthandler && !tournamenthandler->canPlayTurn()) {
            break;
        }
        nbOfTurns++;
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
        if (tournamenthandler) {
            tournamenthandler->execute();
        }
        else {
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
                cout << "You can also choose to start a tournament using the command: tournament -M <listofmapfiles> -P <listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>." << endl;
                break;
            }
            case GS::MAP_LOADED:
                cout << "\nYou are in the map loaded phase." << endl;
                cout << "You can use the \"validatemap\" command to validate the map you've loaded." << endl;
                break;
            case GS::MAP_VALIDATED:
                cout << "\nYou are in the map validated phase." << endl;
                cout << "You can use the \"addplayer <playername>\" command to add a player" << endl;
                break;
            case GS::PLAYERS_ADDED:
                cout << "\nYou are in the player added phase." << endl;
                cout << "You can use the \"addplayer <playername>\" command to add another player or start the game using \"gamestart\" if you've added 2 to 6 players." << endl;
                break;
            case GS::WIN:
                cout << "\nYou are in the win phase" << endl;
                cout << "You can use the \"replay\" command to play another game or quit the program by using the \"quit\" command." << endl;
                break;
            }


            getandexecutecommand();
        }
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
        player->advanceordersnb = 0;
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

            if (list.size() > orderindex) {

                cout << endl << *player << "'s turn" << endl;
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
            cout << endl << "Player " << player->name << " conquered a territory. Giving one card..." << endl;
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
    Player* p = nullptr;

    if(name == "Aggressive")
        p = new Player(name, new AggressivePlayerStrategy());
    else if(name == "Benevolent")
        p = new Player(name, new BenevolentPlayerStrategy());
    else if(name == "Neutral")
        p = new Player(name, new NeutralPlayerStrategy());
    else if(name == "Cheater")
        p = new Player(name, new CheaterPlayerStrategy());
    else
        p = new Player(name, new HumanPlayerStrategy());

    addPlayer(p);
}

void GameEngine::addPlayer(Player* p) {
    for (auto observer : *_observers)
        p->orderList->attach(observer);
    activePlayers.push_back(p);
}

GameEngine::GameState GameEngine::getState()
{
    return state;
}

TournamentHandler::TournamentHandler(vector<Map*> maps, vector<string> playerstrategies, int nbOfGames, int maxNbOfTurns, GameEngine* gameengine) {
    this->maps = maps;
    this->playerstrategies = playerstrategies;
    this->nbOfGames = nbOfGames;
    this->maxNbOfTurns = maxNbOfTurns;
    this->gameengine = gameengine;
    this->currentMapIndex = 0;
    this->currentNbOfGames = 0;
    results.push_back(vector<string>());
}

TournamentHandler::TournamentHandler(const TournamentHandler& th)
{
    this->maps = th.maps;
    this->playerstrategies = th.playerstrategies;
    this->nbOfGames = th.currentNbOfGames;
    this->maxNbOfTurns = th.maxNbOfTurns;
    this->results = th.results;
    this->gameengine = th.gameengine;
    this->currentMapIndex = th.currentMapIndex;
    this->currentNbOfGames = th.currentNbOfGames;
}

TournamentHandler& TournamentHandler::operator=(const TournamentHandler& th)
{
    this->maps = th.maps;
    this->playerstrategies = th.playerstrategies;
    this->nbOfGames = th.currentNbOfGames;
    this->maxNbOfTurns = th.maxNbOfTurns;
    this->results = th.results;
    this->gameengine = th.gameengine;
    this->currentMapIndex = th.currentMapIndex;
    this->currentNbOfGames = th.currentNbOfGames;
    return *this;
}

TournamentHandler::~TournamentHandler()
{
    maps.clear();
    playerstrategies.clear();
    results.clear();
    // game engine deletion is handled by the main driver.
    gameengine = nullptr;
}

ostream& operator<<(ostream& out, const TournamentHandler& g)
{
    out << "[TOURNAMENTHANDLER]";
    return out;
}

bool TournamentHandler::canPlayTurn() {
    if (gameengine->nbOfTurns < this->maxNbOfTurns) {
        return true;
    }
    else {
        cout << "Max number of turns reached, this is a draw." << endl;
        results[currentMapIndex].push_back("Draw");
        gameengine->transition(GS::WIN);
        return false;
    }
        
}

void TournamentHandler::execute() {
    switch (gameengine->state) {
    case GS::START: {
        loadmap(currentMapIndex);
        addplayers();
        gameengine->startgame();
        break;
    }
    case GS::WIN: {
        if (gameengine->activePlayers.size() == 1) {
            results.back().push_back(gameengine->activePlayers.front()->name);
        }
        currentNbOfGames++;
        if (currentNbOfGames == nbOfGames) {
            currentMapIndex++;
            currentNbOfGames = 0;
            

            if (currentMapIndex == maps.size()) {
                cout << "Tournament is over, printing the results." << endl;
                printresults();
                gameengine->tournamenthandler = nullptr;
                delete this;
                return;
            }

            results.push_back(vector<string>());
        }
        gameengine->resetgameengine();
        loadmap(currentMapIndex);
        addplayers();
        gameengine->startgame();
        break;
    }
    default:
        cout << "Impossible state in tournament." << endl;
        break;
    }
}

void TournamentHandler::loadmap(int index) {
    gameengine->map = new Map(*maps[index]);
}

void TournamentHandler::addplayers() {
    for (string playername : playerstrategies) {
        Player* player = new Player(playername);
        player->setPlayerStrategy(playername);
        gameengine->addPlayer(player);
    }
}

void TournamentHandler::printresults()
{
    int width = 10;
    printverticalborder(width);
    cout << "|";
    printspaceandborder(width);
    for (int i = 1; i <= nbOfGames; i++) {
        string str = "Game ";
        cout << str << i;
        printspaceandborder(width - str.size() - 1);
    }
    printverticalborder(width);
    int mapcount = 1;
    for (vector<string> mapwinners : results) {
        string str = "|Map ";
        cout << str << mapcount;
        printspaceandborder(width - str.size());
        int winnercount = 1;
        for (string gamewinner : mapwinners) {
            cout << gamewinner;
            printspaceandborder(width - gamewinner.size());
            winnercount++;
        }
        mapcount++;
        printverticalborder(width);
    }
}
void TournamentHandler::printspaceandborder(int width) {
    for (int i = 0; i < width; i++) {
        cout << " ";
    }
    cout << "|";
}

void TournamentHandler::printverticalborder(int width) {
    cout << endl << "|";
    for (int i = 0; i <= nbOfGames; i++) {
        for (int j = 0; j < width; j++) {
            cout << "-";
        }
        cout << "|";
    }
    cout << endl;
}