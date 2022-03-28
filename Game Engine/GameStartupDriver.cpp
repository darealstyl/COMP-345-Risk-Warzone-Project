#include "GameEngine.h"

using namespace std;

int main() {
	GameEngine* game = new GameEngine();
	game->FORCEWIN = true;
	game->startupPhase();
	cin.get();

}