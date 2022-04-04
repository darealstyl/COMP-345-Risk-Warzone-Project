#pragma once
#include <vector>
#include <string>
using namespace std;
namespace warzoneutils {
	void splitInput(const string&, const char, vector<string>&);
	void splitInput(const string&, vector<string>&);
	constexpr char mapdirectory[] = "MapFiles/";
}