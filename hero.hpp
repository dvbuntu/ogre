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

#ifndef NO_DAMAGE_DISPLAY
#define NO_DAMAGE_DISPLAY -1
#endif

class OgreHero{
    // This represents the hero, an individual within a unit

    // my combat representation
    sf::CircleShape circ;

    // What's my name
    std::string name;

    // Info about the hero
    sf::Text info_str;
    sf::Text damage_str;

    // Speed class, how many steps in a round
    int speed;

    // Max hit points
    int max_hp;

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

    // Hero cost
    int cost;

    // Position within the unit (TODO: make this affect damage and num attacks)
    int position;

    // health bar background and current hp
    sf::RectangleShape health_bar_bg;
    sf::RectangleShape health_bar_current;

    // How hard did we just get hit?
    int damage_taken;

    // Are we the current attacker or defender?
    bool attacking;
    bool defending;

    // Who owns this hero
    OgrePlayer *owner;

public:
    OgreHero(const int start_level);

    // Who is my general?
    inline OgrePlayer *get_owner()
    {
        return owner;
    }

    // Set my general, Maybe roll this into constructor
    inline void set_owner(OgrePlayer *player)
    {
        owner = player;
        circ.setFillColor(owner->get_color());
    }

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

    // TODO: link to unit's get_life_color
    inline sf::Color get_life_color(int str)
    {
        int red, green;
        float life_fraction = str/(float)get_max_hp();
        if (life_fraction > 0.5) {

            red = int(2*(255-255*life_fraction));
            green = 255;
        }
        else {
            red = 255;
            green = int(255*life_fraction*2);
        }
        return sf::Color(red,green,0);
    }

    // Set some information about the hero
    // have to remember to update this after a battle
    inline void set_info(int str, sf::Font *font, int size)
    {
        info_str.setString(std::to_string(str));
        info_str.setFont(*font);
        info_str.setCharacterSize(size);
        info_str.setColor(sf::Color::Black);
        health_bar_bg.setOutlineThickness(1);
        health_bar_bg.setSize(sf::Vector2f(get_max_hp()/2.0, 10));
        health_bar_bg.setOutlineColor(sf::Color(125,125,125));
        health_bar_current.setScale(str/2.0, 1);
        health_bar_current.setFillColor(get_life_color(str));
    }

    // Post battle update
    inline void set_info(int str)
    {
        info_str.setString(std::to_string(str));
        health_bar_bg.setOutlineThickness(1);
        health_bar_bg.setSize(sf::Vector2f(get_max_hp()/2.0, 10));
        health_bar_bg.setOutlineColor(sf::Color(125,125,125));
        health_bar_current.setScale(str/2.0, 1);
        health_bar_current.setFillColor(get_life_color(str));
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

    // Shout my name!
    inline std::string get_name()
    {
        return name;
    }

    // What's my name?
    inline void set_name(std::string hero_name)
    {
        name = hero_name;
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

    // How many hit points could I have?
    inline int get_max_hp() const
    {
        return max_hp;
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
