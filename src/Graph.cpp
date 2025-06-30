#include "Graph.h"

Graph::Graph()
{
    max_vertices = 10; //first we will assign a small size to the array
    vertex_count = 0;
    vertices = new vertex[max_vertices];
}

//finding vertex index by city name
int Graph::find_vertex_index(string& city)
{
    for (int i = 0; i < vertex_count; i++) 
    {
        if (vertices[i].city_name == city) 
        {
            return i;
        }
    }
    return -1;
}
//adding of vertex
void Graph:: add_vertex(string& city_name) 
{
    //if a city already exist
    if (find_vertex_index(city_name) != -1) //-1 means that the city is still not included 
    {
        return;
    }
    //resizing the array
    if (vertex_count == max_vertices) 
    {
        max_vertices += 5;
        vertex* new_vertices = new vertex[max_vertices];
        for (int i = 0; i < vertex_count; i++) 
        {
            new_vertices[i] = vertices[i];
        }
        delete[] vertices;
        vertices = new_vertices;
    }
    //adding now new vertic
    vertices[vertex_count].city_name = city_name;
    vertex_count++;
}
// Addition of flights as edges between cities
void Graph:: add_flight(string& from, string& to, Flight& flight) 
{
    int from_index = find_vertex_index(from);
    int to_index = find_vertex_index(to);

    if (from_index == -1 || to_index == -1) 
    {
        cout << "City not found!" << endl;
        return;
    }

    Node* new_node = new Node;
    new_node->dest_index = to_index;
    new_node->data = flight;

    //checking for destination city if already exist
    Node* current = vertices[from_index].adj_list;
    while (current != NULL) 
    {
        if ((current->dest_index) == to_index) 
        {
            //then make the linked list of flight for same destination and origin
            Node* flight_curr = current;
            while (flight_curr->flight_next != NULL) 
            {
                flight_curr = flight_curr->flight_next;
            }
            flight_curr->flight_next = new_node;
            return;
        }
        current = current->next;
    }

    //if destination not exitst then add new destinatiion as vertix
    //adding at start of the list
    new_node->next = vertices[from_index].adj_list;
    vertices[from_index].adj_list = new_node;
}

//extracting data from file
void Graph:: extract_data_from_file() 
{
    ifstream file("Flights.txt");
    if (!file) 
    {
        cout << "file not opened!" << endl;
        return;
    }

    string origin, destination, date, departure, arrival, airline;
    double price;
    //for cities adding as vertices.
    while (file >> origin >> destination >> date >> departure >> arrival >> price >> airline) 
    {
        add_vertex(origin);
        add_vertex(destination);
    }
    //reset to beginning
    file.clear();
    file.seekg(0);

    //now passing the flight information
    while (file >> origin >> destination >> date >> departure >> arrival >> price >> airline) 
    {
        Flight f(airline, date, departure, arrival, price);
        add_flight(origin, destination, f);
    }

    file.close();
}

//reading hotels price data
void Graph::read_hotel_prices()
{
    ifstream file("HotelCharges_perday.txt");
    if (!file) 
    {
        cout << "file not found!" << endl;
        return;
    }

    string city;
    double price;
    while (file >> city >> price) 
    {
        int index = find_vertex_index(city);
        if (index != -1) 
        {
            vertices[index].hotel_price = price;
        }
    }
    file.close();
}

//display graph
void Graph:: display_graph() const 
{
    for (int i = 0; i < vertex_count; i++) 
    {
        cout << "\nCity: " << vertices[i].city_name << endl;
        cout << "Hotel Price: Rs. " << vertices[i].hotel_price << " per day" << endl;

        Node* current = vertices[i].adj_list;
        if (current == NULL) 
        {
            cout << "No outgoing flights" << endl;
            continue;
        }

        cout << "\nOutgoing Flights : " << endl;
        cout << "----------------" << endl;

        while (current != NULL) 
        {
            cout << "\nTo: " << vertices[current->dest_index].city_name << endl;

            Node* flight = current;
            int flight_num = 1;
            while (flight != NULL) 
            {
                cout << "\nFlight " << flight_num++ << ":" << endl;
                cout << "----------" << endl;
                flight->data.display_flight();
                flight = flight->flight_next;
            }
            current = current->next;
        }
    }
}
Graph::~Graph() 
{
    for (int i = 0; i < vertex_count; i++) 
    {
        Node* current = vertices[i].adj_list;
        while (current != nullptr) 
        {
            Node* temp = current;
            current = current->next;

            //free flight information data
            Node* flight = temp->flight_next;
            while (flight != NULL) 
            {
                Node* flight_temp = flight;
                flight = flight->flight_next;
                delete flight_temp;
            }
            delete temp;
        }
    }
    delete[] vertices;
}