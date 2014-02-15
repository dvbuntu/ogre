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
    //str = 50 + rand() % 50;

    // Recruit some fighters
    for (int i = 0; i < HEROES_IN_UNIT; i++)
    {
        heroes.push_front(new OgreHero(rand() % 10));
        heroes.front()->set_position(i);
    }

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

    int iter = 0;
    int damage = 0;
    int damage_coefficient = 0;

    OgreHero *attacker;
    OgreHero *defender;
    OgreUnit *defending_unit;

    //make a new window for our battle
    // I think I can just advance this section without messing with main window
    sf::RenderWindow window(sf::VideoMode(400, 300), "Fight It Out!");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(120);
    sf::View view = window.getDefaultView();

    // load font
    sf::Font font;
    if (!font.loadFromFile("./resources/DejaVuSans.ttf"))
    {
        std::cerr << "Couldn't find font DejaVuSans.ttf!\n";
        return;
    }

    // hero setup
    for (auto hero:*(get_heroes()))
    {
        hero->set_attacks_left(hero->get_total_attacks());
        hero->set_attacking(false);
        hero->set_defending(false);
        hero->set_info(hero->get_hp(), &font, 12);
        hero->set_damage_str(0, &font, 12);
    }
    for (auto hero:*(enemy->get_heroes()))
    {
        hero->set_attacks_left(hero->get_total_attacks());
        hero->set_attacking(false);
        hero->set_defending(false);
        hero->set_info(hero->get_hp(), &font, 12);
        hero->set_damage_str(0, &font, 12);
    }

    // Fight until we can't fight no more
    // Need to check for units that die that still have attacks left
    // Have a failsafe max_iter for now
    while( get_remaining_attacks() + enemy->get_remaining_attacks() > 0 && iter < 1000 )
    {
        iter++;
        if (rand() % 2 + 1 == PLAYER)
        {
            attacker = *random_element(heroes.begin(), heroes.end());
            defending_unit = enemy;
            damage_coefficient = 1;
        }
        else
        {
            attacker = *random_element(enemy->heroes.begin(), enemy->heroes.end());
            defending_unit = this;
            damage_coefficient = -1;
        }

        // Can we even attack?
        if (attacker->get_attacks_left() == 0 || attacker->get_hp() == 0)
            continue;

        // Need to return who we attacked...
        // return that instead, and attach the damage taken to them
        // reset after this
        defender = attacker->attack(defending_unit->get_heroes());
        if (defender != nullptr)
        {
            damage += defender->get_damage_taken() * damage_coefficient;

            // Set the status to be picked up in the drawing
            attacker->set_attacking(true);
            defender->set_defending(true);

            // Draw everything
            window.clear(sf::Color::White);
            enemy->fight_draw_on(window);
            fight_draw_on(window);
            window.display();

            // clean up status
            attacker->set_attacking(false);
            defender->set_defending(false);
            defender->set_damage_taken(0);

            // Sleep for a second?
            sleep(BATTLE_DELAY);
        }
    }

    window.close();

    // Get the retreat 
    // TODO: fix this so it won't send me in wacky direction when use shortest path
    retreat = get_direction<>(enemy->get_position());
    retreat += retreat; // a little more distance between

    if (damage > 0)// I win
    {
        enemy->move_by(retreat);
        enemy->set_target_position(enemy->get_position());
    }
    else if (damage < 0) // I lose
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

    // Update the health displays
    enemy->set_info(enemy->get_hp());
    set_info(get_hp());
}

void OgreUnit::fight_draw_on(sf::RenderWindow& window)
{
    int offset = 100; // TODO: scale to size of window

    if (get_owner()->get_id() == PLAYER)
    {
        offset += 200; // TODO: scale
    }

    for (auto hero: *(get_heroes()))
    {
        // only show up if we're actually there
        if (hero->get_hp() != 0)
            hero->draw_at(window, offset, hero->get_position()*5*HERO_SIZE + 5*HERO_SIZE);
    }
}
