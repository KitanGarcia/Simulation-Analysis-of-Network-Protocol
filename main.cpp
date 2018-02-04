#include <iostream>
#include <cmath>
#include <cstdlib>
#include <queue>
#include "GEL.h"
using namespace std;


class Packet
{
public:
    int size;
    double servicetime;
};


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
    int packetsDropped = 0; //for packets in buffer and dropped
    int totalLength = 0;
    double time = 0;//current time
    double area = 0;
    double totalServerBusyTime = 0;
    double meanUtilization, meanQueueLength;
    bool aod;
    
    queue<Packet> buffer;//predefine queue
    GEL events;
    
    double u = 1; //service rate. Are service and transmission time the same?
    double lambda;
    cout << "Enter lambda value: " << endl;
    cin >> lambda;

    cout << "Enter lambda max buffer value: " << endl;
    cin >> MAXBUFFER;
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
        
        Packet packet;
        packet.servicetime = negative_exponentially_distributed_time(u);
        
        // PROCESSING ARRIVAL
        if (aod == 1)
        {
            // Schedule next arrival
            Event nextArrival;
            nextArrival.arrivalOrDeparture = 1;
            nextArrival.time = time + negative_exponentially_distributed_time(lambda);
            insertcounter++; cout << insertcounter; //TESTING
            events.insertEvent(nextArrival);
            
            //Process arrival event
            if (length == 0)
            {
                Event departure;
                departure.arrivalOrDeparture = 0;
                departure.time = time + packet.servicetime;
                insertcounter++; cout << insertcounter; //TESTING
                events.insertEvent(departure);//then add to queue inc length
                
                buffer.push(packet);
                length++;
                area += packet.servicetime * (length-1);
                totalServerBusyTime = totalServerBusyTime + packet.servicetime;
                
            } else if (length > 0) { //if server busy
                
                if ((length - 1 < MAXBUFFER) || (MAXBUFFER == -1))//if queue not full
                {
                    buffer.push(packet);
                    length++;
                    area += packet.servicetime * length;
                    totalServerBusyTime = totalServerBusyTime + packet.servicetime;
                } else if (length - 1 >= MAXBUFFER) { //if queue full
                    packetsDropped++;
                }
            }
            
 
            
        } else { //Processing departure
            
            
            
            if (length > 0)
            {
                buffer.pop();
                length--;
                Event departure;
                departure.arrivalOrDeparture = 0;
                departure.time = time + packet.servicetime;
                
                insertcounter++; cout << insertcounter;
                events.insertEvent(departure);
            }
        }
        
    }
    
    cout << "area " << area << endl;
    meanUtilization = totalServerBusyTime / time;
    meanQueueLength = area / time;
    
    
    cout << "length = " << length << endl;
    cout << "area = " << area << endl;
    cout << "total server busy time = " << totalServerBusyTime << endl;//99600
    cout << "time = " << time << endl;//3.91e-13
    cout << "mean utilization = " << meanUtilization << endl;//2.55e+17
    cout << "mean Queue Length = " << meanQueueLength << endl;
    cout << "Packets dropped = " << packetsDropped << endl;
    
    
    
    //MAXBUFFER = -1 to make it infinite

    
    /*
     1) Plot queuelength and sever utilization as function of lambda for lambda = .1, .25, .4, .55, .65, .8, .9 with infinite buffer size (MAXBUFFER = infinity?)
     2) Mathematically computer mean utlilizaiton and mean queue length and compare to that of simulation
     3) Plot totaly number of dropped packets as function of lambda for lambda = .2, .4, .6, .8, 9 for MAXBUFFER = 1, 20, and 50
     
     
     
     utilization: rho= lamda/mu
     
     meanqueuelength = rho^2/(1-rho)
     */
    
    return 0;
}

