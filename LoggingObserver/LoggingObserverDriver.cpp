#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Game Engine/GameEngine.h"
#include "../CommandProcessor/CommandProcessor.h"
#include <string>
#include <sstream>

using std::cout;
using std::endl;

int main() {

	LogObserver* logger = new LogObserver();

	#pragma region CommandProcessorTest

	cout << "------------------ COMMANDPROCESSOR TEST ------------------" << endl;
	
	// CommandProcessor will attach all of its observers to any Command it reads
	cout << "Creating a CommandProcessor and attaching the LogObserver" << endl;
	CommandProcessor* cmdProc = new CommandProcessor();
	cmdProc->attach(logger);

	// Will log the command name to the log file
	cout << endl << "Getting a command using CommandProcessor: ";
	Command* cmd = cmdProc->getCommand();

	// Will log the effect provided to the log file
	cout << endl << "Giving the command an effect: TEST EFFECT" << endl;
	cmd->saveEffect("TEST EFFECT");

	delete cmdProc;

	#pragma endregion

	#pragma region GameEngineTest

	cout << endl << "------------------ GAMEENGINE TEST ------------------" << endl;
	cout << endl << "Creating a GameEngine and attaching the LogObserver" << endl;
	GameEngine* engine = new GameEngine();
	engine->attach(logger);
	
	// Will log the new state of the game engine (MAP_LOADED)
	cout << endl << "Transitioning from " << engine->stateToString() <<" to MAP_LOADED" << endl;
	engine->transition(GameEngine::GameState::MAP_LOADED);

	// Will log the new state of the game engine (MAP_VALIDATED)
	cout << endl << "Transitioning from " << engine->stateToString() << " to MAP_VALIDATED" << endl;
	engine->transition(GameEngine::GameState::MAP_VALIDATED);

	delete engine;

	#pragma endregion

	#pragma region OrderAndOrderListTest

	cout << endl << "------------------ ORDER/ORDERLIST TEST ------------------" << endl;
	// Creating necessary objects to demonstrate orderlist issuing and order execution
	cout << "Creating 2 players, 1 of each order and required parameters..." << endl;
	string continentname = "North America";
	Continent* northAmerica = new Continent(continentname, 0);
	Territory* canada = new Territory(string("Canada"), northAmerica);
	Territory* unitedStates = new Territory(string("United State"), northAmerica);
	
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");

	p1->addTerritory(canada);
	p2->addTerritory(unitedStates);

	Deploy* deploy = new Deploy(p1, 1, canada);
	Advance* advance = new Advance(p1, 1, canada, unitedStates);
	Blockade* blockade = new Blockade(p2, unitedStates);
	Bomb* bomb = new Bomb(p1, canada);
	Airlift* airlift = new Airlift(p1, 1, canada, unitedStates);
	Negotiate* negotiate = new Negotiate(p1, p2);
	
	// OrderList will attach all of its observers to any Order objects added into it
	cout << "Attaching the logger to both players..." << endl;
	p1->orderList->attach(logger);
	p2->orderList->attach(logger);

	// Will log each orderlist addition as "order issued..."
	cout << endl << "Adding 3 orders to each player..." << endl;
	p1->orderList->add(deploy);
	cout << endl;
	p1->orderList->add(advance);
	cout << endl;
	p1->orderList->add(bomb);
	cout << endl;
	p2->orderList->add(blockade);
	cout << endl;
	p2->orderList->add(airlift);
	cout << endl;
	p2->orderList->add(negotiate);

	// Will log each execution as "order executed..."
	cout << endl << "Executing the 1st order in each player's orderlists..." << endl;
	p1->orderList->list.front()->execute();
	cout << endl;
	p2->orderList->list.front()->execute();

	// delete all pointers
	delete p1;
	delete p2;
	delete northAmerica;
	delete canada;
	delete unitedStates;
	deploy = nullptr;
	advance = nullptr;
	blockade = nullptr;
	bomb = nullptr;
	airlift = nullptr;
	negotiate = nullptr;

	#pragma endregion

	delete logger;
	std::cin.get();
}
