
#include "Map.h"
#include "../Utills/warzoneutils.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;
using warzoneutils::splitInput;
// Map implementation
Map::Map() {}

Map::Map(const Map& map) {
    unordered_map<Continent*, Continent*> continents;

    for (Continent* continent : map.continents) {
        Continent* copy = new Continent(continent->name, continent->armyValue);
        continents.insert(make_pair(continent, copy));
        this->continents.push_back(copy);
    }

    unordered_map<Territory*, Territory*> territories;
    
    for (Territory* territory : map.territories) {
        Continent* copycontinent = continents.find(territory->continent)->second;
        Territory* copy = new Territory(territory->name, copycontinent);
        territories.insert(make_pair(territory, copy));
        this->territories.push_back(copy);
    }

    for (Continent* continent : map.continents) {
        Continent* copy = continents.find(continent)->second;
        for (Territory* territory : continent->territories) {
            copy->territories.push_back(territories.find(territory)->second);
        }
    }

    for (Territory* territory : map.territories) {
        Territory* copy = territories.find(territory)->second;
        for (Territory* adjacentTerritory : territory->adjacentTerritories) {
            copy->adjacentTerritories.push_back(territories.find(adjacentTerritory)->second);
        }
    }

}

Map& Map::operator=(const Map& map) {
    territories = map.territories;
    continents = map.continents;
    return *this;
}

bool Map::validate() {
    if (!mapIsConnectedGraph()) return false;
    if (!continentsAreConnectedGraphs()) return false;
    if (!countriesBelongOneContinent()) return false;
    return true;
}

bool Map::mapIsConnectedGraph() {
    // If the map is a connected graph, we should be able to start from any node, perform Depth-First Search,
    // and end up with a set that contains every node in the map.
    for (Territory* territory : territories) {
        unordered_set <string> nameset;

        performDFS(territory, nameset);

        if (!setContainsTerritories(nameset, territories)) return false;
    }

    return true;
}
/*
* Similar approach to mapIsConnectedGraph, but adding an end condition for the DFS is that if the current Territory's continent is not the same
* as the next, then don't add it to the set.
*/
bool Map::continentsAreConnectedGraphs() {
    for (Continent* continent : continents) {
        vector<Territory*>& continentTerritories = continent->territories;

        for (Territory* territory : continent->territories) {
            unordered_set <string> nameset;

            performContinentDFS(territory, nameset);

            if (!setContainsTerritories(nameset, continentTerritories)) return false;
        }
    }
    return true;
}


// Checks if the set contains all of the territories in the vectors
bool Map::setContainsTerritories(unordered_set <string> nameset, vector<Territory*>& territories) {
    for (const Territory* territory : territories) {
        if (nameset.find(territory->name) == nameset.end()) {
            return false;
        }
    }
    return true;
}

/*
* For every continent, every one of its territories should be pointing to it.
*/
bool Map::countriesBelongOneContinent() {
    for (Continent* continent : continents) {
        for (const Territory* territory : continent->territories) {
            if (continent != territory->continent) return false;
        }
    }
    return true;
}

void Map::performContinentDFS(const Territory* territory, unordered_set <string>& nameset) {
    if (nameset.find(territory->name) != nameset.end()) return;

    nameset.insert(territory->name);

    for (const Territory* adjacentTerritory : territory->adjacentTerritories) {
        // If both territories have the same continent, then you can explore the next territory.
        if(territory->continent == adjacentTerritory->continent) 
            performDFS(adjacentTerritory, nameset);
    }
}

void Map::performDFS(const Territory* territory, unordered_set <string>& nameset) {
    // If the current territory already exists in the set, exit the function (to not get stuck in infinite loop)
    if (nameset.find(territory->name) != nameset.end()) return;

    // Insert to the set before to avoid infinite loop.
    nameset.insert(territory->name);

    for (const Territory* adjacentTerritory : territory->adjacentTerritories) {
        // Recursively visit the neighbors
        performDFS(adjacentTerritory, nameset);
    }
}

std::ostream& operator<<(std::ostream& out, const Map& continent) {
    return out;
}

// The map's destructor is the one that handles destroying the territories & continents
Map::~Map() {
    cout << "Destroying the map" << endl;

    for (Territory* territory : territories)
        delete territory;

    territories.clear();

    for (Continent* continent : continents)
        delete continent;

    continents.clear();
}
// Continent implementation
Continent::Continent(string& name, int armyValue) {
    this->name = name;
    this->armyValue = armyValue;
}

// Shallow copy because otherwise it would require copying the entire map? Seems quite useless considering the scope of this project.
Continent::Continent(const Continent& continent) {
    this->name = continent.name;
    this->territories = continent.territories;
    this->armyValue = continent.armyValue;
}

// No need to do anything in here, the destructor of vector is called by default and frees the pointers but not what they're pointing to which will be handled
// by the map
Continent::~Continent() {}

