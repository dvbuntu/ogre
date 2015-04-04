#include "unit.hpp"

// Is there a way to have most of this stuff set already and just do the special stuff?
OgreUnit::OgreUnit(const sf::Vector2f& p)
	: OgreObject(p)
{
    // Fix to center and Set the initial position
    circ.setRadius(UNIT_SIZE);
    circ.setOrigin(UNIT_SIZE,UNIT_SIZE);
	circ.setPosition(p);
	circ.setFillColor(sf::Color(std::rand() % 128 + 128, std::rand() % 128 + 128, std::rand() % 128 + 128));
    
    // here's my portrait!
    if (!picture.loadFromFile("resources/heroes.png"))
    {
        std::cout << "Failed to load image!" << std::endl;
    }

    unit_type = std::rand() % 4;
    circ.setPointCount(unit_type + 3);

    sprite.setTexture(picture);
    sprite.setTextureRect(sf::IntRect(0, SPRITE_SIZEY * unit_type, SPRITE_SIZEX, SPRITE_SIZEY));
    sprite.setScale(sf::Vector2f(SPRITE_SCALE*UNIT_SIZE/((float)SPRITE_SIZEX),
                                 SPRITE_SCALE*UNIT_SIZE/((float)SPRITE_SIZEX)));
    sprite.setPosition(p - sf::Vector2f(UNIT_SIZE,UNIT_SIZE*1.5));
	sprite.move(sf::Vector2f(-0.75*circ.getRadius(), -0.75*circ.getRadius()));
    sprite.setColor(circ.getFillColor());

    // Start not moving, so target is our current position
    // Apparently you're not allowed to use a method to do this during init
    target_position = p;
    target_flag.setSize(sf::Vector2f(UNIT_SIZE,UNIT_SIZE));
    target_flag.setOrigin(UNIT_SIZE/2,UNIT_SIZE/2);
    target_flag.setOutlineThickness(1);
    target_flag.setOutlineColor(sf::Color(128,128,128));

    path = new std::list<PathPt*>;

    // Starting strength...I'll make it random
    //str = 50 + rand() % 50;

    // Recruit some fighters
//    int max_unit_hp = 0;
    vision_aura.setRadius(0);
    for (int i = 0; i < HEROES_IN_UNIT; i++)
    {
        heroes.push_front(new OgreHero(rand() % 10));
        heroes.front()->set_position(i);
//        max_unit_hp += heroes.front()->get_hp();
        vision_aura.setRadius(std::max(vision_aura.getRadius(),
                    (float) heroes.front()->get_vision()));
    }

    // make one unit a leader, a give him the same picture...bugged out?
    hero_leader = *random_element(heroes.begin(), heroes.end());
    hero_leader->set_leader(true);
    hero_leader->get_sprite()->setTexture(picture);
    hero_leader->get_sprite()->setTextureRect(sf::IntRect(0, SPRITE_SIZEY * unit_type, SPRITE_SIZEX, SPRITE_SIZEY));
    hero_leader->get_sprite()->setScale(sf::Vector2f(SPRITE_SCALE*UNIT_SIZE/((float)SPRITE_SIZEX),
                                                        SPRITE_SCALE*UNIT_SIZE/((float)SPRITE_SIZEX)));
    hero_leader->get_sprite()->setColor(circ.getFillColor());

    vision_aura.setOrigin(vision_aura.getRadius(), vision_aura.getRadius());
    vision_aura.setPosition(p);
    //vision_aura.move(-1*vision_aura.getRadius(), -1*vision_aura.getRadius());
    vision_aura.setFillColor(sf::Color::Transparent);
    vision_aura.setOutlineThickness(1);
    vision_aura.setOutlineColor(sf::Color(127,127,127,127));

    health_bar_bg.setSize(sf::Vector2f(1,10));
    health_bar_bg.setFillColor(sf::Color(0,0,0));
    health_bar_bg.setPosition(p - sf::Vector2f(0,3*UNIT_SIZE));

    health_bar_current.setSize(sf::Vector2f(1,10));
    health_bar_current.setScale(1,1);
    health_bar_current.setFillColor(sf::Color(0,255,0));
    health_bar_current.setPosition(p - sf::Vector2f(0,3*UNIT_SIZE));

    scale = UNIT_HEALTH_SCALE;

    select_state = false;
}

// Fight it out!
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
    while( get_remaining_attacks() + enemy->get_remaining_attacks() > 0 && iter < 30 && enemy->get_hp() > 0 && get_hp())
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
            defender->set_damage_taken(NO_DAMAGE_DISPLAY);

            // Sleep for a second?
            usleep(BATTLE_DELAY);
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

    // Gain XP
    for (auto hero:*(get_heroes()))
    {
        hero->gain_xp(enemy->get_heroes());
    }
    for (auto hero:*(enemy->get_heroes()))
    {
        hero->gain_xp(get_heroes());
    }

    // Gain a level
    for (auto hero:*(get_heroes()))
    {
        hero->level_up();
    }
    for (auto hero:*(enemy->get_heroes()))
    {
        hero->level_up();
    }




}

void OgreUnit::fight_draw_on(sf::RenderWindow& window)
{
    int offset = 100; // TODO: scale to size of window

    if (get_owner()->get_id() == PLAYER)
    {
        offset += 200; // TODO: scale
    }

    // draw the leader sprite


    for (auto hero: *(get_heroes()))
    {
        // only show up if we're actually there
        if (hero->get_hp() != 0)
            hero->draw_at(window, offset, hero->get_position()*5*HERO_SIZE + 5*HERO_SIZE);
    }
}



