#include "CommandProcessor.h"
int main() {
	CommandProcessor* consoleprocessor = new CommandProcessor();

	CommandProcessor* fileprocessor = new FileCommandProcessorAdapter("commands.txt");



	delete consoleprocessor;
	delete fileprocessor;
}