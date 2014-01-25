#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdlib>
#include <SFML/Graphics.hpp>

#ifndef NO_PLAYER
#define NO_PLAYER OgrePlayer(0)
#endif

#ifndef PLAYER
#define PLAYER OgrePlayer(1)
#endif

#ifndef ENEMY
#define ENEMY OgrePlayer(2)
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
    inline int get_color() const
    {
        return color;
    }

    bool operator == (const OgrePlayer A) const
    {
        return id == A.get_id(); // feels a little dirty just grabbing the id
    }
}

#endif
