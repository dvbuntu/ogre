// g++ -std=c++11 main.cpp -lsfml-graphics -lsfml-window -lsfml-system
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <time.h>
#include <math.h>

// General helpful functions
#ifndef HELPER_HPP
#include "helper.hpp"
#endif

// ogre specific includes
#ifndef PLAYER_HPP
#include "player.hpp"
#endif

#ifndef HERO_HPP
#include "hero.hpp"
#endif

#ifndef UNIT_HPP
#include "unit.hpp"
#endif

#ifndef TOWN_HPP
#include "town.hpp"
#endif

#ifndef TERRAIN_HPP
#include "terrain.hpp"
#endif

#define NUM_UNITS 5
#define NUM_TOWNS 6
#define FIGHT_THRESH (3*UNIT_SIZE)
#define DAY_LENGTH 1000 // length of a 'game day' in time steps
#define ENEMY_DEPLOY_CHANCE 2
#define X_VIDEO_SIZE 800
#define Y_VIDEO_SIZE 600
#define MIN_HOME_DIST 400

using std::cerr;

// maybe make a 'helper' cpp file
bool check_distances(std::list<OgreUnit*> units, OgreUnit **target_unit, bool *paused, sf::Vector2f position, float unit_size);
OgrePlayer *check_win(std::list<OgreTown*> towns, OgrePlayer *player, OgrePlayer *enemy);
void resolve_fights(std::list<OgreUnit*> units, sf::RenderWindow& window);
void reap_units(std::list<OgreUnit*> *units);
void reap_towns(std::list<OgreTown*> *towns);
void build_town(std::list<OgreTown*> *towns, std::list<OgreUnit*> *units, OgrePlayer *player, OgreUnit *target_unit, sf::Vector2f position);
void deploy_unit(std::list<OgreUnit*> *units, OgrePlayer *player, std::list<OgreTown*> towns, sf::Vector2f position, sf::Font *font);
bool in_my_town(std::list<OgreTown*> towns, OgrePlayer *player, sf::Vector2f position);
bool want_to_rest(OgreUnit *unit);
void find_visible(std::list<OgreUnit*> units, OgrePlayer *player);

