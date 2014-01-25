CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -ggdb -fpermissive
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

#link step
ogre: main.o player.o unit.o town.o
	$(CXX) -o ogre main.o player.o unit.o town.o $(LIBS)

#compile main loop
#main.o: main.cpp car.hpp
main.o: main.cpp player.hpp unit.hpp town.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

#compile helpers
player.o: player.cpp player.hpp
	$(CXX) $(CXXFLAGS) -c player.cpp

unit.o: unit.cpp unit.hpp player.hpp
	$(CXX) $(CXXFLAGS) -c unit.cpp

town.o: town.cpp town.hpp player.hpp unit.hpp
	$(CXX) $(CXXFLAGS) -c town.cpp

clean:
	rm -f *.o ogre
