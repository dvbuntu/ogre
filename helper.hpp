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


#endif
