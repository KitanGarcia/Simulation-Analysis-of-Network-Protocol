#ifndef Eventlist_H
#define Eventlist_H
#include <iostream>
using namespace std;



class Event
{
public:
    int arrivalOrDeparture; //1 is arrival, 0 is departure, 2 is busy, 3 is timeout
    int ackorData;//1 is ack, 0 is data
    bool corrupt;//0 is not corrupt, 1 is corrupt
    bool check;//0 is check, 1 is don't check
    int sender;
    int receiver;
    double time;
    double size;
    Event* prev;
    Event* next;
};

class Eventlist
{
public:
    Event* head;
    Event* tail;
    void insertEvent(Event event);
    void removeEvent();
};


#endif
