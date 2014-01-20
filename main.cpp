// g++ -std=c++11 main.cpp -lsfml-graphics -lsfml-window -lsfml-system
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <stdint.h>

// ogre specific includes
#include "unit.hpp"

#define NUM_UNITS 5

using std::cerr;

// maybe make a 'helper' cpp file
bool check_distances(std::list<OgreUnit*> units, OgreUnit **target_unit, bool *paused, sf::Vector2f position, float unit_size);

int main(int argc, char* argv[])
{
    uint64_t time_step = 0;

    int i;

    // Do we step at all
    bool paused = false;

    // Did I click on a unit?
    bool select_unit;

    // Time for the FPS calculation
    float time;

    // where we click
    sf::Vector2f position;

    // Window with title and our view
	sf::RenderWindow window(sf::VideoMode(800, 600), "Ogre Battle");
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

    // create units to move around
    std::list<OgreUnit*> units;
    for (i = 0; i < NUM_UNITS; i++)
    {
        units.push_front(new OgreUnit(view.getCenter() + sf::Vector2f(rand()%100,rand()%100)));
        units.front()->set_speed(rand()%5 + 1);
    }

    // The unit selected
    OgreUnit *target_unit = units.back();

    // size of the circle should be UNIT_SIZE but it can't seem to understand that...
    float unit_size = target_unit->get_size();

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
                        target_unit->set_target_position(position);
                        target_unit->set_select_state(false);
                        paused = false;
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
                {
                    paused = false;
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
            // Move the unit(s)
            for(auto unit : units)
            {
                if (unit->get_position() != unit->get_target_position())
                    unit->move_speed();
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

		// draw everything to the window
		window.clear(sf::Color::White);
        for(auto unit : units)
        {
            unit->draw_on(window); // maybe roll into the movement
        }
		window.draw(fps_text);
		window.draw(time_text);

		// refresh the window
		window.display();
	}

	return 0;
}


bool check_distances(std::list<OgreUnit*> units, OgreUnit **target_unit, bool *paused, sf::Vector2f position, float unit_size)
{
    bool select_unit = false;
    // check if we clicked a unit
    for(auto unit : units)
    {
        // just check each circle
        // for now, go with the first one
        if (unit->distance<>(position) < unit_size) {
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

