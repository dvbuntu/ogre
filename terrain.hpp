#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <cstdlib>
#include <vector>
#include <SFML/Graphics.hpp>

// Will need to translate all clicks into map_size chunks
#ifndef X_MAP_SIZE
#define X_MAP_SIZE 100
#endif
#ifndef Y_MAP_SIZE
#define Y_MAP_SIZE 100
#endif

std::vector<std::vector<int> > battle_map;
int battle_map[X_MAP_SIZE][Y_MAP_SIZE];


#ifndef NUM_TERRAIN_TYPES
#define NUM_TERRAIN_TYPES 3
#endif

// info about how each unit type moves through each terrain type
// terrain is [plains, hills, mtn]
int move_cost = { {1, 2, 10}, // Nominally, fighter
                  {1, 1, 10}, // amazon or whatever
                  {1, 1, 5},  // I dunno, a mtn wizard?
                  {1, 1, 1}};   // a hawkman?

// assign some colors to terrain
std::vector<sf::Color> terrain_colors;
for (int c = 0; c < NUM_TERRAIN_TYPES; c++){
    terrain_colors.push_back(new sf:Color(std::rand() % 256,
                                          std::rand() % 256,
                                          std::rand() % 256));
}

// Make some random terrain, yeah, doesn't make sense
for (int x = 0; x < X_MAP_SIZE; x++){
    battle_map.push_back(new std::vector<int>);
    for (int y = 0; y < Y_MAP_SIZE; y++){
        battle_map.back->push_back(std::rand() % NUM_TERRAIN_TYPES);
    }
}

#endif
