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
    // in future, change to next step of shortest path
    // compute that when we set the target position
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


// A* for shortest path to target
// as I take in a path, maybe can issue multiple set
// of move orders?  just change the start to the previous
// target and tack on the new target...hmm
// start and target are positions...sfml vector2i?
void OgreUnit::short_path(int **terrain, int **move_cost, PathPt *start, PathPt target, PathPt *path)
{
    // roll my own special sauce, basically a vector2i plus G and F
    PathPt *prev_pt, *current;

    // new computed cost
    int newG;

    start->set_G(0);
    start->set_F(diag_dist(start,target));
    current = start;

    std::list<PathPt*> open_list;
    open_list.push_front(start);

    std::list<PathPt*> closed_list;

    // This will hold the actual adjacent pts, computed from
    // the current pt and the steps
    std::list<PathPt*> nearby_pts;

    // These are the directions we will step, all 8 diagonals
    // Maybe I don't actually want to do diagonals...
    std::list<PathPt> steps;
    steps.push_front(new PathPt(0,1));
    steps.push_front(new PathPt(1,1));
    steps.push_front(new PathPt(1,0));
    steps.push_front(new PathPt(1,-1));
    steps.push_front(new PathPt(0,-1));
    steps.push_front(new PathPt(-1,-1));
    steps.push_front(new PathPt(-1,0));
    steps.push_front(new PathPt(-1,1));

    // keep going while there's things in open list and our target isn't in the closed list
    while(!open_list.empty() and
        std::find(closed_list.begin(),
                  closed_list.end(),
                  target) == closed_list.end())
    {
        open_list.erase(current);
        closed_list.push_front(current);
        nearby_pts.clear();
        for (auto step : steps)
        {
            nearby_pts.push_front(new PathPt( *current + step));
        }

        for (auto adj : nearby_pts)
        {
            if(std::find(closed_list.begin(),
                  closed_list.end(),
                  adj) != closed_list.end())
                continue;
            newG = current.G + move_cost[unit_type][terrain[adj->X][adj->Y]];
            if(std::find(open_list.begin(),
                  open_list.end(),
                  adj) == open_list.end())
            {
                open_list.push_front(adj)
                adj->set_G(newG);
            }
            if(newG <= adj->get_G())
            {
                adj->set_G(newG);
                adj->set_F(adj->get_G() + diag_dist(adj,target));
                adj.parent = current; //ptr to current
                open_list.sort(compare_F); //sort by F values
            }
        }
        current = open_list.front(); // whichever with the lowest F
    }
    prev_pt = target;
    while(prev_pt != nullptr)
    {
        path->push_front(prev_pt);
        prev_pt = prev_pt.parent;
    }
}


