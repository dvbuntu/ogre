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

    // Starting strength...I'll make it random
    str = 50 + rand() % 50;

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

    int player_attacks = 0;
    int enemy_attacks = 0;

    OgreHero *attacker;
    OgreUnit *defenders;

    // compute how many total attacks we'll be doing
    for (auto hero:heroes)
    {
        player_attacks += hero->get_total_attacks();
    }

    for (auto hero:(enemy->heroes))
    {
        enemy_attacks += hero->get_total_attacks();
    }

    // Fight until we can't fight no more
    while( player_attacks + enemy_attacks > 0)
    {

        if (rand() % 2 + 1 == PLAYER)
        {
            attacker = *random_element(heroes.begin(), heroes.end());
            defenders = enemy;
        }
        else
        {
            attacker = *random_element(enemy->heroes.begin(), enemy->heroes.end());
            defenders = player;
        }

    // Some fight randomness
    int result = get_str() - enemy->get_str() + (rand() % 20) - 10;

    // Get the retreat 
    retreat = get_direction<>(enemy->get_position());
    retreat += retreat; // a little more distance between

    if (result > 0)// I win
    {
        enemy->move_by(retreat);
        enemy->set_target_position(enemy->get_position());
    }
    else if (result < 0) // I lose
    {
        move_by(-retreat);
        set_target_position(get_position());
    }
    else// Draw, both retreat
    {
        // move both of us
        enemy->move_by(retreat);
        enemy->set_target_position(enemy->get_position());
        move_by(-retreat);
        set_target_position(get_position());
    }

    // Both of us take a beating
    enemy->set_str(enemy->get_str() - (rand() % 10));
    enemy->set_info(enemy->get_str());
    enemy->update_cost();
    set_str(get_str() - (rand() % 10));
    set_info(get_str());
    update_cost();
}
