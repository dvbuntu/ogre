#ifndef TERRAIN_HPP
#define TERRAIN_HPP

// this could all be refactored to be more c++ like rather than C
#include <cstdlib>

// Will need to translate all clicks into map_size chunks
#ifndef X_MAP_SIZE
#define X_MAP_SIZE 100
#endif
#ifndef Y_MAP_SIZE
#define Y_MAP_SIZE 100
#endif

int battle_map[X_MAP_SIZE][Y_MAP_SIZE];


#ifndef NUM_TERRAIN_TYPES
#define NUM_TERRAIN_TYPES 3
#endif

// info about how each unit type moves through each terrain type
// terrain is [plains, hills, mtn]
int move_cost = { {1, 2, 10}, // Nominally, fighter
                  {1, 1, 10}, // amazon or whatever
                  {1, 1, 5},  // I dunno, a mtn wizard?
                  {1, 1, 1}}   // a hawkman?

for (int x = 0; x < X_MAP_SIZE; x++){
    for (int y = 0; y < Y_MAP_SIZE; y++){
        battle_map[x][y] = std::rand() % NUM_TERRAIN_TYPES;
    }
}
#endif
