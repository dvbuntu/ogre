#ifndef TOWN_HPP
#define TOWN_HPP

#include <cstdlib>
#include <cfloat>
#include <list>
#include <SFML/Graphics.hpp>
#include <math.h>

#ifndef UNIT_HPP
#include "unit.hpp"
#endif

#ifndef PLAYER_HPP
#include "player.hpp"
#endif

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 100
#endif

#ifndef TOWN_SIZE
#define TOWN_SIZE 20
#endif

#ifndef TOWN_TRANS
#define TOWN_TRANS 160
#endif

class OgreTown{
    // This represents the town for now
    // Maybe future just for radius collision in future
    sf::CircleShape circ;

    // What's my name (or the leader's name)
    std::string name;

    // Who owns the town;
    OgrePlayer *owner;

public:
    OgreTown(const sf::Vector2f& p);

    // Who run barter town?
    inline OgrePlayer *get_owner()
    {
        return owner;
    }

    // change owner, as well as color
    inline void set_owner(OgrePlayer *player)
    {
        owner = player;
        sf::Color temp_color = owner->get_color();
        temp_color.a = TOWN_TRANS;
        circ.setFillColor(temp_color);
    }

    // Get the current position of this town
    // For now the position of the circle
    inline sf::Vector2f get_position() const
    {
        return circ.getPosition();
    }

    // Shout my name!
    inline std::string get_name()
    {
        return name;
    }

    // How big am I?
    inline float get_size() const
    {
        return circ.getRadius();
    }

    inline void draw_on(sf::RenderWindow& window) const
    {
        window.draw(circ);
    }

    // Check if town has been captured and set new owner
    OgrePlayer * check_conquest(std::list<OgreUnit*> units);

};

#endif
