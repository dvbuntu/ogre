#include <cstdlib>
#include <iterator>
#include <SFML/Graphics.hpp>
// #include <SFML/system.hpp>

#ifndef HELPER_HPP
#define HELPER_HPP

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

// Points on the shortest path, maybe change to grid pt
class PathPt: public sf::Vector2i{
    // cost to this point
    int G;

    // estimated cost to end
    int F;

public:
    PathPt (int x, int y);
    PathPt (const PathPt& old) : sf::Vector2i(old.x, old.y){
        G = old.G;
        F = old.F;
    }

    inline int get_G()
    {
        return G;
    }

    inline void set_G(int newG)
    {
        G = newG;
    }

    inline int get_F()
    {
        return F;
    }

    inline void set_F(int newF)
    {
        F = newF;
    }

    int diag_dist(PathPt *target)
    {
        int del_x = abs(x - target->x);
        int del_y = abs(y - target->y);
        return 4*std::min(del_x, del_y) + 10*std::max(del_x, del_y);
    }

    // where did we come from?
    PathPt *parent;
};

bool compare_F(PathPt *first, PathPt *second)
{
    return first->get_F() < second->get_F();
}

#endif
