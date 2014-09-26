#include "ogre_obj.hpp"

PathPt::PathPt (int x, int y) : sf::Vector2i(x, y){}
PathPt::PathPt (sf::Vector2i vec) : sf::Vector2i(vec){}
PathPt::PathPt (const PathPt& old) : sf::Vector2i(old.x, old.y){
        set_G(old.G);
        set_F(old.F);
}

// Need this as a function for sorting and shouldn't be in helper
bool compare_F(PathPt *first, PathPt *second)
{
    return first->get_F() < second->get_F();
}

OgreObject::OgreObject(const sf::Vector2f& p)
	: circ(UNIT_SIZE) // radius of the circle
{
    // Fix to center and Set the initial position
    circ.setOrigin(UNIT_SIZE,UNIT_SIZE);
	circ.setPosition(p);
	circ.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

    // Start not moving, so target is our current position
    // Apparently you're not allowed to use a method to do this during init
    target_position = p;

    path = new std::list<PathPt*>;

    // temporary
    speed = 1;

}
