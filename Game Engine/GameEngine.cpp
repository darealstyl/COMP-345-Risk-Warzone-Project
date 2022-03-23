#include <iostream>
#include "GameEngine.h"
using namespace std;

GameEngine::GameEngine() {
    state = new GameState(START);
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

// Method to start the GameEngine 
void GameEngine::start(){
   
    bool gameRunning = true; //boolean to end the loop when the game ends
    cout << "Welcome to Warzone. " << endl;

    //possible commands for the users
    string choice = "\n1. Load the map\n2.Validate the map\n3.Add a player in the game\n4.Assign a country\n5.Issue an order\n6End the issues of order\n7.Execute an order\n8.End the execution of orders\n9.Win the Game\n10.Play again\n11.End the game\n";
    while (gameRunning)
    {
        // switch statement where each case represent a state where it will execute the code and ask a command from the user which determine if it stays in the same state or transition to another
        switch (*state)
        {
        case START:
            cout << "Starting the startup phase" << endl;
            cout << "Please choose from the option below.\n"
                 << endl;
            
            cout << choice;
            int option;
            cout << "Enter your choice: ";
            cin >> option;

            switch (option)
            {
            case 1:
                // go to the MapLoaded state
                *state = MAP_LOADED;
                notify(this);
                break;

            default:
                cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;

        case MAP_LOADED:
            cout << "\nYou are in the map loaded phase\n"
                 << endl;
            // load the map
            //  game->mapLoadedPhase();
            cout << "Map has been loaded" << endl;
            // ask for the options
            cout << choice;
            cout << "Enter your choice: ";
            cin >> option;
            switch (option)
            {
            case 1:
                // load another map
                //    game->mapLoadedPhase();
                cout << "Map has been loaded" << endl;
                break;
            case 2:
                // go to validate the map
                *state = MAP_VALIDATED;
                notify(this);
                break;
            default:
                cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case MAP_VALIDATED:
            cout << "\nYou are in the map validated phase\n"
                 << endl;
            //    game->mapValidatedPhase();
            cout << "The map has been validated" << endl;
            cout << choice;
            cout << "Enter an option: ";
            cin >> option;
            switch (option)
            {
            case 3:
            //go to add player
                *state = PLAYERS_ADDED;
                notify(this);
                break;

            default:
             cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case PLAYERS_ADDED:
            cout << "\nYou are in the player added phase\n"
                 << endl;
            //    game->playersAddedPhase();
            cout << choice;
            cout << "Enter your choice: ";
            cin >> option;
            switch (option)
            {
            case 3:
            //loop in same state to add another player
                //        game->playersAddedPhase();
                break;
            case 4:
            //go to next state
                *state = ASSIGN_REINFORCEMENT;
                notify(this);
                break;
            default:
            cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case ASSIGN_REINFORCEMENT:
            cout << "\nYou are in the assignment reinforcement phase\n"
                 << endl;
            // game->assignReinforcementPhase();
            cout << "\nThe countries have been assigned to the players\n";
            cout << choice;
            cin >> option;
            switch (option)
            {
            case 5: //go to the next state
                *state = ISSUE_ORDERS;
                notify(this);
                break;
            default:
            cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case ISSUE_ORDERS:
            cout << "\nYou are in the issue order phase\n"
                 << endl;
            // game->issueOrdersPhase();
            cout << "\nAn order has been issued\n";
            cout << choice;
            cout << "Enter a choice: ";
            cin >> option;

            switch (option)
            {
            case 5:  //loop in the same state, issue another order
                // game->issueOrdersPhase();
                break;
            case 6:  //move to the next state
                *state = EXECUTE_ORDERS;
                notify(this);
                break;
            default:
            cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case EXECUTE_ORDERS:
            cout << "\nYou are in the execute order phase\n";
            cout << choice;
            cout << "Enter your choice: ";
            cin >> option;
            switch (option)
            {
            case 7:  //loop in the same state
                // game->executeOrdersPhase();
                cout << "\nAn order has been executed\n";
                break;
            case 8:  //move back to previous state
                cout << "Ending execution orders";
                *state = ASSIGN_REINFORCEMENT;
                notify(this);
                break;
            case 9:  //move to next state
                *state = WIN;
                notify(this);
                break;
            default:
            cout << "\nThis is not a valid command\n" << endl;
                break;
            }
            break;
        case WIN:
            cout << "\nYou are in the win phase\n";
            cout << "Congratulation you have won";
            cout << choice;
            cout << "Enter your choice: ";
            cin >> option;
            switch (option)
            {
            case 10:  //move back to start state
                cout << "Restarting a new game...";
                *state = START;
                notify(this);
                break;
            case 11: //move out of the loop and end the game
                cout << "End of the game...";
                gameRunning = false;
                break;
            }
            break;
        default:
            cout << "This is not a valid state" << endl;
                break;
        }
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
        int count = 0; //to check if players own the territories
        // TODO: if the player owns all the territories of a continent, player is given continents control bonus.
        for (Continent* c : theMap->continents) {  //get the list of continent
            for (Territory* t : c->territories) {  // get the list of territories in a continent
                for (Territory* l : p->territories) {  // get the list of the player's territories
                    if (l == t ) {  //if the territories are the same, increment the count
                        count++;
                    }
                }
            }
            if (count == c->territories.size()) {  //if the count is the same as the size of the list in continent, give the bonus
                int bonus = 5; //temporary
                p->addReinforcements(bonus);
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
    for (Player* p : activePlayers) {
        int length = p->orderList->list.size() - 1; //execute the order at the back of the list
        p->orderList->list.at(length)->execute(); 
        p->orderList->list.pop_back(); //pop the order from the list when done 
    }
    
    // check if a player has won 
    int win_count = 0;
    for (Player* p : activePlayers) {
        for (Territory* mapT : theMap->territories) {
            for (Territory* l : p->territories) {
                if (l == mapT) {
                    win_count++;
                }
            }
        }
        if (win_count == theMap->territories.size()) {
            cout << "A Player has all the territories" << endl;
            *state = WIN;
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

void GameEngine::addPlayer(Player* p) {
    activePlayers.push_back(p);
}

GameEngine::GameState GameEngine::getState()
{
    return *state;
}


