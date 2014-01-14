// g++ -std=c++11 main.cpp -lsfml-graphics -lsfml-window -lsfml-system
#include <cmath>
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <stdint.h>

// ogre specific includes
#include "unit.hpp"

using std::cerr;

int main(int argc, char* argv[])
{
    uint64_t time_step = 0;

    // Time for the FPS calculation
    float time;

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

    // create a Unit to move around
    // Future, create a list of units
    // Don't place them on the screen right away
    OgreUnit unit = OgreUnit(view.getCenter());

    // How fast is this unit?  Roll into initialization at some point
    // multiple possible constructors?
    unit.set_speed(1); //hard-coded for now 

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
			// left clicking moves the unit
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
                // this is where we clicked
				auto position = sf::Mouse::getPosition(window);
                // change unit target
                // Future, detect what unit we clicked (if any)
                // So we need a method to determine if clicked within unit radius
                // And if there's multiple take the closest
                // Or go to submenu where you pick the unit
                // do all this while paused, then click to where you want to move
                // or always go to a unit submenu about what to do
				unit.set_target_position(position);
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                // TODO: bring up a menu
                // For now, just set target as well
				auto position = sf::Mouse::getPosition(window);
				unit.set_target_position(position);
            }
		}

		time = timer.getElapsedTime().asSeconds();
		timer.restart();

        // Move the unit(s)
        unit.move_speed();

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
        unit.draw_on(window); // maybe roll into the movement
		window.draw(fps_text);
		window.draw(time_text);

		// refresh the window
		window.display();

        // increment the time step
        time_step++;
	}

	return 0;
}
