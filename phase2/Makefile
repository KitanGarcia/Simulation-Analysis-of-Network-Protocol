simulation.out : Eventlist.o main.o
	g++ -ansi -Wall -g -o simulation.out Eventlist.o main.o

Eventlist.o : Eventlist.cpp Eventlist.h
	g++ -ansi -Wall -g -c Eventlist.cpp

main.o : main.cpp
	g++ -ansi -Wall -g -c main.cpp

clean :
	rm -f simulation.out Eventlist.o main.o
