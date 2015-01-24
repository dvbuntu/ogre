#include "town.hpp"

OgreTown::OgreTown(const sf::Vector2f& p)
	: OgreObject(p) // radius of the circle
{
    // Fix to center and Set the initial position
    circ.setRadius(TOWN_SIZE);
    circ.setOrigin(TOWN_SIZE,TOWN_SIZE);
	circ.setPosition(p);

    // Change this to reflect 'ownership'
	circ.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Give the town some gray walls
    circ.setOutlineColor(sf::Color(127, 127, 127));
    circ.setOutlineThickness(TOWN_SIZE/5);

    if (!picture.loadFromFile("resources/walled_town.jpg"))
    {
        std::cout << "Failed to load image!" << std::endl;
    }

    // put on a pretty face
    sprite.setTexture(picture);
    // scale the face to our size
    sprite.setScale(sf::Vector2f(TOWN_SIZE/((float)picture.getSize().x),
                TOWN_SIZE/((float)picture.getSize().y)));
    // put it where we are (center on center)
	sprite.setPosition(p);
	sprite.move(sf::Vector2f(-0.5*circ.getRadius(), -0.5*circ.getRadius()));
    sprite.setColor(circ.getFillColor());

    // Town's have health bars too!
    health_bar_bg.setSize(sf::Vector2f(1,10));
    health_bar_bg.setFillColor(sf::Color(0,0,0));
    health_bar_bg.setPosition(p - sf::Vector2f(0,3*TOWN_SIZE));

    health_bar_current.setSize(sf::Vector2f(1,10));
    health_bar_current.setScale(1,1);
    health_bar_current.setFillColor(sf::Color(0,255,0));
    health_bar_current.setPosition(p - sf::Vector2f(0,3*TOWN_SIZE));

    scale = TOWN_HEALTH_SCALE;

    // Get a tax_base
    payout = 1 + std::rand() % 6;

    // How sturdy is this town?
    max_hp = 1 + std::rand() % (TOWN_MAX_HP*payout);
    hp = max_hp;

    // Better town's are more circular, naturally
    circ.setPointCount(payout + 3);
}

// Check if town has been captured and set new owner
// Only report if there's a change of ownership
// Maybe change to unit checks when it stops moving
OgrePlayer *OgreTown::check_conquest(std::list<OgreUnit*> units){
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
    // set new owner and damage town
    if (!contested && possible_owner != nullptr && possible_owner != get_owner())
    {
        set_owner(possible_owner);
        pillage();
        set_info(hp);
        return possible_owner;
    }
    else
        return nullptr;
}

