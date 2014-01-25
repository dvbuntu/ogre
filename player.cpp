#include "player.hpp"

OgrePlayer::OgrePlayer(const int new_id)
    //: circ(new_id) // Just a dummy line to inherit from sfml circle?
{
    // Set who I am
    id = new_id;

    // What do I look like?
    color = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);
}
