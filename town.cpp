#include "town.hpp"

OgreTown::OgreTown(const sf::Vector2f& p, const OgrePlayer player)
	: circ(TOWN_SIZE) // radius of the circle
{
    // Fix to center and Set the initial position
    circ.setOrigin(TOWN_SIZE,TOWN_SIZE);
	circ.setPosition(p);

    // Change this to reflect 'ownership'
	circ.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Give the town some gray walls
    circ.setOutlineColor(sf::Color::Color(127, 127, 127);
    circ.setOutlineThickness(TOWN_SIZE/5);

    owner = player;
}

// Check if town has been captured and set new owner
void OgreTown::check_conquest(list<OgreUnit*> units){
    sf::Vector2f town_position = get_position();

    // TODO: actual player object
    OgrePlayer possible_owner = NO_PLAYER;
    OgrePlayer unit_owner;
    bool contested = false;

    // step for each level of speed
    for(auto unit : units){
        unit_owner = unit->get_owner();
        if (unit->distance<>(town_position) < TOWN_SIZE)
        {
            // Set new possible owner
            if (possible_owner == NO_PLAYER)
                possible_owner = unit_owner;
            // Multiple units, don't change owner
            else if (possible_owner.get_id() != unit_owner.get_id())
                contested = true;
                break;
        }
    }
    if (!contested)
        set_owner(possible_owner);
}

