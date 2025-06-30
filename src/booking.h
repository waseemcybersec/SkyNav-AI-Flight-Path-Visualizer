#ifndef BOOKING_H
#define BOOKING_H

#include"layoverMang.h"

class BookFlight 
{
    allFlights* graph;
    int time_to_minutes(string& time) const;

    int time_difference(string& time1, string& time2) const;

    void display_direct_flights(int from_index, int to_index) const;

    void display_connecting_flights(int from_index, int to_index) const;

public:
    BookFlight(allFlights* g);

    void book_flight();
};

#endif
