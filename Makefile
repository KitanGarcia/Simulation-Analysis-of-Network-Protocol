simulation.out : GEL.o main.o
	g++ -ansi -Wall -g -o simulation.out GEL.o main.o

GEL.o : GEL.cpp GEL.h
	g++ -ansi -Wall -g -c GEL.cpp

main.o : main.cpp
	g++ -ansi -Wall -g -c main.cpp

clean :
	rm -f simulation.out GEL.o main.o
