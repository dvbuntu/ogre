#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <cstdlib>
#include <vector>
#include <SFML/Graphics.hpp>

// Will need to translate all clicks into map_size chunks
#ifndef X_MAP_SIZE
#define X_MAP_SIZE 40
#endif
#ifndef Y_MAP_SIZE
#define Y_MAP_SIZE 30
#endif

#ifndef NUM_TERRAIN_TYPES
#define NUM_TERRAIN_TYPES 3
#endif

// It's not racism...
#ifndef WHITE_BIAS
#define WHITE_BIAS 64
#endif


// terrain is [plains, hills, mtn]
// should probable free/delete this crap at end of game...
void setup_move_cost(std::vector< std::vector<int> > *move_cost){
    std::vector<int> *fighter_move = new std::vector<int>;
    fighter_move->push_back(2);// Nominally, fighter
    fighter_move->push_back(10);
    fighter_move->push_back(20);
    move_cost->push_back(*fighter_move);
    std::vector<int> *amazon_move = new std::vector<int>;
    amazon_move->push_back(2);// Nominally, amazon
    amazon_move->push_back(10);
    amazon_move->push_back(10);
    move_cost->push_back(*amazon_move);
    std::vector<int> *wizard_move = new std::vector<int>;
    wizard_move->push_back(5);// Nominally, wizard
    wizard_move->push_back(10);
    wizard_move->push_back(30);
    move_cost->push_back(*wizard_move);
    std::vector<int> *hawkman_move = new std::vector<int>;
    hawkman_move->push_back(1);// Nominally, hawkman
    hawkman_move->push_back(1);
    hawkman_move->push_back(1);
    move_cost->push_back(*hawkman_move);
}

// should probable free/delete this crap at end of game...
// make some colors for the land, and bias toward white
void setup_terrain_colors(std::vector<sf::Color> *terrain_colors){
    for (int c = 0; c < NUM_TERRAIN_TYPES; c++){
        terrain_colors->push_back(*(new sf::Color(
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS),
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS),
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS)
                    )));
    }
}

// bogus color distance
int color_diff(sf::Color c1, sf::Color c2)
{
    int rdiff = c1.r - c2.r;
    int gdiff = c1.g - c2.g;
    int bdiff = c1.b - c2.b;

    return rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
}

int closest_color(sf::Color c1, std::vector<sf::Color> terrain_colors) {
    
    int diff;

    // set first color as default winner
    int min_diff = color_diff(c1, terrain_colors[0]);
    int winner = 0;

    for (int i = 1; i < terrain_colors.size(); i++)
    {
        diff = color_diff(c1,terrain_colors[i]);
        if (diff < min_diff)
            winner = i;
    }
    return winner;
}

// Only read the top left rect of image for now, scale later
void setup_battle_map(std::vector<std::vector<int> > * battle_map,
                      std::vector<sf::Color> terrain_colors,
                      sf::Image * image,
                      sf::Texture * texture,
                      sf::Sprite * sprite){

    if (!image->loadFromFile("resources/basic_map.png"))
    {
        std::cout << "Failed to load image!" << std::endl;
    }

    for (int x = 0; x < X_MAP_SIZE; x++){
        battle_map->push_back(*(new std::vector<int>));
        for (int y = 0; y < Y_MAP_SIZE; y++){
            //battle_map->back().push_back(std::rand() % NUM_TERRAIN_TYPES);
            battle_map->back().push_back(closest_color(image->getPixel(x,y),terrain_colors));
        }
    }
}

#endif
