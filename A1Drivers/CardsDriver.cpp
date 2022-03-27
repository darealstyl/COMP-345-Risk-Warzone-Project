#include <iostream>
#include <vector>
#include "Cards.h"
#include "../Player/Player.h"

using namespace std;

int main() {
    // Initialize the random seed using the current epoch time
    srand(time(0));

    Deck* deck = new Deck();    
    Player* player = new Player("Jonah");

    // Output status
    cout << "- Deck and Player created." << endl;
    cout << *deck;
    cout << "Player Hand Size: " << player->hand->cards.size() << endl;
    cout << "Player Order List Size: " << player->orderList->list.size() << endl;

    // Fill players hand with cards
    cout << endl << "- Filling the Hand..." << endl;
    for (int i = 0; i < HAND_SIZE; i++) {
        Card* c = deck->draw();
        cout << "Card Drawn: " << c->cardTypeToString() << endl;
        player->hand->addCard(c);
    }

    // Output status
    cout << endl << "- Hand is filled." << endl;
    cout << *deck;
    cout << "Player Hand Size: " << player->hand->cards.size() << endl;
    cout << "Player Order List Size: " << player->orderList->list.size() << endl;

    // Play each card
    // Should remove it from the hand and add back to the deck
    cout << endl << "- Playing each Card..." << endl;
    while (!player->hand->cards.empty()) {
        Card* c = player->hand->cards.back();
        c->play(player, deck);
    }

    // Output status
    cout << endl << "- All Cards in Hand have been played." << endl;
    cout << *deck;
    cout << "Player Hand Size: " << player->hand->cards.size() << endl;
    cout << "Player Order List Size: " << player->orderList->list.size() << endl;

    // Free memory
    delete deck;
    delete player;

    cin.get();
}