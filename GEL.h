#ifndef GEL_H
#define GEL_H
#include <iostream>
using namespace std;



class Event
{
public:
    bool arrivalOrDeparture; //1 is arrival, 0 is departure
    double time;
    Event* prev;
    Event* next;
};

class GEL
{
public:
    Event* head;
    Event* tail;
    void insertEvent(Event event);
    void removeEvent();
};


#endif
