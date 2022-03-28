#include "CommandProcessor.h"
#include "../Game Engine/GameEngine.h"

int main() {
	CommandProcessor* consoleprocessor = new CommandProcessor();
	GameEngine* engine = new GameEngine();

	CommandProcessor* fileprocessor = new FileCommandProcessorAdapter(engine, "commands.txt");



	delete consoleprocessor;
	delete fileprocessor;
}