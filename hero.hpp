#ifndef HERO_HPP
#define HERO_HPP

#include <cstdlib>
#include <list>
#include <cfloat>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <math.h>

#ifndef OGRE_OBJ_HPP
#include "ogre_obj.hpp"
#endif

#ifndef HELPER_HPP
#include "helper.hpp"
#endif

#ifndef PLAYER_HPP
#include "player.hpp"
#endif

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 100
#endif

#ifndef BASE_STR
#define BASE_STR 10
#endif

#ifndef HERO_SIZE
#define HERO_SIZE 10
#endif

#ifndef HERO_HEALTH_SCALE
#define HERO_HEALTH_SCALE 2.0
#endif

#ifndef HERO_XP_SCALE
#define HERO_XP_SCALE 1.0
#endif

#ifndef NO_DAMAGE_DISPLAY
#define NO_DAMAGE_DISPLAY -1
#endif

class OgreHero: public OgreObject{
    // This represents the hero, an individual within a unit

    // Info about the hero
    sf::Text damage_str;

    // current hit points
    int hp;

    // Simple Fight Strength parameter
    int str;

    // resistance to attacks
    int def;

    // number of attacks
    int total_attacks;

    // how many attacks left?
    int attacks_left;

    // What experience level am I?  (TODO: just a placeholder now)
    int level;
    int xp;

    // graphics for XP
    sf::Sprite xp_bar_sprite;

    // xp bar
    sf::RectangleShape xp_bar_bg;
    sf::RectangleShape xp_bar_current;
    float xp_scale;

    // Hero cost
    int cost;

    // How far we can see
    int vision;

    // Position within the unit (TODO: make this affect damage and num attacks)
    int position;

    // How hard did we just get hit?
    int damage_taken;

    // Are we the current attacker or defender?
    bool attacking;
    bool defending;

    // Who owns this hero
    OgrePlayer *owner;

public:
    OgreHero(const int start_level);

    // Get the current position of this hero within the unit
    inline int get_position() const
    {
        return position;
    }

    // Set the current position of this hero within the unit
    inline void set_position( int new_position)
    {
        position = new_position;
    }

    inline void set_damage_str(int str, sf::Font *font, int size)
    {
        damage_str.setString(std::to_string(str));
        damage_str.setFont(*font);
        damage_str.setCharacterSize(size);
        damage_str.setColor(sf::Color::Red);
    }

    // Post battle update
    inline void set_damage_str(int str)
    {
        damage_str.setString(std::to_string(str));
    }

    // How strong am I?
    inline int get_str() const
    {
        return str;
    }

    inline void set_str(int new_str)
    {
        if (new_str > 100)
            str = 100; // TODO: replace with a MAX_STR like 255 or something
        else if (new_str > 0)
            str = new_str;
        else
            // TODO: remove the hero...will happen after battles in main though
            str = 0;
    }

    inline void set_max_hp(int new_max_hp)
    {
        if (new_max_hp > 0)
            max_hp = new_max_hp;
        else
            max_hp = 0;
    }

    // current hit points
    inline int get_hp() const
    {
        return hp;
    }

    inline void set_hp(int new_hp)
    {
        if (new_hp > get_max_hp())
            hp = get_max_hp();
        else if (new_hp > 0)
            hp = new_hp;
        else
            // TODO: remove the hero...will happen after battles in main though
            hp = 0;
    }

    // De-fense!  De-fense!
    inline int get_def() const
    {
        return def;
    }

    inline void set_def(int new_def)
    {
        if (new_def > 100)
            def = 100;
        else if (new_def > 0)
            def = new_def;
        else
            def = 0;
    }

    // How many hits can I do?
    inline int get_total_attacks() const
    {
        return total_attacks;
    }

    // How many hits left in this fight
    inline int get_attacks_left() const
    {
        if (get_hp() == 0)
            return 0;
        else
            return attacks_left;
    }

    inline void set_attacks_left(int new_attacks_left)
    {
        if (new_attacks_left > 10)
            attacks_left = 10; // TODO: replace with a MAX_STR like 255 or something
        else if (new_attacks_left > 0)
            attacks_left = new_attacks_left;
        else
            attacks_left = 0;
    }

    // How fast am I? (TODO: also affects miss rate?)
    inline int get_speed() const
    {
        return speed;
    }

    inline void set_speed(int new_speed)
    {
        if (new_speed > 100)
            speed = 100;
        else if (new_speed > 0)
            speed = new_speed;
        else
            speed = 0;
    }

