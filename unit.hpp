#ifndef UNIT_HPP
#define UNIT_HPP

#include <cstdlib>
#include <cfloat>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <math.h>

#ifndef PLAYER_HPP
#include "player.hpp"
#endif

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 100
#endif

#ifndef UNIT_SIZE
#define UNIT_SIZE 10
#endif

class OgreUnit{
    // This represents the unit for now
    // Maybe future just for radius collision in future
    sf::CircleShape circ;

    // Where we're headed
    sf::Vector2f target_position;

    // What's my name (or the leader's name)
    std::string name;

    // Info about the unit
    std::stringstream info_str;

    // Speed class, how many steps in a round
    int speed;

    // Simple Fight Strength parameter
    int str;

    // Who owns this unit
    OgrePlayer *owner;

public:
    OgreUnit(const sf::Vector2f& p);

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

    inline std::string get_info()
    {
        return info_str.str();
    }

    // Set some information about the unit
    // have to remember to update this after a battle
    inline void set_info(int str, sf::Font font, int size)
    {
        info_str << str;
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

    // How strong am I?
    inline int get_str() const
    {
        return str;
    }

    inline void set_str(int new_str)
    {
        if (new_str > 0)
            str = new_str;
        else
            str = 0;
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


    // Move this unit one step toward its target
    void move_one();

    // Move this unit its speed toward its target
    void move_speed();

    // Fight it out!
    void fight(OgreUnit *enemy);

    inline void draw_on(sf::RenderWindow& window)
    {
        window.draw(circ);
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

    // Set where we want to end up
    // We set the target with integer coordinates, but we move with float
    template <class T>
    void set_target_position(T p)
    {
        target_position = sf::Vector2f(p);
    }

    // Magic move after combat or other necessary things
    template <class T>
    void move_by(T p)
    {
        circ.move(p);
    }
};

#endif
