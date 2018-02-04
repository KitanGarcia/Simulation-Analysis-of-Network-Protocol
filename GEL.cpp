#include <iostream>
#include "GEL.h"
using namespace std;



void GEL::insertEvent(Event event)
{
    Event *newEvent = new Event;
    newEvent->arrivalOrDeparture = event.arrivalOrDeparture;
    newEvent->time = event.time;
    newEvent->prev = NULL;
    newEvent->next = NULL;
    if (head == NULL)
    {
        head = newEvent;
    }
    else
    {
        Event* current = head;
        Event* previousEvent = NULL;
        while (current && current->time <= newEvent->time)//think this could be wrong
        {
            previousEvent = current;
            current = current->next;
        }
        if (current == NULL)// (current->next == NULL)
        {
            previousEvent->next = newEvent;
        }
        else
        {//insert before curr position
            newEvent->next = current;
            newEvent->prev = current->prev;
            current->prev = newEvent;
            if (previousEvent != NULL)
                previousEvent->next = newEvent;
            else
                head = newEvent;
        }
        /*   if(current->next != NULL) // if not the end of the list
         {
         newEvent->next = current->next;
         current->next = newEvent;
         newEvent->prev = current;
         (newEvent->next)->prev = newEvent;
         }*/
        cout << "Insertion success\n";
    }
}

void GEL::removeEvent() // Removing first element
{
    Event *temp = head;
    
    if (head->next != NULL)
    {
        delete(temp);
        head->next->prev = NULL;
        head = head->next;
    } else {
        delete(temp);
        head = NULL;
    }
    
}
