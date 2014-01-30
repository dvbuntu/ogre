#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdlib>
#include <SFML/Graphics.hpp>

#ifndef NO_PLAYER
#define NO_PLAYER 0
#endif

#ifndef PLAYER
#define PLAYER 1
#endif

#ifndef ENEMY
#define ENEMY 2
#endif

#ifndef MIN_START_GOLD
#define MIN_START_GOLD 50
#endif

#ifndef TAX_RATE
#define TAX_RATE 1
#endif

#ifndef PAY_RATE
#define PAY_RATE 2
#endif

class OgrePlayer{
    int id;
    int gold;
    int num_towns;
    int num_units;
    sf::Color color;

public:
    OgrePlayer(const int new_id);

    // Who am I?
    inline int get_id() const
    {
        return id;
    }

    // How rich am I?
    inline int get_gold() const
    {
        return gold;
    }

    // Make a deposit or withdrawal
    inline void set_gold(int new_gold)
    {
        if (new_gold < 0)
            gold = 0;
        else
            gold = new_gold;
    }

    // How many houses do I own?
    inline int get_num_towns() const
    {
        return num_towns;
    }

    // Real estate change
    inline void set_num_towns(int new_num_towns)
    {
        num_towns = new_num_towns;
    }

    // The Taxman cometh
    inline void collect_taxes()
    {
        set_gold(get_gold() + get_num_towns()*TAX_RATE);
    }

    // How rich am I?
    inline int get_num_units() const
    {
        return num_units;
    }

    // Make a deposit or withdrawal
    inline void set_num_units(int new_num_units)
    {
        num_units = new_num_units;
    }

    // Pay the soldier - now happens in unit.hpp: collect_pay()
    //inline void pay_troops()
    //{
    //    set_gold(get_gold() - get_num_units()*PAY_RATE);
    //}

    // What do I wear?
    sf::Color get_color() const
    {
        return color;
    }

    friend bool operator== (const OgrePlayer& A, const OgrePlayer& B);
};

#endif
