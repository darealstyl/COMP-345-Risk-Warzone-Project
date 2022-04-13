#include "PlayerStrategies.h"
#include "../Game Engine/GameEngine.h"

int main() {
	/*
		Driver Requirements:
		(1) Different players can be assigned different strategies that lead to different behaviour using the Strategy design pattern.
		(2) The strategy adopted by a player can be changed dynamically during play.
		(3) The human player makes decisions according to user interaction, and computer players make decisions automatically. Both using Strategy pattern.	
	*/
	/*
		Code Requirements:
		(1) Player class does not have subclasses that implement behaviours.
		(2) PlayerStrategy is an Abstract class that is not a subclass of the Player class.
		(3) For each strategy listed, there is a ConcreteStrategy class for each that are subclasses of PlayerStrategy.
		(4) Each of the ConcreteStrategy classes implement their own issueOrder(), toAttack(), and toDefend().
		(5) The Player class contains a data member of type PlayerStrategy.
		(6) The issueOrder(), toAttack(), and toDefend() methods of the player do no implement behaviours, they just delegate to their strategies implementation.
	
	*/
	GameEngine* g = new GameEngine();
	g->startupPhase();

	delete g;
	cin.get();
}
