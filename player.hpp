#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdlib>
#include <SFML/Graphics.hpp>

#ifndef NO_PLAYER
#define NO_PLAYER 0
#endif

#ifndef PLAYER
#define PLAYER 1
#endif

#ifndef ENEMY
#define ENEMY 2
#endif

class OgrePlayer{
    int id;
    sf::Color color;

public:
    OgrePlayer(const int new_id);

    // Who am I?
    inline int get_id() const
    {
        return id;
    }

    // What do I wear?
    sf::Color get_color() const
    {
        return color;
    }

    bool operator == (OgrePlayer* A) const
    {
        return this->id == A->get_id();
    }
};

#endif
