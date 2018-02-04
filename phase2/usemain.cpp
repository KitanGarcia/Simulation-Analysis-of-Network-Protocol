#include <iostream>
#include <cmath>
#include <cstdlib>
#include <queue>
#include "Eventlist.h"
using namespace std;



class Packet
{
public:
  double size;//in bytes
  double servicetime;
};


class Host
{
public:
  int length;//size of host's queue.
  queue<Packet> buffer;
  double ranBackoff;//each host has backoff time
  int backoffCounter;
  int attempts;//max 3
  double Ttrans;
  double Tqueue;
  int overallBuffer;//queuesize/totaltime
  //other has N, packets  dropped, and queuesize/totaltime
};


//Host communicates with random neighbor during transmission. hosts[srand()%20]
//^^If array of hosts called "hosts"
//each host has infinite FIFO buffer to hold outstanding frames




double negative_exponentially_distributed_time(double rate)//generates events
{
    double u;
    u = drand48();
    return ((-1/rate)*log(1-u));
}//u is uniformly distributed random variable between 0 and 1



int main()
{
  int MAXBUFFER;//max number of packets (can be infinity ie. no packets are dropped)
  int length = 0;//number packets in queue
  double time = 0;//current time
  double area = 0;
  double totalServerBusyTime = 0;
  int aod;
 




  double SIFS = .05;//Short interframe spacing: wait between time received and ACK sent (destination)
  double DIFS = .1;//Distributed interframe space: wait time between being idle and transmitting frame (sender)
  double CSMACA = .01;//Carrier Sense Multiple Access with Collision Avoidance
  int N; //number of hosts: 10 or 20
  double T;//user-defined. Test different values and find the best one
  double timeoutvalue;
  int numbytes = 0;
  double totaldelay;


  Eventlist events;//GEL
  Eventlist eventsintransit;


    
  double u = 1; //service rate. 
  double lambda;
  cout << "Enter lambda value: " << endl;
  cin >> lambda;
    
  cout << "Enter number hosts, 10 or 20: " << endl;
  cin >> N;
 
  cout << "Enter T-Value: " << endl;
  cin >> T;

  cout << "Enter time-out value: \n";
  cin >> timeoutvalue;

  MAXBUFFER = -1;

//to initialize hosts
  Host hosts[N];
  for (int i = 0; i < N; i++)
  {
    hosts[i].length = 0;
    hosts[i].ranBackoff = 0;
    hosts[i].Ttrans = 0;
    hosts[i].Tqueue = 0;
  }


  Event event1; // Create first event
  event1.arrivalOrDeparture = 1;
  event1.time = time + negative_exponentially_distributed_time(lambda);
  events.insertEvent(event1);
    
  int insertcounter = 0;
    
    
  //define arrivalOrDeparture or its equivalent
  for (int i = 0; i < 100000; i++)
  {
    if (events.head == NULL)//If GEL empty, break
        break;
        
    time = events.head->time;//Set current time
    aod = events.head->arrivalOrDeparture;
    events.removeEvent();
    


    Event currEvent;
    currEvent = *events.head;


    
    Packet packet;
    packet.servicetime = negative_exponentially_distributed_time(u);

    // PROCESSING ARRIVAL
    if (aod == 1)
    {
      // Schedule next arrival
      Event nextArrival;
      nextArrival.arrivalOrDeparture = 1;
      nextArrival.time = time + negative_exponentially_distributed_time(lambda);
      events.insertEvent(nextArrival);
      packet.size = ceil(1544 * negative_exponentially_distributed_time(u));
      //^or should it be right outside of "if (aod == 1)???"
 
      //Process arrival event
      //Randomize hosts...
      
      int randHost;
      randHost = rand() % N;
      if (hosts[randHost].length == 0)
      { 
        hosts[randHost].buffer.push(packet);
        Event departure;
        departure.arrivalOrDeparture = 0;
        departure.sender = randHost;
        departure.receiver = currEvent.receiver;
        departure.time = time + packet.servicetime;
        events.insertEvent(departure);//then add to queue inc length
 
        hosts[randHost].buffer.push(packet);
        hosts[randHost].length++;
        area += packet.servicetime * (length-1);
        totalServerBusyTime = totalServerBusyTime + packet.servicetime;
                
      } 
       else if (hosts[randHost].length > 0)
       { //if server busy
                
         if ((hosts[randHost].length - 1 < MAXBUFFER) || (MAXBUFFER == -1))//if queue not full
         {
           hosts[randHost].buffer.push(packet);
           hosts[randHost].length++;
           area += packet.servicetime * length;
           totalServerBusyTime = totalServerBusyTime + packet.servicetime;
         }
          
       }



//1 is ack, 0 is data
       //Process data packet arrival
       if (currEvent.ackorData == 0)
       {
         Event checkEvent = *eventsintransit.head;
         //Schedule ack departure
         if (checkEvent.corrupt == 0)
         {
           Event ackDepart;
           ackDepart.ackorData = 0;
           ackDepart.time = time + SIFS;
           ackDepart.size = currEvent.size;
           ackDepart.receiver = currEvent.receiver;
           events.insertEvent(ackDepart);
         }

         hosts[randHost].Ttrans = hosts[randHost].Ttrans + (currEvent.size*8/11000000);
         eventsintransit.removeEvent();
       }

       else if (currEvent.ackorData == 1)
       {
         //if there is only one event in eventsintransit
         if (eventsintransit.head != NULL && eventsintransit.head->next == NULL)
         {
           numbytes = numbytes + 64;
           numbytes = numbytes + currEvent.size;
           hosts[randHost].buffer.pop();
           hosts[randHost].length--;
           //Schedule departure for next even in queue
           Event departure;
           Packet packet2 = hosts[randHost].buffer.front();
           departure.time = time + packet2.servicetime;
           departure.receiver = currEvent.receiver;
           events.insertEvent(departure);
           hosts[randHost].attempts = 1;
         }

         else if (eventsintransit.head->next != NULL)//if it has more than 1
         {
           //placeholder; do nothing
         }

         hosts[randHost].Ttrans = hosts[randHost].Ttrans + 64*8/11000000;
       }

    }//close arrivals


    else if (aod == 0)
    { //Processing departure
 

      if (currEvent.check == 0)//check
      {
        if (hosts[currEvent.sender].length > 0)
        {
          if (eventsintransit.head == NULL)//if empty/channel free
          {
            Event nextArrival;
            nextArrival.receiver = currEvent.sender;
            nextArrival.time = time + (packet.size * 8)/11000000 + DIFS;
            nextArrival.size = packet.size;
            nextArrival.check = 1;//don't check
            events.insertEvent(nextArrival);

            //to see if corrupted
            if (eventsintransit.head != NULL)//if not empty
            {
              eventsintransit.insertEvent(nextArrival);

              Event tempevent;
              tempevent = *eventsintransit.head;
              tempevent.corrupt = 1;
              eventsintransit.head->corrupt = 1;
              while (tempevent.next != NULL)
              {
                *eventsintransit.head = tempevent;
                tempevent = *tempevent.next;
                tempevent.corrupt = 1;
                eventsintransit.head->corrupt = 1;
              }
            }

            else if (eventsintransit.head == NULL)//if empty
            {
              nextArrival.corrupt = 0;
              eventsintransit.insertEvent(nextArrival);
            }

                    //timeout
            Event timeout;
            timeout.receiver = currEvent.receiver;
            timeout.time = time + SIFS + packet.size*8/11000000
                         + 64*8/11000000;
            events.insertEvent(timeout);
          }

          else if (eventsintransit.head != NULL)//not free
          {
            hosts[currEvent.sender].ranBackoff = 
                       ceil(drand48() * T);
            Event channelsense;
            channelsense.receiver = currEvent.receiver;
            events.insertEvent(channelsense); 
          

         }

          else
          {
            cout << "error 2: data packet departure\n";
          }
        }
      }

      else if (currEvent.check == 1)//don't check
      {
        if (eventsintransit.head != NULL)//schedule ack arrivals
        {
          Event nextArrival;
          nextArrival.ackorData = 1;
          nextArrival.size = currEvent.size;
          nextArrival.sender = currEvent.receiver;
          nextArrival.receiver = currEvent.sender;
          nextArrival.time = time + 64*8/11000000;

          nextArrival.corrupt = 0;//to check for corruption
          eventsintransit.insertEvent(nextArrival);
        }

        else
        {
          if (eventsintransit.head != NULL)//if not empty
          {
            Event tempevent;
            tempevent = *eventsintransit.head;
            tempevent.corrupt = 1;
            eventsintransit.head->corrupt = 1;
            while (tempevent.next != NULL)
            {
              *eventsintransit.head = tempevent;
              tempevent = *tempevent.next;
              tempevent.corrupt = 1;
              eventsintransit.head->corrupt = 1;
            }
          }
        }
      }
    }

    else if (aod == 2)
    { //Processing channel sensing
      Event channelsensing;
      channelsensing.arrivalOrDeparture = 2;
      channelsensing.time = time + CSMACA;
      insertcounter++; cout << insertcounter;
      events.insertEvent(channelsensing);
      if (eventsintransit.head == NULL)//channel free
      {
        hosts[currEvent.sender].backoffCounter = hosts[currEvent.sender].backoffCounter - 1;
        if (hosts[currEvent.sender].ranBackoff == 0)//transmit packet
        {
          packet = hosts[currEvent.sender].buffer.front();
          time = currEvent.time;

          //create arrival for packet to new host
          Event nextArrival;
          nextArrival.receiver = currEvent.sender;
          nextArrival.time = time = packet.size*8/11000000 + DIFS;
          nextArrival.size = packet.size;
          nextArrival.ackorData = 1;
          events.insertEvent(nextArrival);

          //corruption check
          if (eventsintransit.head != NULL)//not empty
          {
            Event tempevent;
            tempevent = *eventsintransit.head;
            tempevent.corrupt = 1;
            eventsintransit.head->corrupt = 1;
            while (tempevent.next != NULL)
            {
              *eventsintransit.head = tempevent;
              tempevent = *tempevent.next;
              tempevent.corrupt = 1;
              eventsintransit.head->corrupt = 1;
            }
          }
          else if (eventsintransit.head == NULL)
          {
            nextArrival.corrupt = 0;
            eventsintransit.insertEvent(nextArrival);
          }
          //new timeout event
          Event timeout;
          timeout.receiver = currEvent.receiver;
          timeout.time = time = SIFS + packet.size*8/11000000
                       + 64*8/11000000;
          events.insertEvent(timeout);
        }
      }

      else if (eventsintransit.head != NULL)
      {
        Event channelsense;
        channelsense.receiver = currEvent.receiver;
        events.insertEvent(channelsense);
      }
    }

    else if (aod == 3)
    { //Processing timeout
      Event timeout;
      timeout.arrivalOrDeparture = 3;
      timeout.time = time + timeoutvalue;
      insertcounter++; cout << insertcounter;
      events.insertEvent(timeout);

      hosts[currEvent.sender].backoffCounter = hosts[currEvent.sender].backoffCounter + 1;
      Packet packet;
      packet = hosts[currEvent.sender].buffer.front();
      Event departure;
      departure.time = time + packet.servicetime;
      departure.receiver = currEvent.receiver;
      departure.check = 0;
    }
    events.removeEvent();
  }

  for (int i = 0; i < N; i++)
  {
    totaldelay = totaldelay + hosts[i].Ttrans + hosts[i].Tqueue;
  }
  cout << "Throughput: " << numbytes / time << endl;
  cout << "Average Network Delay: " << totaldelay / (numbytes / time) << endl;

  return 0;
} 




