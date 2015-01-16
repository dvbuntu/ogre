CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -ggdb -fpermissive
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

#link step
ogre: main.o player.o ogre_obj.o hero.o unit.o town.o
	$(CXX) -o ogre main.o player.o ogre_obj.o hero.o unit.o town.o $(LIBS)

#compile main loop
main.o: main.cpp player.hpp ogre_obj.hpp hero.hpp unit.hpp town.hpp helper.hpp terrain.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

#compile helpers
player.o: player.cpp player.hpp 
	$(CXX) $(CXXFLAGS) -c player.cpp

ogre_obj.o: ogre_obj.cpp ogre_obj.hpp 
	$(CXX) $(CXXFLAGS) -c ogre_obj.cpp

hero.o: hero.cpp hero.hpp player.hpp ogre_obj.hpp helper.hpp
	$(CXX) $(CXXFLAGS) -c hero.cpp

unit.o: unit.cpp unit.hpp player.hpp ogre_obj.hpp hero.hpp helper.hpp
	$(CXX) $(CXXFLAGS) -c unit.cpp

town.o: town.cpp town.hpp player.hpp ogre_obj.hpp unit.hpp
	$(CXX) $(CXXFLAGS) -c town.cpp

#terrain.o: terrain.cpp terrain.hpp
#	$(CXX) $(CXXFLAGS) -c terrain.cpp

clean:
	rm -f *.o *~ ogre