int main(int argc, char* argv[])
{
    // generate a random seed
    srand(time(NULL));

    uint64_t time_step = 0;

    int i, j;

    // Do we step at all
    bool paused = false;

    // Did I click on a unit?
    bool select_unit;

    // Time for the FPS calculation
    float time;

    // where we click
    sf::Vector2f position;

    // distance vector info
    sf::Vector2f distance;

    // tile that we actually click
    sf::Vector2i tile_coord;

    // ratio of screen to tiles
    float x_ratio = X_VIDEO_SIZE / X_MAP_SIZE;
    float y_ratio = Y_VIDEO_SIZE / Y_MAP_SIZE;
    sf::Vector2f ratio_vector = sf::Vector2f(x_ratio, y_ratio);


    // premade terrain shapes
    std::vector< std::vector<int> > move_cost;
    setup_move_cost(&move_cost);

    std::vector<sf::Color> terrain_colors;
    setup_terrain_colors(&terrain_colors);

    // map image
    sf::Image map_image;

    // map texture
    sf::Texture map_texture;

    // map image for displaying
    sf::Sprite map_sprite;

    std::vector<std::vector<int> > battle_map;
    setup_battle_map(&battle_map, terrain_colors, &map_image, &map_texture, &map_sprite);

    std::vector< std::vector<sf::RectangleShape>> tile_display;
    i = 0;
    for (auto row : battle_map) {
        tile_display.push_back(*(new std::vector<sf::RectangleShape>));
        j = 0;
        for (auto tile : row) {
            tile_display.back().push_back(*(new sf::RectangleShape(
                          sf::Vector2f(x_ratio,
                          y_ratio))));
            tile_display[i][j].setPosition( i * x_ratio, j * y_ratio);
            // twiddle the bottom bits of color
            tile_display[i][j].setFillColor(twiddle_color(terrain_colors[battle_map[i][j]]));
            j++;
        }
        i++;
    }

    // Window with title and our view
	sf::RenderWindow window(sf::VideoMode(X_VIDEO_SIZE, Y_VIDEO_SIZE), "Ogre Battle");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(120);
    sf::View view = window.getDefaultView();

    // load font
	sf::Font font;
	if (!font.loadFromFile("./resources/DejaVuSans.ttf"))
    {
        cerr << "Couldn't find font DejaVuSans.ttf!\n";
        return 1;
    }

    // Create two players, and a pointer to whoever
    OgrePlayer player = OgrePlayer(PLAYER);
    OgrePlayer enemy = OgrePlayer(ENEMY);

    // assign the capitol locations, make sure they're far enough apart
    player.set_home(sf::Vector2f(std::rand() % 200 - 150, std::rand() % 200 - 150));
    enemy.set_home(sf::Vector2f(std::rand() % 200 - 50, std::rand() % 200 - 50));
    distance = player.get_home() - enemy.get_home();
    while(sqrt(distance.x * distance.x + distance.y * distance.y) < MIN_HOME_DIST)
    {
        player.set_home(sf::Vector2f(std::rand() % 200 - 150, std::rand() % 200 - 150));
        enemy.set_home(sf::Vector2f(std::rand() % 200 - 50, std::rand() % 200 - 50));
        distance = player.get_home() - enemy.get_home();
    }

    OgrePlayer *owner;

    // change in town ownership, +1 if player liberates, -1 if enemy, 0 no change
    int town_shift = 0;

    // create some towns to capture
    std::list<OgreTown*> towns;
    // set the capitols
    towns.push_front(new OgreTown(view.getCenter()
                + player.get_home()));
    towns.front()->set_owner(&player);
    towns.front()->set_capitol(true);
    player.set_num_towns(player.get_num_towns() + 1);
    towns.push_front(new OgreTown(view.getCenter()
                + enemy.get_home()));
    towns.front()->set_owner(&enemy);
    towns.front()->set_capitol(true);
    enemy.set_num_towns(enemy.get_num_towns() + 1);
    for (i = 0; i < NUM_TOWNS-2; i++)
    {
        // somewhat hacky
        // should define a start position for player and enemy
        // then build towns out from that (and units too, I guess)
        if (rand()%100 > 50)
        {
            towns.push_front(new OgreTown(view.getCenter()
                        + player.get_home()
                        + sf::Vector2f(std::rand() % 200 - 100, std::rand() % 200 - 100)));
            towns.front()->set_owner(&player);
            player.set_num_towns(player.get_num_towns() + 1);
        }
        else
        {
            towns.push_front(new OgreTown(view.getCenter()
                        + enemy.get_home()
                        + sf::Vector2f(std::rand() % 200 - 100, std::rand() % 200 - 100)));
            towns.front()->set_owner(&enemy);
            enemy.set_num_towns(enemy.get_num_towns() + 1);
        }
        towns.front()->set_info(towns.front()->get_max_hp());
    }


    // create units to move around
    // should really just do random deploy_unit() calls
    std::list<OgreUnit*> units;
    for (i = 0; i < NUM_UNITS; i++)
    {
        // should move unit to position depending on player
        // or start with no units...
        position = sf::Vector2f(rand()%200 - 100,rand()%200 - 100);
        units.push_front(new OgreUnit(view.getCenter()));
        // random speed
        units.front()->set_speed(rand()%4 + 1);

        //Set max HP
        units.front()->set_max_hp();

        //Set the info
        units.front()->set_info(units.front()->get_hp(), &font, 12);

        //units.front()->update_cost();

        // Dirty hack for now, TODO
        if (rand()%100 > 50)
        {
            units.front()->set_owner(&player);
            player.set_num_units(player.get_num_units() + 1);
            units.front()->move_by(position
                                +  player.get_home()
                                );
            // set target to current position
            units.front()->set_target_position(units.front()->get_position());
        }
        else
        {
            units.front()->set_owner(&enemy);
            // send it to a random town...
            // They all seem to be walking to the same town...
            // TODO: True AI, walk toward player towns
            enemy.set_num_units(enemy.get_num_units() + 1);
            units.front()->move_by(position
                                +  enemy.get_home()
                                );
            units.front()->set_target_tile(
                    (*random_element(towns.begin(),towns.end()))->get_position(),
                    ratio_vector,
                    battle_map,
                    move_cost);
        }
    }

    // The unit selected
    OgreUnit *target_unit = units.back();

    // size of the circle should be UNIT_SIZE but it can't seem to understand that...
    float unit_size = target_unit->get_size();

    // a boxed area for the match information
    sf::RectangleShape info_bg;
    info_bg.setSize(sf::Vector2f(100,100));
    info_bg.setFillColor(sf::Color(128,128,128,128));
    info_bg.setOutlineColor(sf::Color::Black);
    info_bg.setOutlineThickness(2);
    info_bg.setPosition(sf::Vector2f(2,2));

	// create a stringstream for converting fps to string, and text for displaying it
	std::stringstream fps;
	sf::Text fps_text("", font, 12);
	fps_text.setColor(sf::Color(127, 127, 127));
	fps_text.setPosition(10, 10);

    // setup time step display
	std::stringstream time_step_disp;
	sf::Text time_text("", font, 12);
	time_text.setColor(sf::Color(127, 127, 127));
	time_text.setPosition(10, 30);

    // setup player gold display
	std::stringstream player_gold_disp;
	sf::Text player_gold_text("", font, 12);
	player_gold_text.setColor(player.get_color());
	player_gold_text.setPosition(10, 50);

    // setup player gold display
	std::stringstream enemy_gold_disp;
	sf::Text enemy_gold_text("", font, 12);
	enemy_gold_text.setColor(enemy.get_color());
	enemy_gold_text.setPosition(10, 70);

	sf::Clock timer; // for measuring time per frame
	sf::Clock clock; // for measuring overall time

    // main program loop
	while (window.isOpen())
	{
		// process all input events that have occured since last frame
		sf::Event event;
		while (window.pollEvent(event))
		{
			// clicking the OS's close button or pressing escape
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			// toggle pause with spacebar
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
            {
                paused = !paused;
                continue;
            }

            // Check to bring up display, don't care if paused or not
            // bring up the info window here using fight_draw_on, plus info
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I )
            {
                for (auto unit : units)
                {
                    if (unit == target_unit)
                        unit->display_info();
                }
            }

            if (paused)
            {

                // left clicking moves the unit or selects it and pauses game
                // see max's tron2
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    // reset unit selection bool
                    select_unit = false;

                    // this is where we clicked
                    position = sf::Vector2f(sf::Mouse::getPosition(window));

                    select_unit = check_distances(units,&target_unit,&paused, position, unit_size);
                    if (!select_unit)
                    {
                        // give target unit new move order
                        target_unit->set_target_tile(position,
                                ratio_vector,
                                battle_map,
                                move_cost);
                        target_unit->set_select_state(false);
                        paused = false;
                    }
                }
                // deploy a unit where the pointer is
                else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) )
                {
                    position = sf::Vector2f(sf::Mouse::getPosition(window));
                    deploy_unit(&units,&player,towns,position,&font);
                }
                // Build a town on location of selected unit
                else if (select_unit && (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T))
                {
                    position = target_unit->get_position();
                    build_town(&towns,&units,&player,target_unit,position);
                    select_unit = false;
                    target_unit = units.back();
                }
			}
            else
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    // this is where we clicked
                    position = sf::Vector2f(sf::Mouse::getPosition(window));
                    // change target unit
                    select_unit = check_distances(units,&target_unit,&paused, position, unit_size);
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
                {
                    paused = true;
                }
            }
		}

        if (!paused){
            // Fight it Out!
            resolve_fights(units, window);

            // Reap the dead!
            reap_units(&units);
            reap_towns(&towns);

            // Move the unit(s)
            for(auto unit : units)
            {
                // Control unit movement
                if (unit->get_position() != unit->get_target_position())
                {
                    unit->animate(time_step);
                    unit->move_speed(ratio_vector, battle_map, move_cost);
                }

                // give enemy units a new target
                // But they'll pause for at least one round
                // TODO: actual AI like moving to player towns
                // TODO: AI controlled by unit itself with helper AI fcns
                else if (unit->get_owner() == &enemy)
                {
                    if (!want_to_rest(unit))
                    {
                        unit->set_target_tile(
                        (*random_element(towns.begin(),towns.end()))->get_position(),
                        ratio_vector,
                        battle_map,
                        move_cost);
                    }
                }

                if (unit->get_position() == unit->get_target_position()) {
                    // check if we 
                    // Potentially heal if not moving
                    if (rand() % 100 < UNIT_HEAL_PERCENT)
                        unit->heal();
                }
                unit->set_info(unit->get_hp());
            }

            // Check town ownership and life
            for(auto town : towns)
            {
                owner = town->check_conquest(units);
                if (rand() % 100 < TOWN_HEAL_PERCENT)
                    town->heal();

                if (owner == &player)
                    town_shift = 1;
                else if (owner == &enemy)
                    town_shift = -1;
                else
                    town_shift = 0;
                // Don't need this once each town pays its own taxes
                player.set_num_towns(player.get_num_towns()+town_shift);
                enemy.set_num_towns(enemy.get_num_towns()-town_shift);

            }

            // Collect taxes and pay troops if it's a new day
            // Pay after, but you might be left with no money...
            if ((time_step % DAY_LENGTH) == 0)
            {
                for (auto town: towns)
                {
                    town->pay_taxes();
                }
                for (auto unit: units)
                {
                    unit->collect_pay();
                }
            }

            // Enemy chance to deploy
            if ((rand() % 500) < ENEMY_DEPLOY_CHANCE ) // more than max req
            {
                // change position to random town
                // Increase ENEMY_DEPLOY_CHANCE b/c
                // we just select a random town
                // so they'll deploy more when they have more towns
                deploy_unit(&units, &enemy, towns,
                (*random_element(towns.begin(),towns.end()))->get_position()
                ,&font);
            }
            // Check win condition
            if (check_win(towns,&player,&enemy) != nullptr)
            {
                // TODO: splash 'winner is: ...' and pause game
                std::cout << "Game Over!" << std::endl;
                break;
            }

            // increment the time step
            time_step++;
        }


		time = timer.getElapsedTime().asSeconds();
		timer.restart();

		// update fps text
		fps << (int)(1 / time) << " FPS";
		fps_text.setString(fps.str());
		fps.str("");

        // update time step
		time_step_disp << "Time Step: " << time_step ;
		time_text.setString(time_step_disp.str());
		time_step_disp.str("");

        // update gold
		player_gold_disp << "Player: " << player.get_gold() ;
		player_gold_text.setString(player_gold_disp.str());
		player_gold_disp.str("");
		enemy_gold_disp << "Enemy: " << enemy.get_gold() ;
		enemy_gold_text.setString(enemy_gold_disp.str());
		enemy_gold_disp.str("");

		// draw everything to the window
		window.clear(sf::Color::White);

        //order of drawing is important, put terrain first
        // how can I avoid doing this every time?
        for(auto row : tile_display){
            for (auto tile : row) {
                window.draw(tile);
            }
        }

        // draw the Match information
        window.draw(info_bg);
		window.draw(fps_text);
		window.draw(time_text);
		window.draw(player_gold_text);
		window.draw(enemy_gold_text);

        // then features like towns and finally units
        for(auto town : towns)
        {
            town->draw_on(window); // maybe roll into the movement
        }
        find_visible(units, &player);
        for(auto unit : units)
        {
            if (unit->get_visible())
                unit->draw_on(window); // maybe roll into the movement
        }

		// refresh the window
		window.display();
	}

    // clean up memory
    units.clear();
    towns.clear();
	return 0;
}


