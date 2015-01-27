#ifndef UNIT_HPP
#define UNIT_HPP

#include <cstdlib>
#include <cfloat>
#include <iostream>
#include <sstream>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <math.h>
#include <unistd.h> // for sleep
#include <cmath> // for round
#include <assert.h>

#ifndef OGRE_OBJ_HPP
#include "ogre_obj.hpp"
#endif

#ifndef HERO_HPP
#include "hero.hpp"
#endif

#ifndef HELPER_HPP
#include "helper.hpp"
#endif

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 100
#endif

#ifndef UNIT_SIZE
#define UNIT_SIZE 20
#endif

#ifndef HEROES_IN_UNIT
#define HEROES_IN_UNIT 3
#endif

#ifndef BATTLE_DELAY
#define BATTLE_DELAY 500000
#endif

#ifndef NUM_UNIT_TYPES
#define NUM_UNIT_TYPES 4
#endif

#ifndef SPRITE_SCALE
#define SPRITE_SCALE (1.3)
#endif

#ifndef SPRITE_SIZEX
#define SPRITE_SIZEX 250
#endif

#ifndef SPRITE_SIZEY
#define SPRITE_SIZEY 300
#endif

#ifndef UNIT_HEALTH_SCALE
#define UNIT_HEALTH_SCALE 3.0
#endif

#ifndef UNIT_HEAL_PERCENT
#define UNIT_HEAL_PERCENT 1
#endif

class OgreUnit: public OgreObject{
    // This represents the unit for now
    // Who's in this unit?
    std::list<OgreHero*> heroes;

    bool visible;

public:
    OgreUnit(const sf::Vector2f& p);

    // Fixed for now, later random or based on heroes?
    int unit_type;

    // Who are my heroes?
    inline std::list<OgreHero*> *get_heroes()
    {
        return &heroes;
    }

    // Set my general, Maybe roll this into constructor
    inline void set_owner(OgrePlayer *player)
    {
        owner = player;
        sf::Color color = owner->get_color();
        circ.setFillColor(color);
        vision_aura.setOutlineColor(sf::Color(color.r,color.g,color.b,127));
        for (auto hero:heroes)
        {
            hero->set_owner(owner);
        }
        //info.SetColor(owner->get_color());
    }
    
    // Set my general, Maybe roll this into constructor
    inline void set_max_hp()
    {
        max_hp = 0;
        for (auto hero:heroes)
        {
            max_hp += hero->get_hp();
        }
    }

    // How lively am I?
    inline int get_hp() const
    {
        int total_hp = 0;
        for (auto hero: heroes)
        {
            total_hp += hero->get_hp();
        }
        return total_hp;
    }

    // What's my field pay?
    inline int get_cost() const
    {
        int total_cost = 0;
        for (auto hero: heroes)
        {
            total_cost += hero->get_cost();
        }
        return total_cost;
    }

    // How much fighting left?
    inline int get_remaining_attacks() const
    {
        int remaining_attacks = 0;
        for (auto hero:heroes)
            remaining_attacks += hero->get_attacks_left();
        return remaining_attacks;
    }

    // Have I been noticed?
    inline void set_visible(bool am_visible)
    {
        visible = am_visible;
    }

    inline bool get_visible() const
    {
        return visible;
    }

    // we can see as far as our best hero
    inline int get_vision() const
    {
        int vision = 0;
        for (auto hero: heroes)
        {
            vision = std::max(vision,hero->get_vision());
        }
        return vision;
    }

    inline void collect_pay()
    {
        for (auto hero: heroes)
        {
            hero->collect_pay(get_owner());
        }
    }

    // we use a health bar instead of text to be awesome
    // TODO: integrate this into ogre_obj
    inline void draw_on(sf::RenderWindow& window)
    {
        window.draw(circ);
        window.draw(sprite);
        window.draw(vision_aura);
        health_bar_bg.setPosition(get_position() -
                sf::Vector2f(UNIT_SIZE,-1*UNIT_SIZE));
        window.draw(health_bar_bg);
        health_bar_current.setPosition(get_position() -
                sf::Vector2f(UNIT_SIZE,-1*UNIT_SIZE));
        window.draw(health_bar_current);
    }

    inline void heal()
    {
        for (auto hero: heroes)
        {
            hero->set_hp(hero->get_hp() + 1);
        }
    }

    // Outline the unit in red so we know it's fighting
    void show_fighting()
    {
        circ.setOutlineColor(sf::Color::Red);
        circ.setOutlineThickness(UNIT_SIZE/5);
    }

    // remove fight halo
    void done_fighting()
    {
        circ.setOutlineColor(sf::Color(0, 0, 0));
        circ.setOutlineThickness(OBJECT_SIZE/10);
    }

    // Fight it out!
    void fight(OgreUnit *enemy);

    // Picture a battle
    void fight_draw_on(sf::RenderWindow& window);

};

#endif
