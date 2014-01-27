#include "unit.hpp"

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

// Move this unit one step toward its target
void OgreUnit::move_one(){
    sf::Vector2f direction;

    // How far to go
    direction = get_direction();
    // Normalized to 1 unit if we're far
    // A little hacky, we add the position because the 
    // distance method will subtract it.  Stupid
    if (distance<>(direction + get_position()) > 1 ){
        direction = direction / distance<>(direction + get_position());
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

// Fight it out!
// dummy for now, just push both back...or randomly pick a winner?
// Or possibly a draw and both go back, I like it
void OgreUnit::fight(OgreUnit *enemy)
{
    // Could just give new target pos and move a bunch there, or magic move to
    // new retreated position
    sf::Vector2f retreat;

    // Some fight randomness
    int winner = rand() % 3;
    retreat = get_direction<>(enemy->get_position());

    switch (winner)
    {
    case 1: // I win
        enemy->move_by(retreat);
        enemy->set_target_position(enemy->get_position());
        break;
    case 2: // I lose
        move_by(-retreat);
        set_target_position(get_position());
        break;
    default: // case 0: // Draw, both retreat
        // Get the retreat move both of us
        enemy->move_by(retreat);
        enemy->set_target_position(enemy->get_position());
        move_by(-retreat);
        set_target_position(get_position());
    }
}