// Check if we click any units
bool check_distances(std::list<OgreUnit*> units, OgreUnit **target_unit, bool *paused, sf::Vector2f position, float unit_size)
{
    bool select_unit = false;
    // check if we clicked a unit
    for(auto unit : units)
    {
        // just check each circle
        // Player not allowed to move enemy units
        if (unit->distance<>(position) < unit_size
                && unit->get_owner()->get_id() != ENEMY)
        {
            // unselect old unit
            (*target_unit)->set_select_state(false);

            // select the new unit
            (*target_unit) = unit;
            (*target_unit)->set_select_state(true);
            (*paused) = true;
            select_unit = true;
            break;
        }
    }
    return select_unit;
}

// Check for a winner, someone who has all the towns,
// or only player with money
OgrePlayer *check_win(std::list<OgreTown*> towns, OgrePlayer *player, OgrePlayer *enemy)
{
    OgrePlayer *winner = nullptr;
    bool player_capitol = false;
    bool enemy_capitol = false;

    // Check to see if a player owns 2 capitols (really, if a player lost their capitol
    for (auto town : towns)
    {
        if (town->get_owner() == player)
            player_capitol = town->get_capitol();
        else if (town->get_owner() == enemy)
            enemy_capitol = town->get_capitol();
    }

    if (player_capitol == false && enemy_capitol == true)
        winner = enemy;
    if (enemy_capitol == false && player_capitol == true)
        winner = player;

    // Check if one player ran out of money
    if (winner == nullptr)
    {
        if ((player->get_gold()) == 0)
            winner = enemy;
        else if ((enemy->get_gold()) == 0)
            winner = player;
    }

    return winner;
}