// Print the Territory type as a string
std::ostream& operator<<(std::ostream& out, const Continent& continent) {
    return out << continent.name << " with army value " << continent.armyValue;
}

// Territory comparator
bool Territory::operator<(const Territory& t) const {
    return nbOfArmy < t.nbOfArmy;
}

bool Territory::territoriesAreAdjacent(Territory* t1, Territory* t2) {
    for (Territory* neighbor : t1->adjacentTerritories) {
        if (t2 == neighbor) return true;
    }

    return false;
}

// Territory implementation
Territory::Territory(const string& name, Continent* continent) {
    this->name = name;
    this->continent = continent;
    this->nbOfArmy = 0;
    this->owner = Player::neutralplayer;
}

// Shallow copy because otherwise it would require copying the entire map? Seems quite useless considering the scope of this project.
Territory::Territory(const Territory& territory) {
    this->name = territory.name;
    this->continent = territory.continent;
    this->nbOfArmy = territory.nbOfArmy;
    this->owner = territory.owner;
}

Territory& Territory::operator=(const Territory& territory) {
    this->name = territory.name;
    this->continent = territory.continent;
    this->nbOfArmy = territory.nbOfArmy;
    this->owner = territory.owner;

    return *this;
}

/*
* Only set pointers to null, the Map object will handle deleting continents and the Game Engine will handle deleting the players
*/
Territory::~Territory() {
    continent = NULL;
    owner = NULL;
}

// Print the Territory type as a string
std::ostream& operator<<(std::ostream& out, const Territory& territory) {
    out << territory.name;
    return out;
}


// <-- MAP LOADER -->
Map* MapLoader::createMap(string filename) {
	const string continentsflag = "[continents]";
	Map* gamemap = new Map();

	ifstream mapstream;
	string line;
	cout << "Reading from " << filename << endl;
	mapstream.open("MapFiles/" + filename);

	// This will get the next line until it hits "[continents]" which will indicate the start of the continents.
	while (getline(mapstream, line) && line != continentsflag);

	unordered_map<int, Continent*> continents;
	int continentnumber = 1;

	cout << endl << "Processing continents" << endl;
	// This will process every continent until there isn't any continent left.
	while (getline(mapstream, line) && line != "") {
		Continent& continent = createContinent(line);
		gamemap->continents.push_back(&continent);

		pair<int, Continent*> currentpair(continentnumber, &continent);
		continents.insert(currentpair);

		cout << "\tProcessing continent #" << continentnumber << " " << continent << endl;

		continentnumber++;
	}

	// Skip [countries] line
	getline(mapstream, line);

	unordered_map<int, Territory*> countries;

	cout << endl << "Processing countries" << endl;
	// This will process every countries until there isn't any left.
	while (getline(mapstream, line) && line != "") {
		int countrynumber;

		Territory& territory = createTerritory(line, continents, countrynumber);
		gamemap->territories.push_back(&territory);

		pair<int, Territory*> currentpair(countrynumber, &territory);
		countries.insert(currentpair);

		cout << "\tProcessing country #" << countrynumber << " " << territory << " that is part of " << *(territory.continent) << endl;
	}

	// Skip [borders] line
	getline(mapstream, line);

	cout << endl << "Processing borders" << endl;
	// This will process every borders until there isn't any left.
	while (getline(mapstream, line) && line != "") {

		processBorders(line, countries);

	}

	mapstream.close();

	return gamemap;
}

void MapLoader::processBorders(string& line, unordered_map<int, Territory*> countries) {
    vector<string> split;
    splitInput(line, split);

	int countrynumber = stoi(split[0]);

	Territory* origincountry = countries.at(countrynumber);

	cout << "\tProcessing the borders of " << *origincountry << ". Added borders to the following countries: ";

	for (size_t i = 1, N = split.size(); i < N; i++) {
		int bordercountrynumber = stoi(split[i]);
		Territory* bordercountry = countries.at(bordercountrynumber);

		origincountry->adjacentTerritories.push_back(bordercountry);

		cout << *bordercountry << " ";
	}
	cout << endl;
}

Continent& MapLoader::createContinent(string& line) {
    vector<string> split;
    splitInput(line, split);

	string name = split[0];
    int armyValue = stoi(split[1]);

	Continent* continent = new Continent(name, armyValue);
	return *continent;
}

Territory& MapLoader::createTerritory(string& line, unordered_map<int, Continent*> continents, int& countrynumber) {
    vector<string> split; 
    splitInput(line, split);

	countrynumber = stoi(split[0]);
	string name = split[1];
	int continentid = stoi(split[2]);

	// Retrieve the continent from the map and add the new territory to the list of territories of that continent
	Continent* continent = continents.at(continentid);

	Territory* territory = new Territory(name, continent);

	continent->territories.push_back(territory);

	return *territory;
}



