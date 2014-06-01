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

#ifndef NUM_TERRAIN_TYPES
#define NUM_TERRAIN_TYPES 3
#endif

// info about how each unit type moves through each terrain type
std::vector< std::vector<int> > move_cost;

// assign some colors to terrain
std::vector<sf::Color> terrain_colors;

// terrain is [plains, hills, mtn]
void setup_move_cost(){
    move_cost.push_back(new std::vector<int>);
    move_cost.back.push_back(1);// Nominally, fighter
    move_cost.back.push_back(2);
    move_cost.back.push_back(10);
    move_cost.push_back(new std::vector<int>);
    move_cost.back.push_back(1);// amazon or whatever
    move_cost.back.push_back(1);
    move_cost.back.push_back(10);
    move_cost.push_back(new std::vector<int>);
    move_cost.back.push_back(1);// I dunno, a mtn wizard?
    move_cost.back.push_back(1);
    move_cost.back.push_back(5);
    move_cost.push_back(new std::vector<int>);
    move_cost.back.push_back(1);// a hawkman?
    move_cost.back.push_back(1);
    move_cost.back.push_back(1);
}

void setup_terrain_colors(){
    for (int c = 0; c < NUM_TERRAIN_TYPES; c++){
        terrain_colors.push_back(new sf:Color(std::rand() % 256,
                                              std::rand() % 256,
                                              std::rand() % 256));
    }
}

// Make some random terrain, yeah, doesn't make sense
void setup_battle_map(){
    for (int x = 0; x < X_MAP_SIZE; x++){
        battle_map.push_back(new std::vector<int>);
        for (int y = 0; y < Y_MAP_SIZE; y++){
            battle_map.back->push_back(std::rand() % NUM_TERRAIN_TYPES);
        }
    }
}

void setup_terrain(){
    setup_move_cost();
    setup_terrain_colors();
    setup_battle_map();
}

#endif
