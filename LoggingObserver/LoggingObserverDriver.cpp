#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Game Engine/GameEngine.h"
#include "../CommandProcessor/CommandProcessor.h"
#include <string>
#include <sstream>

int main() {

	LogObserver* logger = new LogObserver();

	// show Command, CommandProcessor logging later

	// ----- Order/OrderList Test -----
	string* na = new string("North America");
	string* ca = new string("Canada");
	string* us = new string("United States");
	Player* p1 = new Player("Michael");
	Player* p2 = new Player("William");
	Continent* northAmerica = new Continent(*na);
	Territory* canada = new Territory(*ca, northAmerica);
	Territory* unitedStates = new Territory(*us, northAmerica);

	Deploy* deploy = new Deploy(p1, 1, canada);
	Advance* advance = new Advance(p1, 1, canada, unitedStates);
	Blockade* blockade = new Blockade(p1, canada);
	Bomb* bomb = new Bomb(p1, canada);
	Airlift* airlift = new Airlift(p1, 1, canada, unitedStates);
	Negotiate* negotiate = new Negotiate(p1, p2);
	
	deploy->attach(logger);
	advance->attach(logger);
	bomb->attach(logger);
	blockade->attach(logger);
	airlift->attach(logger);
	negotiate->attach(logger);

	p1->orderList->attach(logger);
	p2->orderList->attach(logger);

	p1->orderList->add(deploy);
	p1->orderList->add(advance);
	p1->orderList->add(bomb);
	p2->orderList->add(blockade);
	p2->orderList->add(airlift);
	p2->orderList->add(negotiate);

	p1->orderList->list.front()->execute();

	
	// ----- Command / CommandProcessor Test -----
	CommandProcessor* cmdProc = new CommandProcessor();
	cmdProc->attach(logger);
	// following line should write the input command to gamelog
	std::cout << "\nEnter a command: ";
	Command* cmd = cmdProc->getCommand();
	cmd->attach(logger);
	// following line should write this effect to gamelog
	cmd->saveEffect("TEST EFFECT");

	// ----- GameEngine Test -----
	GameEngine* engine = new GameEngine();
	engine->attach(logger);
	engine->start();

	// free mem
	delete logger;
	delete engine;
	delete p1;
	delete p2;
	delete na;
	delete ca; 
	delete us;
	delete canada;
	delete unitedStates;
	delete northAmerica;
	delete cmdProc;
	std::cin.get();
}
