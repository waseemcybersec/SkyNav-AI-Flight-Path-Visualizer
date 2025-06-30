#ifndef GRAPH_H
#define GRAPH_H

#include "Flight.h"

class Graph 
{
public:
    vertex* vertices;    
    int vertex_count;   
    int max_vertices;   

    Graph();
    void extract_data_from_file();
    int find_vertex_index(string& city);
    void add_vertex(string& city_name);
    void add_flight(string& from, string& to, Flight& flight);
    void read_hotel_prices();
    void display_graph() const;
    ~Graph();
};

#endif
