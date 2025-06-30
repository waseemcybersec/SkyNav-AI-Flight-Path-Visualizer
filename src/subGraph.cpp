#include "subGraph.h"

AirlineQuery::AirlineQuery(allFlights* g)
{
    main_graph = g;
    max_vertices = main_graph->vertex_count; //max. possible vertices
    vertex_count = 0;
    vertices = new vertex[max_vertices];
}

void AirlineQuery::create_airline_subgraph(string& airline)
{
    vertex_count = 0;

    //find and add the cities served by the airline
    for (int i = 0; i < main_graph->vertex_count; i++)
    {
        Node* current = main_graph->vertices[i].adj_list;
        bool city_added = false;

        //checking outgoing flights
        while (current != NULL && !city_added)
        {
            Node* flight = current;
            while (flight != NULL)
            {
                if (flight->data.airline_name == airline)
                {
                    //add city to the sub graph
                    vertices[vertex_count].city_name = main_graph->vertices[i].city_name;
                    vertices[vertex_count].hotel_price = main_graph->vertices[i].hotel_price;
                    vertices[vertex_count].adj_list = NULL;
                    vertex_count++;
                    city_added = true;
                    break;
                }
                flight = flight->flight_next;
            }
            current = current->next;
        }
    }

    //now add flights between cities for this airline
    for (int i = 0; i < vertex_count; i++)
    {
        string from_city = vertices[i].city_name;
        int main_from_index = main_graph->find_vertex_index(from_city);

        Node* current = main_graph->vertices[main_from_index].adj_list;
        while (current != NULL)
        {
            Node* flight = current;
            while (flight != NULL)
            {
                if (flight->data.airline_name == airline)
                {
                    string to_city = main_graph->vertices[current->dest_index].city_name;
                    int to_index = find_vertex_in_subgraph(to_city);

                    if (to_index != -1)
                    {
                        Node* new_node = new Node();
                        new_node->dest_index = to_index;
                        new_node->data = flight->data;

                        //add to adj list
                        new_node->next = vertices[i].adj_list;
                        vertices[i].adj_list = new_node;
                    }
                }
                flight = flight->flight_next;
            }
            current = current->next;
        }
    }
}

int AirlineQuery::find_vertex_in_subgraph(string& city)
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

void AirlineQuery::display_subgraph() const
{
    cout << "\nAirline Subgraph information" << endl;
    cout << "===========================" << endl;
    cout << "Total cities served: " << vertex_count << endl;

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

        cout << "\nOutgoing Flights:" << endl;
        cout << "----------------" << endl;

        while (current != NULL)
        {
            cout << "\nTo: " << vertices[current->dest_index].city_name << endl;
            current->data.display_flight();
            current = current->next;
        }
    }
}

AirlineQuery::~AirlineQuery()
{
    for (int i = 0; i < vertex_count; i++)
    {
        Node* current = vertices[i].adj_list;
        while (current != NULL)
        {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] vertices;
}