    // Did I hit someone?
    inline bool get_attacking() const
    {
        return attacking;
    }

    inline void set_attacking(int new_attacking)
    {
        attacking = new_attacking;
        if (attacking)
        {
            circ.setOutlineColor(sf::Color::Green);
            circ.setOutlineThickness(HERO_SIZE/5);
        }
        else
        {
            circ.setOutlineThickness(0);
        }
    }

    // Did I defend myself?
    inline bool get_defending() const
    {
        return defending;
    }

    inline void set_defending(int new_defending)
    {
        defending = new_defending;
        if (defending)
        {
            circ.setOutlineColor(sf::Color::Red);
            circ.setOutlineThickness(HERO_SIZE/5);
        }
        else
        {
            circ.setOutlineThickness(0);
        }
    }

    // How bad did I just get stabbed?
    inline int get_damage_taken() const
    {
        return damage_taken;
    }

    inline void set_damage_taken(int new_damage_taken)
    {
        damage_taken = new_damage_taken;
    }

    // What's my field pay?
    inline int get_cost() const
    {
        return (get_max_hp() + get_def() + get_str() * total_attacks) / 50;
    }

    // Maybe only do this in OgreUnit, iterating through heros
    inline void collect_pay(OgrePlayer *player)
    {
        player->set_gold(player->get_gold() - get_cost());
    }

    // Fight it out!
    OgreHero * attack(std::list<OgreHero*> *enemies);

    inline int get_level()
    {
        return level;
    }

    inline void set_level(int new_level)
    {
        level = new_level;
    }

    inline int get_vision() const
    {
        return vision;
    }

    // compute XP
    // Get the average of the differences between my level and the enemies fought
    inline void gain_xp(std::list<OgreHero*> *enemies)
    {
        int gain = 0;
        for(auto enemy : *enemies)
        {
            gain += enemy->get_level();
        }
        gain -= enemies->size()*get_level();
        gain = round((float) gain / (float) enemies->size());
        if (gain < 1) { gain = 1;}
        xp += gain;
    }

    // how much xp needed for next level?  minimum 10
    inline double xp_for_next_level()
    {
        //return std::max(pow(level+1,2),10.0);
        return 2 * level;
    }

    // do I level up?
    inline void level_up()
    {
        if (xp > xp_for_next_level())
        {
            level += 1;
            max_hp += 1 + (rand() % 2);
            str += 1 + (rand() % 2);
            def += 1 + (rand() % 2);
            speed += 1 + (rand() % 2);
            vision += HERO_SIZE;
            circ.setPointCount(level + 3);
            xp = 0;
        }
    }

    // Show how much we've learned
    inline void set_xp_bar()
    {
        xp_bar_bg.setSize(sf::Vector2f(5,xp_for_next_level()/xp_scale));
        xp_bar_bg.setOutlineColor(sf::Color(125,125,125));
        xp_bar_bg.setOutlineThickness(1);
        xp_bar_current.setSize(sf::Vector2f(5,xp/xp_scale));
//        xp_bar_current.setScale(1, xp/xp_scale);
    }

    inline void draw_at(sf::RenderWindow& window, int x, int y)
    {
        circ.setPosition(x,y);
        window.draw(circ);
        if (get_damage_taken())
            draw_damage(window, get_damage_taken(), x + HERO_SIZE*4, y);
        info_str.setPosition(x - HERO_SIZE*4, y);
        set_info(get_hp());
//        window.draw(info_str);
//        Now with health bars
        health_bar_bg.setPosition(sf::Vector2f(x - HERO_SIZE*4,y));
        window.draw(health_bar_bg);
        health_bar_current.setPosition(sf::Vector2f(x - HERO_SIZE*4,y));
        window.draw(health_bar_current);
        // and xp bars! but futz with location
        set_xp_bar();
        xp_bar_bg.setPosition(sf::Vector2f(x + 1.5*HERO_SIZE,y + 1*HERO_SIZE -
                    xp_for_next_level()/xp_scale));
        window.draw(xp_bar_bg);
        xp_bar_current.setPosition(sf::Vector2f(x + 1.5*HERO_SIZE,y + 1*HERO_SIZE - 
                    xp/xp_scale));
        window.draw(xp_bar_current);
    }

    inline void draw_damage(sf::RenderWindow& window, int damage, int x, int y)
    {
        damage_str.setPosition(x, y);
        set_damage_str(damage);
        if (damage != NO_DAMAGE_DISPLAY)
            window.draw(damage_str);
    }


};

#endif
