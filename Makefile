CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -ggdb
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

#link step
ogre: main.o unit.o
	$(CXX) -o ogre main.o unit.o $(LIBS)

#compile main loop
#main.o: main.cpp car.hpp
main.o: main.cpp unit.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

#compile helpers
unit.o: unit.cpp unit.hpp
	$(CXX) $(CXXFLAGS) -c unit.cpp

clean:
	rm -f *.o ogre
