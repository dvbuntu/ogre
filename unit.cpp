#include "unit.hpp"

PathPt::PathPt (int x, int y) : sf::Vector2i(x, y){}
PathPt::PathPt (sf::Vector2i vec) : sf::Vector2i(vec){}
PathPt::PathPt (const PathPt& old) : sf::Vector2i(old.x, old.y){
        set_G(old.G);
        set_F(old.F);
}

// Need this as a function for sorting and shouldn't be in helper
bool compare_F(PathPt *first, PathPt *second)
{
    return first->get_F() < second->get_F();
}

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

    path = new std::list<PathPt*>;

    // Starting strength...I'll make it random
    //str = 50 + rand() % 50;

    unit_type = std::rand() % 4;
    circ.setPointCount(unit_type + 2);

    // Recruit some fighters
    for (int i = 0; i < HEROES_IN_UNIT; i++)
    {
        heroes.push_front(new OgreHero(rand() % 10));
        heroes.front()->set_position(i);
    }

}

// Move this unit one step toward its target
void OgreUnit::move_one(int tile_move_cost){
    sf::Vector2f direction;

    float cost = float(tile_move_cost);
    // How far to go
    // in future, change to next step of shortest path
    // compute that when we set the target position
    direction = get_direction();
    // Normalized to 1 unit if we're far
    // A little hacky, we add the position because the 
    // distance method will subtract it.  Stupid
    if (distance<>(direction + get_position()) > 1/cost ){
        direction = direction /distance<>(direction + get_position());
        // change to move distance inversely proportional to move_cost
        circ.move(direction/cost);
    }
    else
    {
        circ.move(direction);
    }
}

// Figure out the next target position based on path
void OgreUnit::next_target(sf::Vector2f ratio)
{

    // get our new target, or sit tight
    if (!path->empty())
    {
        // advance to next path point if reached previous
        if (get_position() == get_target_position())
        {
            path->pop_front();
            if (!path->empty())
                set_target_position(path->front()->get_as_position(ratio));
        }
    }
    else
    {
        set_target_position(get_position());
    }
}

// Move this unit its speed toward its target
void OgreUnit::move_speed(sf::Vector2f ratio,
        std::vector<std::vector<int>> terrain, std::vector< std::vector<int> > move_cost){
    int i;

    float move_ratio;
    int tile_move_cost = move_cost[unit_type]
                            [terrain[int(get_position().x/ratio.x)]
                                    [int(get_position().y/ratio.y)]];
    // step for each level of speed
    for(i = 0; i < speed; i++) {
        move_one(tile_move_cost);
    }

    // get next target position based on next tile
    next_target(ratio);
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

void OgreUnit::short_path(std::vector<std::vector<int>> terrain, std::vector< std::vector<int> > move_cost, PathPt *target, sf::Vector2f ratio)
{
    // first things first, delete old shortest path...
    if (path != nullptr)
        path->clear();

    // roll my own special sauce, basically a vector2i plus G and F
    PathPt *prev_pt, *current;

    int map_x_size = terrain.size();
    int map_y_size = terrain[0].size();

    // get our current pos, so we can make first open list
    sf::Vector2f pos= get_position();

    // new computed cost
    int newG;

    // grand list of actual PathPts
    // any time I need a new pathpt, always add it to
    // this list first, everything else will be a pointer to it
    std::list<PathPt> grand_list;

    std::list<PathPt*> open_list;
    grand_list.push_front(*(new PathPt(round(pos.x/ratio.x),
                                 round(pos.y/ratio.y))));
    open_list.push_front(&(grand_list.front()));
    open_list.front()->set_G(0);
    open_list.front()->set_F(open_list.front()->diag_dist(target));
    current = open_list.front();

    std::list<PathPt*> closed_list;

    // This will hold the actual adjacent pts, computed from
    // the current pt and the steps
    std::list<PathPt*> nearby_pts;

    // These are the directions we will step, all 8 diagonals
    // Maybe I don't actually want to do diagonals...
    std::list<PathPt> steps;
    steps.push_front(PathPt(0,1));
    steps.push_front(PathPt(1,1));
    steps.push_front(PathPt(1,0));
    steps.push_front(PathPt(1,-1));
    steps.push_front(PathPt(0,-1));
    steps.push_front(PathPt(-1,-1));
    steps.push_front(PathPt(-1,0));
    steps.push_front(PathPt(-1,1));

    // keep going while there's things in open list and our target isn't in the closed list
    while(!open_list.empty() and
        std::find_if(closed_list.begin(),
                  closed_list.end(),
                  [target](const PathPt *element)
                  { return target->x == element->x and
                    target->y == element-> y;}) == closed_list.end())
    {
        closed_list.push_front(current);
        // this is supposed to destroy current...still in closed_list?
        open_list.remove(current);
        // If I clear, then I don't have them in adj...
        nearby_pts.clear();
        for (auto step : steps)
        {
            grand_list.push_front(*(new PathPt( *current + step)));
            nearby_pts.push_front(&(grand_list.front()));
        }

        for (auto adj : nearby_pts)
        {
            if (adj->x < 0 or adj->x >= map_x_size or adj->y < 0 or adj->y >= map_y_size)
                continue;
            if(std::find_if(closed_list.begin(),
                  closed_list.end(),
                  [adj](const PathPt *element)
                  { return adj->x == element->x and
                    adj->y == element-> y;}) != closed_list.end())
                continue;
            newG = current->G + current->diag_dist(adj) * move_cost[unit_type][terrain[adj->x][adj->y]];
            // if not in open list add it
            // deref ptr and look at x/y vals
            if(std::find_if(open_list.begin(),
                  open_list.end(),
                  [adj](const PathPt *element)
                  { return adj->x == element->x and
                    adj->y == element-> y;}) == open_list.end())
            {
                adj->parent = current; //ptr to current
                adj->set_G(newG);
                open_list.push_front(adj);
            }
            // if this path better to current spot, replace old
            if(newG <= adj->get_G())
            {
                adj->set_G(newG);
                adj->set_F(adj->get_G() + adj->diag_dist(target));
                adj->parent = current; //ptr to current
                open_list.sort(compare_F); //sort by F values
            }
        }
        current = open_list.front(); // whichever with the lowest F
    }
    // never actually set parent of target, I think
    // so instead, find point in closed_list that has same coord
    // as the target
    // yea, could have used find_if...
    for(auto point: closed_list)
    {
        if(point->x == target->x and point->y == target->y)
        {
            prev_pt = point;
            break;
        }
    }
    // save off the path, really want pathpts, not pointers to
    while(true)
    {
        path->push_front(new PathPt(prev_pt->x, prev_pt->y));
        if (prev_pt->parent == nullptr)
            break;
        prev_pt = prev_pt->parent;
    }
    // pop first off of path to ensure we go to another tile
    if (path->size() != 1)
        path->pop_front();
    // just to be paranoid
    open_list.clear();
    closed_list.clear();
    nearby_pts.clear();
    grand_list.clear();
}


