#include <iostream>
#include "GameEngine.h"
using namespace std;

int main()
{

   GameEngine*g = new GameEngine();
   g->start();
   delete g;
   g = nullptr;
}
