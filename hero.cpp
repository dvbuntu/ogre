#include "hero.hpp"

OgreHero::OgreHero(const int start_level)
{
    // initialize dummy level
    level = start_level;

    // Some stats
    max_hp = 50 + rand() % 50 + level;
    str = 50 + rand() % 50 + level;
    def = 50 + rand() % 50 + level;
    speed = 50 + rand() % 50 + level;
    hp = max_hp;

    // 1 or 2 hits
    total_attacks = 1 + rand() % 2;

}

// Fight it out!
int OgreHero::attack(std::list<OgreHero*> *enemies)
{
    OgreHero *enemy;
    int damage;

    // TODO: pick someone to fight, but now, just do random
    // should pick someone who still has hit points...
    // Can we even hit this unit?
    do
    {
        enemy = *random_element(enemies->begin(), enemies->end());
    }
    while ( enemy->get_hp() == 0)

    // How hard do I hit?
    // TODO: Miss chance based on speed comparison
    // Lots of conditionals for front vs back row
    // damage type vs damage defense
    damage = get_str()/2 - enemy->get_def() + (rand() % get_str());
    if (damage < 0)
        damage = 0;

    // Dish it out
    enemy->set_hp(enemy->get_hp() - damage);
    set_attacks_left(get_attacks_left() - 1);

    return damage;
}
