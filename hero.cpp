#include "hero.hpp"

OgreHero::OgreHero(const int start_level)
{
    // initialize dummy level
    level = start_level;

    // Some stats
    max_hp = BASE_STR*5 + rand() % BASE_STR + level;
    str = BASE_STR + rand() % BASE_STR + level;
    def = BASE_STR + rand() % BASE_STR + level;
    speed = BASE_STR + rand() % BASE_STR + level;
    hp = max_hp;

    // 1 or 2 hits
    total_attacks = 1 + (rand() % 2);

    // A null value of sorts
    damage_taken = -1;

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
    /*
    do
    {
        enemy = *random_element(enemies->begin(), enemies->end());
    }
    while ( enemy->get_hp() == 0);
    */

    // For now, just fight whoever's in front
    for (auto poss_enemy:*enemies)
    {
        if (poss_enemy->get_hp()) //fight someone who's alive
        {
            enemy=poss_enemy;
            break;
        }
    }

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
