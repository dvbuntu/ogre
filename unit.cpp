#include "unit.hpp"

#ifndef UNIT_SIZE
#define UNIT_SIZE 10
#endif

OgreUnit::OgreUnit(const sf::Vector2f& p)
	: circ(UNIT_SIZE) // radius of the circle
{
    // Fix to center and Set the initial position
    circ.setOrigin(UNIT_SIZE,UNIT_SIZE);
	circ.setPosition(p);
	circ.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Start not moving, so target is our current position
    // Apparently you're not allowed to use a method to do this during init
    target_position = p;

}

// Set where we want to end up
// We set the target with integer coordinates, but we move with float
void OgreUnit::set_target_position(sf::Vector2i& p)
{
	target_position = sf::Vector2f(p);
}

// Move this unit one step toward its target
void OgreUnit::move_one(){
    sf::Vector2f direction;

    // How far to go
    direction = get_direction();
    // Normalized to 1 unit if we're far
    if (length(direction) > 1 ){
        direction = direction / length(direction);
    }
    circ.move(direction);
}

// Move this unit its speed toward its target
void OgreUnit::move_speed(){
    int i;

    // step for each level of speed
    for(i = 0; i < speed; i++) {
        move_one();
    }
}

