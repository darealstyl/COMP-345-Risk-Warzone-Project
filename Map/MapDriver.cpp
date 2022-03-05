#include "Map.h"
#include <iostream>
using namespace std;

int main()
{
    vector<string> filenames ({ "solar.map" });
    vector<Map*> validmaps;

    for (string filename : filenames) {
        try {
            Map& map = MapLoader::createMap(filename);
            if (map.validate()) {
                cout << "The map found in \"" << filename << "\" is valid." << endl;
                validmaps.push_back(&map);
            }
            else throw "Invalid Map";
        }
        catch (const char* txtException) {
            cout << "Exception: " << txtException << endl;
            cout << "Rejected file " << filename << ". It is an invalid file.";
        }
        catch (...) {
            cout << "Rejected file " << filename << ". It is an invalid file.";
        }
    }

    for (Map* map : validmaps) delete map;

    cin.get();
    
} 