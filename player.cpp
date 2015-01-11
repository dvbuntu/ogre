#include "player.hpp"

OgrePlayer::OgrePlayer(const int new_id)
    //: circ(new_id) // Just a dummy line to inherit from sfml circle?
{
    // Set who I am
    id = new_id;

    // What do I look like?
    color = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);

    // Make it rain!
    gold = (std::rand() % MIN_START_GOLD) + MIN_START_GOLD;

    // Get some land!
    num_towns = 0;

    // Get some soldiers!
    num_units = 0;

    // Where we start!
}

bool operator == (const OgrePlayer& A, const OgrePlayer& B)
{
    return A.get_id() == B.get_id();
}
