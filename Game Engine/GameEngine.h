// Create a finite state machine for the game engine 
//Create an enum that represents the different possible states
//For each state create a function with the name of the state that is being call when the user wants to transition to that state
//always set the currentstate after executing the function


//create an enum variable to keep track of the current state in the switch statement

#pragma once

#include <iostream>
#include "../LoggingObserver/LoggingObserver.h"
#include "../Player/Player.h"
#include "../CommandProcessor/CommandProcessor.h"
using namespace std;

class CommandProcessor;
class Command;

class GameEngine : public Subject, public ILoggable {
public:
    enum class GameState { //all the different states
        START, MAP_LOADED, MAP_VALIDATED, PLAYERS_ADDED, ASSIGN_REINFORCEMENT, ISSUE_ORDERS, EXECUTE_ORDERS, WIN
    };

    GameEngine(); //default constructor
    GameEngine(const GameEngine &game1); //copy constructor
    GameEngine& operator =(const GameEngine&); //assignment operator
    ~GameEngine(); //destructor

    friend ostream & operator << (ostream &out, const GameEngine &g);
    friend istream & operator >> (istream &in,  GameEngine &g);

    void start(); //start the gameEngine in the first state
    void startupPhase();

    void addPlayer(Player*);
    void addPlayer(string);
    int getPlayerCount();

   
    GameState getState();

    std::string stateToString();
    std::string stringToLog() override;

 /* 
    void mapLoadedPhase();
    void mapValidatedPhase();
    void playersAddedPhase();
    void assignReinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void winPhase();
    */Map* map;
private:
    CommandProcessor* commandprocessor;
    // Enum doesn't need to be of pointer type
    GameState state;
    vector<Player*> activePlayers;
    Deck* deck;
    bool running;

    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void transition(GameEngine::GameState);
    void initializeCommandProcessor();
    void getandexecutecommand();
    void execute(Command*);
    void mainGameLoop();
    void gamestart();
    void distributeterritories();
    void randomizeplayerorder();
    void distributearmies();
    void distributecards();
    void checkforwin();
    void removelosers();
    void getcontrolledcontinents(unordered_map<Player*, vector<Continent*>*>&);
    void cleanupcontrolledcontinents(unordered_map<Player*, vector<Continent*>*>&);
    
};



