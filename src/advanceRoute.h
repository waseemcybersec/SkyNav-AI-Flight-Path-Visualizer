#ifndef ADVANCED_ROUTE_H
#define ADVANCED_ROUTE_H

#include"subGraph.h"

class RouteInformation 
{
public:
    Flight flight_info; //for flight information
    string from_city;
    string to_city;
    int layover_minutes;
    bool is_valid;

    RouteInformation(Flight f = Flight(), string from = "", string to = "", int lay = 0, bool valid = true);
};

class RouteNode
{
public:
    RouteInformation data;
    RouteNode* next;

    RouteNode(RouteInformation ri);
};

class TransitCity //for transit cities (if multiple transit cities)
{
public:
    string city_name;
    TransitCity* next;

    TransitCity(string name);
};

class AdvancedRouteGenerator
{
private:
    allFlights* graph;
    RouteNode* route_head;
    TransitCity* transit_head;
    int transit_count;

    bool is_valid_date_sequence(string& date1, string& date2) const;
    int calculate_layover_time(Flight& first_leg, Flight& second_leg);
    int convert_time_to_minutes(string& time) const;
    Flight find_best_flight(int from_index, int to_index);
    void add_transit_city(string city);
    void clear_transit_cities();
    void clear_route();
    void add_route_segment(RouteInformation ri);
    void display_route();
    bool verify_cities(string& origin, string& destination);

public:
    AdvancedRouteGenerator(allFlights* g);
    void generate_route(); //called from main ftn
    ~AdvancedRouteGenerator();
};

#endif 