// Check for and resolve any conflict between opposing units that are too close
void resolve_fights(std::list<OgreUnit*> units, sf::RenderWindow& window)
{
    // Maybe check each direction instead, and just skip if not move
    // Could be dangerous, I fiddle with the contents of units within the loop...
    for (auto unit1 : units)
    {
        // Skip if we're not moving
        // Maybe don't do if we got knocked back by a previous battle..
        if (unit1->get_position() == unit1->get_target_position())
            continue;
        for (auto unit2 : units)
        {
            // Stop hitting yourself!
            if (unit1->get_owner() == unit2->get_owner())
                continue;
            if (unit1->distance<>(unit2->get_position()) < FIGHT_THRESH)
            {
                unit1->show_fighting();
                unit2->show_fighting();
                unit1->draw_on(window);
                unit2->draw_on(window);
                window.display();
                unit1->fight(unit2);
                unit1->done_fighting();
                unit2->done_fighting();
            }
        }
    }
    // Check all pairs of units
/*    for (auto i = foo.begin(); i != foo.end(); ++i) {
        for (auto j = i; ++j != foo.end(); ) {
            std::cout << *i << *j << std::endl;
        }
    }*/
}

// check for dead units, change the player unit counts
void reap_units(std::list<OgreUnit*> *units)
{
    OgrePlayer *owner;
    std::list<OgreUnit*>::iterator unit = units->begin();
    while(unit != units->end())
    {
        if ((*unit)->get_hp() == 0)
        {
            // Remove this unit from the player's rolls
            owner = (*unit)->get_owner();
            owner->set_num_units(owner->get_num_units() - 1);
            // erase the current guy and move to the next
            unit = units->erase(unit);
        } // TODO: reap the dead heroes within units as well
        else
        {
            ++unit;
        }
    }
}

