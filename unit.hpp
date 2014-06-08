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

#ifndef PLAYER_HPP
#include "player.hpp"
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
#define UNIT_SIZE 10
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

// Points on the shortest path, maybe change to grid pt
class PathPt: public sf::Vector2i{
public:
    // cost to this point
    int G;

    // estimated cost to end
    int F;

    PathPt ();
    PathPt (int x, int y);
    PathPt (sf::Vector2i);
    PathPt (const PathPt& old);

    inline int get_G()
    {
        return G;
    }

    inline void set_G(int newG)
    {
        G = newG;
    }

    inline int get_F()
    {
        return F;
    }

    inline void set_F(int newF)
    {
        F = newF;
    }

    int diag_dist(PathPt *target)
    {
        int del_x = abs(x - target->x);
        int del_y = abs(y - target->y);
        return 4*std::min(del_x, del_y) + 10*std::max(del_x, del_y);
    }

    sf::Vector2f get_as_position(sf::Vector2f ratio)
    {
        return sf::Vector2f(x * ratio.x, y * ratio.y);
    }

    // where did we come from?
    PathPt *parent = nullptr;
};


class OgreUnit{
    // This represents the unit for now
    // Maybe future just for radius collision in future
    sf::CircleShape circ;

    // Where we're headed
    sf::Vector2f target_position;

    // how am I getting there?
    std::list<PathPt*> *path;

    // What's my name (or the leader's name)
    std::string name;

    // Info about the unit
    sf::Text info_str;

    // Speed class, how many steps in a round
    int speed;

    // Who owns this unit
    OgrePlayer *owner;

    // Who's in this unit?
    std::list<OgreHero*> heroes;

public:
    OgreUnit(const sf::Vector2f& p);

    // Fixed for now, later random or based on heroes?
    int unit_type;

    // Who are my heroes?
    inline std::list<OgreHero*> *get_heroes()
    {
        return &heroes;
    }

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
        for (auto hero:heroes)
        {
            hero->set_owner(owner);
        }
        //info.SetColor(owner->get_color());
    }


    // Get the current position of this unit
    // For now the position of the circle
    inline sf::Vector2f get_position() const
    {
        return circ.getPosition();
    }

    inline sf::Vector2f get_target_position() const
    {
        return target_position;
    }

    // Get the direction to travel this step
    // This will change when we have shortest path finding
    inline sf::Vector2f get_direction() const
    {
        return target_position - get_position();
    }

    // Different sense, give me the offset to a given position
    template <class T>
    inline sf::Vector2f get_direction(T position) const
    {
        return position - get_position();
    }

    /*
    inline std::string get_info()
    {
        return info_str.str();
    }
    */

    // Set some information about the unit
    // have to remember to update this after a battle
    inline void set_info(int str, sf::Font *font, int size)
    {
        info_str.setString(std::to_string(str));
        info_str.setFont(*font);
        info_str.setCharacterSize(size);
        info_str.setColor(sf::Color::Black);
    }

    // Post battle update
    inline void set_info(int str)
    {
        info_str.setString(std::to_string(str));
    }

    // Shout my name!
    inline std::string get_name()
    {
        return name;
    }

    // What's my name?
    inline void set_name(std::string unit_name)
    {
        name = unit_name;
    }

    // How big am I?
    inline float get_size() const
    {
        return circ.getRadius();
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

    inline void collect_pay()
    {
        for (auto hero: heroes)
        {
            hero->collect_pay(get_owner());
        }
    }

    inline void heal()
    {
        for (auto hero: heroes)
        {
            hero->set_hp(hero->get_hp() + 1);
        }
    }

    // Draw a ring around the unit to show it's selected
    inline void set_select_state(bool select)
    {
        if (select)
        {
            circ.setOutlineColor(sf::Color::Green);
            circ.setOutlineThickness(UNIT_SIZE/5);
        }
        else
        {
            circ.setOutlineThickness(0);
        }
    }

    // Set the speed in units/time step
    void set_speed(int s)
    {
        speed = s;
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
        circ.setOutlineThickness(0);
    }


    // Move this unit one step toward its target
    void move_one();

    // Get our next position to move to
    void next_target(sf::Vector2f ratio);

    // Move this unit its speed toward its target
    void move_speed(sf::Vector2f ratio);

    // Fight it out!
    void fight(OgreUnit *enemy);

    // Picture a battle
    void fight_draw_on(sf::RenderWindow& window);

    inline void draw_on(sf::RenderWindow& window)
    {
        window.draw(circ);
        info_str.setPosition(get_position());
        window.draw(info_str);
    }

    // Super simple distance to a point
    // stupid c++ templates in methods
    template <class T>
    float distance(T v)
    {
        sf::Vector2f temp;
        temp = sf::Vector2f(v) - get_position();
        return sqrt(temp.x * temp.x + temp.y * temp.y);
    }

    // Set where we want to end up, locally
    // We set the target with integer coordinates, but we move with float
    template <class T>
    void set_target_position(T p)
    {
        // round to nearest ten
        target_position = sf::Vector2f(round(p.x/10)*10, round(p.y/10)*10);
    }

    template <class T>
    void set_target_tile(T p, T ratio,
            std::vector<std::vector<int>> terrain,
            std::vector< std::vector<int> > move_cost)
    {
        PathPt target = PathPt(round(p.x/ratio.x), round(p.y/ratio.y));
        short_path(terrain, move_cost, &target, ratio);
        // maybe I will allow this, but not right now
        assert(!path->empty());
        set_target_position(path->front()->get_as_position(ratio));
    }

    // Magic move after combat or other necessary things
    template <class T>
    void move_by(T p)
    {
        circ.move(p);
    }

    // shortest path computation prototype
    void short_path(std::vector<std::vector<int>> terrain, std::vector< std::vector<int> > move_cost, PathPt *target, sf::Vector2f ratio);
};

#endif
