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
#define TOWN_SIZE 30
#endif

#ifndef TOWN_TRANS
#define TOWN_TRANS 160
#endif

class OgreTown: public OgreObject{
    // This represents the town for now
    // How much does this town pay out?
    int payout;

public:
    OgreTown(const sf::Vector2f& p);

    // change owner, as well as color
    inline void set_owner(OgrePlayer *player)
    {
        owner = player;
        sf::Color temp_color = owner->get_color();
        temp_color.a = TOWN_TRANS;
        circ.setFillColor(temp_color);
    }

    inline int get_payout() const
    {
        return payout;
    }

    // Pay Caesar what is due to him
    inline void pay_taxes()
    {
        OgrePlayer *player;
        player = get_owner();
        player->set_gold(player->get_gold() + get_payout());
    }

    // Check if town has been captured and set new owner
    OgrePlayer * check_conquest(std::list<OgreUnit*> units);

};

#endif