// Check for ruined towns
// TODO: make a templated 'reap' function
void reap_towns(std::list<OgreTown*> *towns)
{
    OgrePlayer *owner;
    std::list<OgreTown*>::iterator town = towns->begin();
    while(town != towns->end())
    {
        if ((*town)->get_hp() == 0)
        {
            // Remove this town from the player's rolls
            owner = (*town)->get_owner();
            owner->set_num_towns(owner->get_num_towns() - 1);
            // erase the current guy and move to the next
            town = towns->erase(town);
        }
        else
        {
            ++town;
        }
    }
}

// Build a town here using this unit
void build_town(std::list<OgreTown*> *towns, std::list<OgreUnit*> *units, OgrePlayer *player, OgreUnit *target_unit, sf::Vector2f position)
{
    int town_cost = (*random_element(towns->begin(),towns->end()))->get_payout() * 20;
    if (player->get_gold() >=
        (town_cost) && target_unit->get_select_state())
    {
        // We built this city!
        towns->push_front(new OgreTown(position));
        towns->front()->set_payout(town_cost / 20);
        towns->front()->set_info(towns->front()->get_max_hp());
        towns->front()->set_owner(player);

        // On blood and bone...
        // remove b/c the pointer value is unique, and erase is for iterator object
        units->remove(target_unit);
        player->set_num_units(player->get_num_units() - 1);

        // Pay for materials
        player->set_gold(player->get_gold() - town_cost);

        // Do some accounting
        player->set_num_towns(player->get_num_towns() + 1);
    }
}

