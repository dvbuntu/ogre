#ifndef UNIT_HPP
#define UNIT_HPP

#include <cstdlib>
#include <cfloat>
#include <SFML/Graphics.hpp>

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 100
#endif

class Unit{
    // This represents the unit for now
    // Maybe future just for radius collision in future
    sf::RectangleShape rect;

    // Where we're headed
    sf::Vector2f target_position;

    char[MAX_NAME_LENGTH];

public:
    Unit(const sf::VEctor2f& p);

    // Get the current position of this unit
    // For now the position of the rectangle
    inline sf::Vector2f Unit::get_position() const
    {
        return rect.getPosition();
    }

    void Unit::set_target_position(sf::Vector2f& p)
    {
        target_position = p;
    }



Unit::Unit(const sf::Vector2f& p)
	: rect(sf::Vector2f(2, 2)) // dimensions of the box
{
    // Set the initial position
	rect.setPosition(p);
	rect.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Start not moving, so target is our current position
    set_target_position(p);


}


// Get the direction to travel this step
// This will change when we have shortest path finding
sf::Vector2f Unit::get_direction() {
    return target_position - get_position();
}

// Move this unit one step toward its target
void Unit::move_one(){
    sf::Vector2f direction;

    // How far to go
    direction = get_direction();
    // Normalized to 1 unit if we're far
    if (length(direction) > 1 ){
        direction = direction / length(direction);
    }
    rect.move(direction);
}

// Move this unit its speed toward its target
void Unit::move_speed(){
    int i;

    // step for each level of speed
    for(i = 0; i < speed, i++) { 
        move_one();
    }
}

