#include "optimalPath.h"

//MinHeap class definitions
MinHeap::MinHeap(int cap) 
{
    capacity = cap;
    size = 0;
    heap = new HeapNode[capacity];
}

void MinHeap::swap(HeapNode& a, HeapNode& b)
{
    HeapNode temp = a;
    a = b;
    b = temp;
}

void MinHeap::heapify(int i) 
{
    int smallest = i;
    int left = left_child(i);
    int right = right_child(i);

    if (left < size && heap[left].value < heap[smallest].value)
    {
        smallest = left;
    }
    if (right < size && heap[right].value < heap[smallest].value)
    {
        smallest = right;
    }

    if (smallest != i) 
    {
        swap(heap[i], heap[smallest]);
        heapify(smallest);
    }
}

void MinHeap::insert(int vertex, double value) 
{
    if (size >= capacity)
    {
        return;
    }
    size++;
    int i = size - 1;
    heap[i].vertex = vertex;
    heap[i].value = value;

    while (i != 0 && heap[parent(i)].value > heap[i].value) 
    {
        swap(heap[i], heap[parent(i)]);
        i = parent(i);
    }
}

MinHeap::HeapNode MinHeap::extract_min() 
{
    if (size <= 0)
    {
        return HeapNode(); //return dummy heapNode;
    }
    if (size == 1) 
    {
        size--;
        return heap[0];
    }

    HeapNode root = heap[0];
    heap[0] = heap[size - 1];
    size--;
    heapify(0);

    return root;
}

MinHeap::~MinHeap() 
{
    delete[] heap;
}

//--------------------------------------------------------------------------------

// OptimalPath class definitons

int OptimalPath::get_flight_time(Flight& flight) 
{
    //departure time
    int dep_hours = stoi(flight.departure_time.substr(0, 2));
    int dep_mins = stoi(flight.departure_time.substr(3, 2));

    //arrival time
    int arr_hours = stoi(flight.arrival_time.substr(0, 2));
    int arr_mins = stoi(flight.arrival_time.substr(3, 2));

    int total_mins = (arr_hours * 60 + arr_mins) - (dep_hours * 60 + dep_mins);

    if (total_mins < 0)
    {
        total_mins += 24 * 60; //check for overnight flights time
    }
    return total_mins;
}

bool OptimalPath::check_date_sequence(string& date1, string& date2) 
{
    int day1 = stoi(date1.substr(0, 2));

    int day2 = stoi(date2.substr(0, 2));

    if (day2 >= day1)
    {
        return true; //date check same date or subsequent date
    }
    else
    {
        return false;
    }
}

int OptimalPath::calculate_total_time(Flight& leg1, Flight& leg2) 
{
    if (!check_date_sequence(leg1.flight_date, leg2.flight_date)) 
    {
        return -1; //invalid date sequence
    }

    int day1 = stoi(leg1.flight_date.substr(0, 2));

    int day2 = stoi(leg2.flight_date.substr(0, 2));

    //calculating arrival and departure time
    int arr_time1 = stoi(leg1.arrival_time.substr(0, 2)) * 60 + stoi(leg1.arrival_time.substr(3, 2));
    int dep_time2 = stoi(leg2.departure_time.substr(0, 2)) * 60 + stoi(leg2.departure_time.substr(3, 2));

    int layover_time;
    if (leg1.flight_date == leg2.flight_date) 
    {
        //if same day connection
        layover_time = dep_time2 - arr_time1;
        if (layover_time < 60)
        {
            return -1; //invalid connection
        }
    }
    else 
    {
        //calculate days difference
        int days_diff;
        days_diff = day2 - day1;
        
        //remaining time of 1st day + full day in between + time untill last day departure time
        layover_time = (24 * 60 - arr_time1) + ((days_diff - 1) * 24 * 60) + dep_time2;
    }

    // total time = first flight + layover + second flight
    int total_time = get_flight_time(leg1) + layover_time + get_flight_time(leg2);
    return total_time;
}

