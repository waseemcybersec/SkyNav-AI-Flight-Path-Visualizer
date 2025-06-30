#ifndef OPTIMAL_PATH_H
#define OPTIMAL_PATH_H

#include"advanceRoute.h"

//MinHeap class declaration
class MinHeap 
{
public:
    //heapNode class declaration as a member of MinHeap claass
    class HeapNode 
    {
    public:
        int vertex;
        double value;

        HeapNode()
        {
            vertex = -1;
            value = 0;
        }
        HeapNode(int v, double val)
        {
            vertex = v;
            value = val;
        }
    };

    HeapNode* heap;
    int capacity;
    int size;

    void swap(HeapNode& a, HeapNode& b);
    void heapify(int i);
    int parent(int i)
    {
        return (i - 1) / 2; 
    }
    int left_child(int i) 
    {
        return (2 * i + 1); 
    }
    int right_child(int i) 
    { 
        return (2 * i + 2); 
    }

    MinHeap(int cap);
    void insert(int vertex, double value);
    HeapNode extract_min();
    bool is_empty()
    {
        if (size == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    ~MinHeap();
};

//OptimalPath class declaration
class OptimalPath 
{
    allFlights* graph;
    const double INF = 999999.0; //larges value like for INFINITY

    class PathNode 
    {
    public:
        Flight* flight;
        int city_index;
        PathNode* next;
        PathNode()
        {
            flight = NULL;
            city_index = -1;
            next = NULL;
        }
    };

    int get_flight_time(Flight& flight);
    int calculate_total_time(Flight& leg1, Flight& leg2);
    bool check_date_sequence(string& date1, string& date2);
    void display_path(int dest, int* parent, Flight** flights, double total_value, bool by_price);
    void find_optimal_path(int from_index, int to_index, bool by_price);

public:
    OptimalPath(allFlights* g)
    {
        graph = g;
    }
    void find_flight();
    ~OptimalPath() {}
};

#endif