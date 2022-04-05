#include "PlayerStrategies.h"

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

	Player* p1 = new Player("Michael", new HumanPlayerStrategy());
	Player* p2 = new Player("Cedric", new BenevolentPlayerStrategy());
	Player* p3 = new Player("Jonah", new AggressivePlayerStrategy());
	Player* p4 = new Player("William", new NeutralPlayerStrategy());
	Player* p5 = new Player("test", new CheaterPlayerStrategy());
	
	cout << *p1 << endl;
	p1->issueOrder();
	p1->toAttack();
	p1->toDefend();

	cout << *p2 << endl;
	p2->issueOrder();
	p2->toAttack();
	p2->toDefend();

	cout << *p3 << endl;
	p3->issueOrder();
	p3->toAttack();
	p3->toDefend();

	cout << *p4 << endl;
	p4->issueOrder();
	p4->toAttack();
	p4->toDefend();

	cout << *p5 << endl;
	p5->issueOrder();
	p5->toAttack();
	p5->toDefend();

	delete p1;
	delete p2;
	delete p3;
	delete p4;
	delete p5;
	cin.get();
}
