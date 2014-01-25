#include "player.hpp"

OgrePlayer::OgrePlayer(const int new_id)
{
    // Set who I am
    id = new_id;

    // What do I look like?
    color = sf::Color(std::rand() % 256,std::rand() % 256,std::rand() % 256);

}
