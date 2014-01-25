#include "town.hpp"

OgreTown::OgreTown(const sf::Vector2f& p)
	: circ(TOWN_SIZE) // radius of the circle
{
    // Fix to center and Set the initial position
    circ.setOrigin(TOWN_SIZE,TOWN_SIZE);
	circ.setPosition(p);

    // Change this to reflect 'ownership'
	circ.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Give the town some gray walls
    circ.setOutlineColor(sf::Color(127, 127, 127));
    circ.setOutlineThickness(TOWN_SIZE/5);
}

// Check if town has been captured and set new owner
// Maybe change to unit checks when it stops moving
void OgreTown::check_conquest(std::list<OgreUnit*> units){
    sf::Vector2f town_position = get_position();

    OgrePlayer *possible_owner = nullptr;
    OgrePlayer *unit_owner;
    bool contested = false;

    // step for each level of speed
    for(auto unit : units){
        unit_owner = unit->get_owner();
        if (unit->distance<>(town_position) < TOWN_SIZE)
        {
            // Set new possible owner
            if (possible_owner == nullptr)
                possible_owner = unit_owner;
            // Multiple units, don't change owner
            // just comparing pointers, not actual ids...
            else if (possible_owner == unit_owner)
                contested = true;
                break;
        }
    }
    if (!contested && possible_owner != nullptr)
        set_owner(possible_owner);
}

