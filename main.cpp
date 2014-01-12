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

    // Window title
	sf::RenderWindow window(sf::VideoMode(800, 600), "Ogre Battle");

    // load font
	sf::Font font;
	if (!font.loadFromFile("./resources/DejaVuSans.ttf"))
    {
        cerr << "Couldn't find font DejaVuSans.ttf!\n";
        return 1;
    }

	// create text to display
	sf::Text text("Unit", font, 32);
	text.setColor(sf::Color::Black);
	// center text
	auto bounds = text.getLocalBounds();
	text.setOrigin(bounds.width / 2, bounds.height / 2);
	text.setPosition(window.getDefaultView().getCenter());

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
                // move the text
                // Change this to a 'unit'
				text.setPosition(position.x, position.y);
			}
		}

		// crazy-ass transforms
		float time = clock.getElapsedTime().asSeconds();
		text.setRotation(std::sin(time) * 45);
		text.setScale(std::cos(time / 2), std::cos(time / 3));

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
		window.draw(text);
		window.draw(fps_text);
		window.draw(time_text);

		// refresh the window
		window.display();

        // increment the time step
        time_step++;
	}

	return 0;
}
