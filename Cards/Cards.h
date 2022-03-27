#pragma once

#include <vector>
#include <iostream>

#define DECK_SIZE 50
#define HAND_SIZE 5

class Hand;
class Deck;
class Player;

class Card {
    public:
        enum class CardType {
            AIRLIFT,
            BLOCKADE,
            BOMB,
            DIPLOMACY,
            REINFORCEMENT
        };        

        Card(CardType);
        Card(const Card&);
        ~Card();

        CardType getCardType() const;
        std::string cardTypeToString() const;

        void play(Player*, Deck*);

        Card& operator=(const Card&);
        friend std::ostream& operator<<(std::ostream&, const Card&);
    
    private:
        CardType type;
};

class Hand {
    public:  
        std::vector<Card*> cards;      
        Hand();
        Hand(const Hand&);
        ~Hand();
       
        void addCard(Card*);      

        Hand& operator=(const Hand&);
        friend std::ostream& operator<<(std::ostream&, const Hand&);
        
    private: 
};

class Deck {
    public:
        Deck();
        Deck(const Deck&);
        ~Deck();

        Card* draw();
        void addCard(Card*);

        Deck& operator=(const Deck&);
        friend std::ostream& operator<<(std::ostream&, const Deck&);

    private:
        std::vector<Card*> cards;
};
