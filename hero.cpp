#include "hero.hpp"

OgreHero::OgreHero(const int start_level)
    : OgreObject(sf::Vector2f(0,0))
{
    circ.setRadius(HERO_SIZE);
    // initialize dummy level
    level = start_level;

    // we're probably not the leader
    leader = false;

    // we might have a little experience already
    xp = rand() % (level+1);

    // Stronger heroes are more like circles
    circ.setPointCount(level + 3);

    // Some stats
    // maybe distribute 'points' to these categories...
    max_hp = BASE_STR*4 + rand() % BASE_STR + level;
    str = BASE_STR + rand() % BASE_STR + level;
    def = BASE_STR/2 + level;
    speed = BASE_STR + rand() % BASE_STR + level;
    hp = max_hp;
    vision = HERO_SIZE*(level + 1);

    // 1 or 2 hits
    total_attacks = 1 + (rand() % 2);

    // A null value of sorts
    damage_taken = NO_DAMAGE_DISPLAY;

    // Give this grunt a picture and a club!
    // here's my portrait!
    if (!picture.loadFromFile("resources/grunt.png"))
    {
        std::cout << "Failed to load image!" << std::endl;
    }

    profile.setTexture(picture);
    profile.setColor(circ.getFillColor());


    // setup health bar stuff
    health_bar_bg.setSize(sf::Vector2f(1,10));
    health_bar_bg.setFillColor(sf::Color(0,0,0));
    //health_bar_bg.setPosition(p - sf::Vector2f(0,3*HERO_SIZE));

    health_bar_current.setSize(sf::Vector2f(1,10));
    health_bar_current.setScale(1,1);
    health_bar_current.setFillColor(sf::Color(0,255,0));
    //health_bar_current.setPosition(p - sf::Vector2f(0,3*HERO_SIZE));

    // setup xp bar
    xp_bar_bg.setSize(sf::Vector2f(5,HERO_SIZE));
    xp_bar_bg.setFillColor(sf::Color(0,0,0));
    xp_bar_current.setSize(sf::Vector2f(5,HERO_SIZE));
    xp_bar_current.setScale(1,1);
    xp_bar_current.setFillColor(sf::Color::Yellow);

    scale = HERO_HEALTH_SCALE;
    xp_scale = HERO_XP_SCALE;

    // We don't do anything yet
    attacking = false;
    defending = false;

}

// Fight it out!
OgreHero * OgreHero::attack(std::list<OgreHero*> *enemies)
{
    OgreHero *enemy = nullptr;

    // If there's no enemies to fight, return zero damage
    int damage = 0;
    //int iter = 0; // TODO only cycle through all units once...

    // TODO: pick someone to fight, but now, just do random
    // should pick someone who still has hit points...
    // Can we even hit this unit?
    do
    {
        enemy = *random_element(enemies->begin(), enemies->end());
    }
    while ( enemy->get_hp() == 0);

    // For now, just fight whoever's in front
    /*
    for (auto poss_enemy:*enemies)
    {
        if (poss_enemy->get_hp()) //fight someone who's alive
        {
            enemy=poss_enemy;
            break;
        }
    }
    */

    // TODO: if a unit is dead, end the fight

    // How hard do I hit?
    // TODO: Miss chance based on speed comparison
    // Lots of conditionals for front vs back row
    // damage type vs damage defense
    if (enemy != nullptr)
    {
        damage = get_str()/2 - enemy->get_def() + (rand() % get_str());
        if (damage < 0)
            damage = 0;

        // can't hurt them for more than they have
        damage = std::min(damage, enemy->get_hp());

        // Dish it out
        enemy->set_hp(enemy->get_hp() - damage);
        set_attacks_left(get_attacks_left() - 1);

        enemy->set_damage_taken(damage);
    }

    return enemy;
}
