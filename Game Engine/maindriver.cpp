#include "GameEngine.h"

using namespace std;

int main() {
	LogObserver* logger = new LogObserver();
	GameEngine* normalgame = new GameEngine();
	normalgame->attach(logger);
	normalgame->startupPhase();
	cin.get();
}