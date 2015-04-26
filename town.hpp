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
#define TOWN_TRANS 200
#endif

#ifndef TOWN_HEALTH_SCALE
#define TOWN_HEALTH_SCALE 1.0
#endif

#ifndef TOWN_MAX_HP
#define TOWN_MAX_HP 16
#endif

#ifndef TOWN_MAX_PILLAGE_DAMAGE
#define TOWN_MAX_PILLAGE_DAMAGE TOWN_MAX_HP
#endif

#ifndef TOWN_HEAL_PERCENT
#define TOWN_HEAL_PERCENT 1
#endif

class OgreTown: public OgreObject{
    // This represents the town for now
    // How much does this town pay out?
    int payout;

    // check if this is our primary town
    bool capitol;

    // town hit points
    int hp;
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

    inline void set_payout(int new_payout)
    {
        payout = new_payout;
    }

    inline int get_payout() const
    {
        return payout;
    }

    // current hit points
    inline int get_hp() const
    {
        return hp;
    }

   // Pay Caesar what is due to him
    inline void pay_taxes()
    {
        OgrePlayer *player;
        player = get_owner();
        player->set_gold(player->get_gold() + get_payout());
    }

    inline void draw_on(sf::RenderWindow& window)
    {
        window.draw(circ);
        window.draw(sprite);
        health_bar_bg.setPosition(get_position() -
                sf::Vector2f(TOWN_SIZE,-1.2*TOWN_SIZE));
        window.draw(health_bar_bg);
        health_bar_current.setPosition(get_position() -
                sf::Vector2f(TOWN_SIZE,-1.2*TOWN_SIZE));
        window.draw(health_bar_current);
    }

    inline void pillage()
    {
        hp -= std::rand() % TOWN_MAX_PILLAGE_DAMAGE;
        if (hp < 0) { hp = 0;}
    }

    inline void heal()
    {
        hp += 1;
        if (hp > max_hp) { hp = max_hp;}
        set_info(hp);
    }

    // Check if town has been captured and set new owner
    OgrePlayer * check_conquest(std::list<OgreUnit*> units);

};

#endif
