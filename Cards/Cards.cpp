#include <algorithm>

#include "Cards.h"
#include "../Player/Player.h"
#include "../Orders/Orders.hpp"

// Deck Constructor
// Decks are initialized with DECK_SIZE amount of Cards
Deck::Deck() { 
    for (int i = 0; i < DECK_SIZE; i++) {

        // Alternate through the types
        Card* c = new Card(static_cast<Card::CardType>(i % 5));
        addCard(c);
        
    }

}

// Deck Copy Constructor
Deck::Deck(const Deck& d) {
    cards = d.cards;
}

// Deck Destructor
// Deck is a vector of pointers, delete each pointer to free memory, then clear the vector
Deck::~Deck() {
    for(Card* c : cards)
        delete c;
    
    cards.clear();
}

// Draw a Card from the Deck
// Picks a random index, removes that Card from the Deck and returns it.
Card* Deck::draw() {
    int randIndex = rand() % cards.size();
    Card* c = cards.at(randIndex);
    cards.erase(cards.begin() + randIndex);
    return c;
}

// Insert a Card at the end of the Deck
void Deck::addCard(Card* c) {
    cards.push_back(c);
}

// Assignment Operator
// Move the Deck cards to the recipient Deck
Deck& Deck:: operator=(const Deck& d) {
    cards = d.cards;
    return *this;
}

// Stream Insertion Operator
// Currently just printing the Deck size
std::ostream& operator<<(std::ostream& out, const Deck& deck) {
    out << "Deck Size: " << deck.cards.size() << std::endl;
    return out;
}


// Hand Constructor
Hand::Hand() {}

// Hand Copy Constructor
Hand::Hand(const Hand& h) {
    cards = h.cards;
}

// Hand Destructor
// Free each Cards memory then clear the Hand's vector
Hand::~Hand() {
    for(Card* c : cards) 
        delete c;
    
    cards.clear();
}

// Places a Card at the end of the Hand
void Hand::addCard(Card* c) {
    cards.push_back(c);
}

// Assignment Operator
// Move the Hand cards to the recipient Hand
Hand& Hand::operator=(const Hand& h) {
    cards = h.cards;
    return *this;
}

// Stream Insertion Operator
// Currently just printing the Hand size
std::ostream& operator<<(std::ostream& out, const Hand& hand) {
    out << "Hand Size: " << hand.cards.size() << std::endl;
    return out;
}


// Card Constructor
// Cards require a CardType to be created
Card::Card(CardType c) {
    type = c;
}

// Card Copy Constructor
Card::Card(const Card& c) {
    type = c.getCardType();
}

// Card Destructor
// Free the Card
Card::~Card() {
};

// CardType Value Getter
Card::CardType Card::getCardType() const {
    return type;
}

// Converts a Cards type to String for printing purposes.
std::string Card::cardTypeToString() const {
    switch(this->getCardType()) { 
        case CardType::AIRLIFT:         return "Airlift";
        case CardType::BLOCKADE:        return "Blockade";
        case CardType::BOMB:            return "Bomb";
        case CardType::DIPLOMACY:       return "Diplomacy";
        case CardType::REINFORCEMENT:   return "Reinforcement";
        default:                        return "Error reading card type.";
    }
}

// Playing a Card
// Create an Order depending on the Cards type
// Remove the Card from the players Hand and add it back to the Deck.
void Card::play(Player* player, Deck* deck) {

    // will add the card type to the order list later
    
    vector<Territory*> defend = player->toDefend();
    vector<Territory*> attack = player->toAttack();

    Order* order = nullptr;

    switch (this->type) {
    case CardType::AIRLIFT: {
        Territory* from = defend.back();
        Territory* to = defend.front();

        int transfer = from->nbOfArmy;
        if (transfer <= 0) {
            transfer = 1;
        }

        int numOfArmies = (rand() % transfer) + 1;

        order = new Airlift(player, numOfArmies, to, from);
        break;
    }
    case CardType::BLOCKADE: {
        Territory* location = defend.front();

        order = new Blockade(player, location);
        break;
    }
    case CardType::BOMB: {
        Territory* location = attack.front();

        order = new Bomb(player, location);

        break;
    }
    case CardType::DIPLOMACY: {
        Territory* territory = attack.front();
        Player* targetPlayer = territory->owner;

        order = new Negotiate(player, targetPlayer);
        break;
    }
    case CardType::REINFORCEMENT: {

        player->reinforcements += 10;
        break;
    }
    }

    if (order != nullptr) {
        player->orderList->add(order);
    }

    // Remove the card from the Hand and add it to the Deck
    cout << "Hand size before playing the card:" << player->hand->cards.size() << endl;
    auto index = std::find(player->hand->cards.begin(), player->hand->cards.end(), this);
    if (index != player->hand->cards.end()) {
        player->hand->cards.erase(index);
        deck->addCard(this);
    }
    cout << "Hand size after playing the card:" << player->hand->cards.size() << endl;

}

// Assignment Operator
// Only concerned about the Card type
Card& Card::operator=(const Card& t){
    this->type = t.type;
    return *this;
}

// Stream Insertion Operator
// Print the Cards type as a string
std::ostream &operator<<(std::ostream &out, const Card& card) {    
    return out << card.cardTypeToString() << std::endl;
}