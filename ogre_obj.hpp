#ifndef OGRE_OBJ_HPP
#define OGRE_OBJ_HPP

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

#ifndef OBJECT_SIZE
#define OBJECT_SIZE 10
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

class OgreObject{
public:
    // This represents a generic object
    // Maybe future just for radius collision in future
    sf::CircleShape circ;

    // what do I look like?
    sf::Texture picture;

    // Here's my 5x10 glossy
    sf::Sprite sprite;

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

    // How tough can I be?
    int max_hp;

    // because I use it to move and I'm a horrible person
    int unit_type;

    // Who owns this unit
    OgrePlayer *owner;

    // How healthy am I?
    sf::Texture health_bar;

    // Here's my 5x10 glossy
    sf::Sprite health_bar_sprite;

    // health bar background and current hp
    sf::RectangleShape health_bar_bg;
    sf::RectangleShape health_bar_current;


    OgreObject(const sf::Vector2f& p);

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
    // Set some information about the object
    // have to remember to update this after a battle
    inline void set_info(int str, sf::Font *font, int size)
    {
        info_str.setString(std::to_string(str));
        info_str.setFont(*font);
        info_str.setCharacterSize(size);
        info_str.setColor(sf::Color::Black);
        //Would like the position to be outside the unit, but this doesn't seem to match...could also do
        //a health bar
        //info_str.setPosition(circ.getPosition() + sf::Vector2f(2*circ.getRadius(),2*circ.getRadius()));
    }

    // Post battle update
    inline void set_info(int str)
    {
        info_str.setString(std::to_string(str));
    }
*/

    // Shout my name!
    inline std::string get_name()
    {
        return name;
    }

    // What's my name?
    inline void set_name(std::string object_name)
    {
        name = object_name;
    }

    // How big am I?
    inline float get_size() const
    {
        return circ.getRadius();
    }

    // Draw a ring around the object to show it's selected
    inline void set_select_state(bool select)
    {
        if (select)
        {
            circ.setOutlineColor(sf::Color::Green);
            circ.setOutlineThickness(OBJECT_SIZE/5);
        }
        else
        {
            circ.setOutlineColor(sf::Color::Black);
            circ.setOutlineThickness(OBJECT_SIZE/10);
        }
    }

    // Set the speed in objects/time step
    void set_speed(int s)
    {
        speed = s;
    }

    // Move this object one step toward its target
    void move_one(int move_tile_cost);

    // Get our next position to move to
    void next_target(sf::Vector2f ratio);

    // Move this object its speed toward its target
    void move_speed(sf::Vector2f ratio,std::vector<std::vector<int>> terrain, std::vector< std::vector<int> > move_cost);

    inline void draw_on(sf::RenderWindow& window)
    {
        window.draw(circ);
        window.draw(sprite);
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
        sprite.move(p);
    }

    // Just a dummy
    inline int get_max_hp()
    {
        return max_hp;
    }

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

    // Set some information about the object
    inline void set_info(int str, sf::Font *font, int size)
    {
        info_str.setString(std::to_string(str));
        info_str.setFont(*font);
        info_str.setCharacterSize(size);
        info_str.setColor(sf::Color::Black);
        health_bar_bg.setSize(sf::Vector2f(get_max_hp()/3.0, 10));
        health_bar_bg.setOutlineColor(sf::Color(125,125,125));
        health_bar_bg.setOutlineThickness(1);
        health_bar_current.setScale(str/3.0, 1);
        health_bar_current.setFillColor(get_life_color(str));
    }

    // Post battle update
    inline void set_info(int str)
    {
        info_str.setString(std::to_string(str));
        health_bar_bg.setSize(sf::Vector2f(get_max_hp()/3.0, 10));
        health_bar_bg.setOutlineColor(sf::Color(125,125,125));
        health_bar_bg.setOutlineThickness(1);
        health_bar_current.setScale(str/3.0, 1);
        health_bar_current.setFillColor(get_life_color(str));
    }

    // shortest path computation prototype
    void short_path(std::vector<std::vector<int>> terrain, std::vector< std::vector<int> > move_cost, PathPt *target, sf::Vector2f ratio);
};


#endif
