#ifndef SUBGRAPH_H
#define SUBGRAPH_H

#include"customFLightSearch.h"

class AirlineQuery
{
private:
    allFlights* main_graph;
    vertex* vertices;
    int vertex_count;
    int max_vertices;

public:
    AirlineQuery(allFlights* g);
    void create_airline_subgraph(string& airline);
    int find_vertex_in_subgraph(string& city);
    void display_subgraph() const;
    ~AirlineQuery();
};

#endif 
