#include "GameEngine.h"

using namespace std;

int main() {
	GameEngine forcewingame;;
	forcewingame.FORCEWIN = true;
	forcewingame.startupPhase();
	cout << "Warning: next game will go by very fast and you won't be able to observe without breakpoints." << endl;
	
	
	GameEngine normalgame;
	normalgame.startupPhase();
	cin.get();
}