void OptimalPath::find_optimal_path(int from_index, int to_index, bool by_price) 
{
    //boundry check
    if (from_index < 0 || to_index < 0 || from_index >= graph->vertex_count || to_index >= graph->vertex_count) 
    {
        cout << "Invalid city indices!" << endl;
        return;
    }

    //initializing arrays for dijkstra
    double* distance = new double[graph->vertex_count]; //for storing the distance at each vertex (distance in this case is price or time)
    int* parent = new int[graph->vertex_count];  //for reconstruct of the path at last
    bool* visited = new bool[graph->vertex_count]; //mark as visted if we have done with a vertex
    Flight** best_flights = new Flight * [graph->vertex_count]; //array of pointer to storing the flight

    for (int i = 0; i < graph->vertex_count; i++) 
    {
        distance[i] = INF;
        parent[i] = -1;
        visited[i] = false;
        best_flights[i] = NULL;
    }

    distance[from_index] = 0;
    MinHeap heap(graph->vertex_count * graph->vertex_count); //setting MinHeap capacity
    heap.insert(from_index, 0); //inserting into the heap

    //dijkstra algorithm main loop
    while (!heap.is_empty()) 
    {
        MinHeap::HeapNode current = heap.extract_min();
        int u = current.vertex;

        if (visited[u])
        {
            continue;
        }
        visited[u] = true;

        Node* adj = graph->vertices[u].adj_list;
        while (adj != NULL) 
        {
            int v = adj->dest_index;
            Node* flight = adj;

            while (flight != NULL) 
            {
                bool is_feasible = true;
                double edge_weight;

                if (best_flights[u] != NULL) 
                {
                    //check connecting flight feasibility
                    int total_time = calculate_total_time(*best_flights[u], flight->data);
                    is_feasible = (total_time != -1);

                    if (is_feasible) 
                    {
                        if (by_price) //if we are checking on price
                        {
                            edge_weight = flight->data.ticket_price;
                        }
                        else //if we are checking on time
                        {
                            edge_weight = total_time;
                        }
                    }
                }
                else
                {
                    if (by_price)
                    {
                        edge_weight = flight->data.ticket_price;
                    }
                    else 
                    {
                        edge_weight = get_flight_time(flight->data);
                    }

                }

                if (is_feasible && !visited[v] && distance[u] + edge_weight < distance[v]) 
                {
                    distance[v] = distance[u] + edge_weight;
                    parent[v] = u;
                    if (best_flights[v] != NULL)
                    {
                        delete best_flights[v];
                    }
                    best_flights[v] = new Flight(flight->data);
                    heap.insert(v, distance[v]);
                }
                flight = flight->flight_next;
            }
            adj = adj->next;
        }
    }
    //display result if path found
    if (distance[to_index] != INF) 
    {
        graph->runShortestPath(to_index,parent,best_flights,distance[to_index],by_price);
        display_path(to_index, parent, best_flights, distance[to_index], by_price);
    }
    else 
    {
        cout << "\nNo feasible path found between the cities" << endl;
    }

    //cleanup the memory
    for (int i = 0; i < graph->vertex_count; i++) 
    {
        if (best_flights[i] != NULL)
        {
            delete best_flights[i];
        }
    }
    delete[] best_flights;
    delete[] distance;
    delete[] parent;
    delete[] visited;
}

void OptimalPath::display_path(int dest, int* parent, Flight** flights, double total_value, bool by_price) 
{
    //first get the path
    int current = dest;
    int path_length = 0;
    int* path = new int[graph->vertex_count];

    while (current != -1) //whenever the source city comes for which the parent is -1 it will terminate
    {
        path[path_length++] = current;
        current = parent[current];
    }

    //display city sequence
    cout << "\nRoute: ";
    for (int i = path_length - 1; i >= 0; i--) 
    {
        cout << graph->vertices[path[i]].city_name;
        if (i > 0)
        {
            cout << " -> ";
        }
    }
    cout << endl << endl;

    //now display flight detailss
    for (int i = path_length - 1; i > 0; i--) 
    {
        cout << "Flight " << path_length - i << ":" << endl;
        cout << "---------" << endl;
        flights[path[i - 1]]->display_flight();
    }

    //display total
    cout << "\nTotal ";
    if (by_price) 
    {
        cout << "Cost: Rs. " << total_value << endl;
    }
    else 
    {
        cout << "Time: " << total_value << " minutes" << endl;
    }
    cout << endl;
    delete[] path;
}

void OptimalPath::find_flight()
{
    string from_city, to_city;
    cout << "\nEnter origin city: ";
    cin >> from_city;
    cout << "Enter destination city: ";
    cin >> to_city;

    int from_index = graph->find_vertex_index(from_city);
    int to_index = graph->find_vertex_index(to_city);

    if (from_index == -1 || to_index == -1) 
    {
        cout << "One or both cities not found!" << endl;
        return;
    }

    cout << "\nChoose optimal path criteria:" << endl;
    cout << "1. For Price" << endl;
    cout << "2. For Time" << endl;
    cout << "Enter your choice (1/2): ";

    int choice;
    cin >> choice;
    bool by_price = (choice == 1); //if choice 1 then this is true

    find_optimal_path(from_index, to_index, by_price);
}