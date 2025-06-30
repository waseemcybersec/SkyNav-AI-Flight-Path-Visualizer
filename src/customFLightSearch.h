#ifndef CUSTOM_FLIGHT_H
#define CUSTOM_FLIGHT_H

#include"SFML.h"

class CustomFlightSearch 
{
private:
    allFlights* graph;

    bool matches_airline_preference(Flight& flight, string& preferred_airline) const;
    int convert_time_to_minutes(string& time) const;
    bool is_valid_date_sequence(string& date1, string& date2) const;
    bool is_feasible_connection(Flight& leg1, Flight& leg2) const;

    void display_connection(string& from_city, string& transit_city, string& to_city, Flight& leg1, Flight& leg2) const;

public:
   
    CustomFlightSearch(allFlights* g);
    //main start function
    void search_preferred_flights();

    bool find_direct_flights(string& from_city, string& to_city, string& preferred_airline) const;
    bool find_connecting_flights(string& from_city, string& to_city, string& preferred_airline, string& transit_city);
};

#endif
