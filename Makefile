CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -ggdb
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

#link step
ogre: main.o #car.o
	$(CXX) -o ogre main.o $(LIBS)
	#$(CXX) -o sim main.o car.o $(LIBS)

#compile main loop
#main.o: main.cpp car.hpp
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

#compile helpers
#car.o: car.cpp car.hpp
#	$(CXX) $(CXXFLAGS) -c car.cpp

clean:
	rm -f *.o sim