// Add a unit for the given player
void deploy_unit(std::list<OgreUnit*> *units, OgrePlayer *player, std::list<OgreTown*> towns, sf::Vector2f position, sf::Font *font)
{
    // check if we clicked in one of our towns
    if (in_my_town(towns, player, position))
    {
        // Cost is some function of current units?  
        // Really depends on the unit we're planning to deploy
        if (player->get_gold() >=
                ((*random_element(units->begin(),units->end()))->get_cost()) * 2)
        {
            units->push_front(new OgreUnit(position));

            // TODO: store units->front() in a pointer to clean this up

            // random speed
            // TODO: make a unit.init() method to take care of this stuff
            // that will standardize the initial speed and whatnot as well
            // Why not set speed when generate unit?
            units->front()->set_speed(rand()%4 + 1);

            // set the hit points
            units->front()->set_max_hp();

            //Set the info
            units->front()->set_info(units->front()->get_hp(), font, 12);

            // Update the labor costs for weakened unit
            //units->front()->update_cost();

            // taken care of when we make the unit
            units->front()->set_owner(player);

            units->front()->set_target_position(units->front()->get_target_position());

            player->set_gold(player->get_gold() - units->front()->get_cost() * 2);
            player->set_num_units(player->get_num_units() + 1);
        }
    }
}

bool in_my_town(std::list<OgreTown*> towns, OgrePlayer *player, sf::Vector2f position)
{
    for(auto town : towns){
        if (town->get_owner() != player)
            continue;
        if (town->distance<>(position) < TOWN_SIZE)
        {
            return true;
        }
    }
    return false;
}

// enemy units will stop to rest in town if they're hurting bad, or just sometimes
bool want_to_rest(OgreUnit *unit)
{
    float life_fraction = unit->get_hp() / (float) unit->get_max_hp();
    float r = rand() / (float) RAND_MAX;
    if (life_fraction < 0.5)
    {
        return true;
    }
    else if (life_fraction < r )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void find_visible(std::list<OgreUnit*> units, OgrePlayer *player)
{
    std::list<OgreUnit*> player_units;
    std::list<OgreUnit*> enemy_units;
    for (auto unit: units)
    {
        if (unit->get_owner() == player)
        {
            unit->set_visible(true);
            player_units.push_front(unit);
        }
        else
            enemy_units.push_front(unit);
    }
    for (auto unit: enemy_units)
    {
        unit->set_visible(false);
        for (auto  my_unit:player_units)
        {
            if (unit->distance(my_unit->get_position()) < my_unit->get_vision())
            {
                unit->set_visible(true);
                break;
            }
        }
    }
    // may not have to do this
    player_units.clear();
    enemy_units.clear();
}


// TODO: put this in a helper .cpp file (or even just a header file
/*
template <typename I>
I random_element(I begin, I end)
{
    unsigned long n = std::distance(begin, end);
    // We have to store this in a variable or it makes divisor huge
    unsigned long temp = RAND_MAX;
    unsigned long divisor = (temp + 1) / n;

    unsigned long k;
    do { k = std::rand() / divisor; } while (k >= n);

    return std::next(begin, k);
}
*/

/* // Not sure I want this function
template<typename It>
void for_each_pair(It begin, It end) {
    for (It  i = begin; i != end; ++i) {
        for (It j = i; ++j != end;) {
             std::cout << *i << *j << std::endl;
        }
    }
}
*/
