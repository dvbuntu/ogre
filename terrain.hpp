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

#ifndef DS_SIZE
#define DS_SIZE 65
#endif

#ifndef NUM_TERRAIN_TYPES
#define NUM_TERRAIN_TYPES 3
#endif

// It's not racism...
#ifndef WHITE_BIAS
#define WHITE_BIAS 64
#endif


// terrain is [plains, water, mtn]
// should probable free/delete this crap at end of game...
void setup_move_cost(std::vector< std::vector<int> > *move_cost){
    std::vector<int> *fighter_move = new std::vector<int>;
    fighter_move->push_back(20);
    fighter_move->push_back(2);// Nominally, fighter
    fighter_move->push_back(10);
    move_cost->push_back(*fighter_move);
    std::vector<int> *wizard_move = new std::vector<int>;
    wizard_move->push_back(10);
    wizard_move->push_back(5);// Nominally, wizard
    wizard_move->push_back(17);
    move_cost->push_back(*wizard_move);
    std::vector<int> *amazon_move = new std::vector<int>;
    amazon_move->push_back(14);
    amazon_move->push_back(13);// Nominally, amazon
    amazon_move->push_back(5);
    move_cost->push_back(*amazon_move);
    std::vector<int> *hawkman_move = new std::vector<int>;
    hawkman_move->push_back(10);
    hawkman_move->push_back(10);// Nominally, monk...
    hawkman_move->push_back(12);
    move_cost->push_back(*hawkman_move);
}

// should probable free/delete this crap at end of game...
// make some colors for the land, and bias toward white
void setup_terrain_colors(std::vector<sf::Color> *terrain_colors){
    /*
    for (int c = 0; c < NUM_TERRAIN_TYPES; c++){
        terrain_colors->push_back(*(new sf::Color(
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS),
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS),
                    WHITE_BIAS + std::rand() % (256-WHITE_BIAS)
                    )));
    }
    */
    terrain_colors->push_back(*(new sf::Color(sf::Color::Blue)));
    terrain_colors->push_back(*(new sf::Color(sf::Color::Green)));
    terrain_colors->push_back(*(new sf::Color(sf::Color(64,64,64))));
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
        {
            winner = i;
            min_diff = diff;
        }
    }
    return winner;
}

// terrain generation adapted from http://www.bluh.org/code-the-diamond-square-algorithm/
int sample(int x, int y, std::vector<std::vector<int> > * battle_map)
{
    int sq_size = battle_map->size();
    // if we're negative, be careful
    while (x < 0)
        x += sq_size;
    while (y < 0)
        y += sq_size;
    return (*battle_map)[x % sq_size][y % sq_size];
}
 
void set_sample(int x, int y, int value, std::vector<std::vector<int> > * battle_map)
{
    int sq_size = battle_map->size();
    // if we're negative, be careful
    while (x < 0)
        x += sq_size;
    while (y < 0)
        y += sq_size;
    (*battle_map)[x % sq_size][y % sq_size] = value;
}

void sample_square(int x, int y, int size, int value, std::vector<std::vector<int> > * battle_map)
{
    int hs = size / 2;
 
    // a     b 
    //
    //    x
    //
    // c     d
 
    double a = sample(x - hs, y - hs, battle_map);
    double b = sample(x + hs, y - hs, battle_map);
    double c = sample(x - hs, y + hs, battle_map);
    double d = sample(x + hs, y + hs, battle_map);
 
    set_sample(x, y, ((a + b + c + d) / 4) + value, battle_map);
}
 
void sample_diamond(int x, int y, int size, int value, std::vector<std::vector<int> > * battle_map)
{
    int hs = size / 2;
 
    //   c
    //
    //a  x  b
    //
    //   d
 
    double a = sample(x - hs, y, battle_map);
    double b = sample(x + hs, y, battle_map);
    double c = sample(x, y - hs, battle_map);
    double d = sample(x, y + hs, battle_map);
 
    set_sample(x, y, ((a + b + c + d) / 4) + value, battle_map);
}

void diamond_square(int stepsize, int scale,
                    std::vector<std::vector<int> > * battle_map)

{
    int halfstep = stepsize / 2;
    int sq_size = battle_map->size();
 
    for (int y = halfstep; y < sq_size + halfstep; y += stepsize)
    {
        for (int x = halfstep; x < sq_size + halfstep; x += stepsize)
        {
            sample_square(x, y, stepsize, std::rand() % scale - scale/2, battle_map);
        }
    }
 
    for (int y = 0; y < sq_size; y += stepsize)
    {
        for (int x = 0; x < sq_size; x += stepsize)
        {
            sample_diamond(x + halfstep, y, stepsize, rand() % scale - scale/2, battle_map);
            sample_diamond(x, y + halfstep, stepsize, rand() % scale - scale/2, battle_map);
        }
    }
 
}


void generate_terrain(std::vector<std::vector<int> > * battle_map,
                      int X,
                      int Y)
{
    int sq_size=4;
    int sample_size;
    int scale = 2;
    int n,x,y,tile;

    int bounds[NUM_TERRAIN_TYPES + 1];
    int max_val = 0;
    int min_val = -1;

    // we need to make a map big enough, we just grab the top X,Y corner
    while (X > sq_size || Y > sq_size) {
        sq_size = (sq_size)*2;
        scale *= 2;
    }

    // this keeps track of which iteration
    sample_size = sq_size;


    // create initial blank map
    for (int x = 0; x < sq_size; x++){
        battle_map->push_back(*(new std::vector<int>));
        for (int y = 0; y < sq_size; y++){
            battle_map->back().push_back(0);
        }
    }

    // initial seed values
    (*battle_map)[0][0] = std::rand() % scale;
    (*battle_map)[0][sq_size-1] = (*battle_map)[0][0];
    (*battle_map)[sq_size-1][0] = (*battle_map)[0][0];
    (*battle_map)[sq_size-1][sq_size-1] = (*battle_map)[0][0];

    while (sample_size > 1)
    {
        diamond_square(sample_size, scale, battle_map);

        sample_size /= 2;
        scale /= 2;
    }

    // now remove everything that's beyond the bounds we want
    for(x = X; x < sq_size; x++) {
        for(y = Y; y < sq_size; y++) {
            battle_map->back().pop_back();
        }
        battle_map->pop_back();
    }

    // get bounds of terrain type cut offs
    for(x = 0; x < X; x++){
        for(y = 0; y < Y; y++){
            tile = (*battle_map)[x][y];
            if (min_val < 0 || tile < min_val)
                min_val = tile;
            if (tile > max_val)
                max_val = tile;
        }
    }

    for(n = 0; n < NUM_TERRAIN_TYPES +1; n++){
        bounds[n] = n*(max_val - min_val) / NUM_TERRAIN_TYPES + min_val;
    }

    // just to be safe
    bounds[NUM_TERRAIN_TYPES] += 1;

    // apply the bounds to the battle map
    for(x = 0; x < X; x++){
        for(y = 0; y < Y; y++){
            for(n = 0; n < NUM_TERRAIN_TYPES; n++){
                if ((*battle_map)[x][y] < bounds[n+1])
                {
                    (*battle_map)[x][y] = n;
                    break;
                }
            }
        }
    }

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
        generate_terrain(battle_map, X_MAP_SIZE, Y_MAP_SIZE);
    }
    else{
        for (int x = 0; x < X_MAP_SIZE; x++){
            battle_map->push_back(*(new std::vector<int>));
            for (int y = 0; y < Y_MAP_SIZE; y++){
                battle_map->back().push_back(closest_color(image->getPixel(x,y),terrain_colors));
            }
        }
    }
}

#endif
