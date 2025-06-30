#ifndef LAYOVER_MANG_H
#define LAYOVER_MANG_H

#include"optimalPath.h"

class FlightInformation
{
public:
    Flight flight_info;
    string from_city;
    string to_city;
    int layover_minutes;

    FlightInformation(Flight f = Flight(), string from = "", string to = "", int lay = 0);
};

class QueueNode 
{
public:
    FlightInformation data;
    QueueNode* next;

    QueueNode(FlightInformation fi);
};

class LayoverQueue 
{
private:
    QueueNode* front, * rear;
    int size;

public:
    LayoverQueue();
    ~LayoverQueue();
    void enqueue(FlightInformation fi);
    FlightInformation dequeue();
    bool isEmpty() const;
    int getSize() const;
    void clear();
};

//layover management class basically an application of queue
class LayoverManager
{
private:
    allFlights* graph;
    LayoverQueue journey_queue;

    bool is_valid_date_sequence(string& date1, string& date2) const;
    int calculate_layover_time(Flight& first_leg, Flight& second_leg);
    int convert_time_to_minutes(string& time) const;
    void process_journey();
    void find_best_connection(int from_index, int to_index);

public:
    LayoverManager(allFlights* g);
    void manage_journey(string& from_city, string& to_city);
};

#endif