#include "warzoneutils.h"

void warzoneutils::splitInput(const string& line, const char delimiter, vector<string>& destination) {
	destination.clear();
	size_t i = 0, j = 0;

	for (; i < line.size(); i++) {
		if (line[i] == delimiter) {
			// Check first if j is already equal to i. (This happens when theres two consecutive spaces, we do not want to save "").
			if (i != j) destination.push_back(line.substr(j, i - j));
			j = i + 1;
		}
	}

	// save the last part when i goes out of bounds if previous character was not an empty space.
	if (i != j) destination.push_back(line.substr(j, i - j));

}
void warzoneutils::splitInput(const string& line, vector<string>& destination) {
	splitInput(line, ' ', destination);
}