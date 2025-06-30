#include "LayoverMang.h"

FlightInformation::FlightInformation(Flight f, string from, string to, int lay)
{
    flight_info = f;
    from_city = from;
    to_city = to;
    layover_minutes = lay;
}

QueueNode::QueueNode(FlightInformation fi)
{
    data = fi;
    next = NULL;
}
//layoverQueue class definition
LayoverQueue::LayoverQueue()
{
    front = rear = NULL;
    size = 0;
}

LayoverQueue::~LayoverQueue() 
{
    clear(); //directly clear by clear ftn
}

void LayoverQueue::enqueue(FlightInformation fi) 
{
    QueueNode* newNode = new QueueNode(fi);
    if (rear == NULL)
    {
        front = rear = newNode;
    }
    else 
    {
        rear->next = newNode;
        rear = newNode;
    }
    size++;
}

FlightInformation LayoverQueue::dequeue()
{
    if (front == NULL)
    {
        return FlightInformation();
    }
    QueueNode* temp = front;
    FlightInformation fi = temp->data;
    front = front->next;
    if (front == NULL)
    {
        rear = NULL;
    }
    delete temp;
    size--;
    return fi;
}

bool LayoverQueue::isEmpty() const 
{
    if (front == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int LayoverQueue::getSize() const 
{
    return size;
}

void LayoverQueue::clear() 
{
    while (front != NULL)
    {
        QueueNode* temp = front;
        front = front->next;
        delete temp;
    }
    rear = nullptr;
    size = 0;
}

//layoverManager class definitions
LayoverManager::LayoverManager(allFlights* g)
{
    graph = g;
}

bool LayoverManager::is_valid_date_sequence(string& date1, string& date2) const 
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

int LayoverManager::convert_time_to_minutes(string& time) const 
{
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    int total_time = hours * 60 + minutes;
    return total_time;
}
int LayoverManager::calculate_layover_time(Flight& first_leg, Flight& second_leg) 
{
    if (!is_valid_date_sequence(first_leg.flight_date, second_leg.flight_date)) 
    {
        return -1;
    }

    if (first_leg.flight_date == second_leg.flight_date) 
    {
        int arrival = convert_time_to_minutes(first_leg.arrival_time);

        int departure = convert_time_to_minutes(second_leg.departure_time);

        int layover = departure - arrival;

        if (layover >= 60)
        {
            return layover;
        }
        else
        {
            return -1;
        }
    }
    else 
    {
        int day1 = stoi(first_leg.flight_date.substr(0, 2));
        
        int day2 = stoi(second_leg.flight_date.substr(0, 2));
       
       int days_diff = day2 - day1;
        
        int arrival = convert_time_to_minutes(first_leg.arrival_time);
        int departure = convert_time_to_minutes(second_leg.departure_time);

        //same line also implemented in dijkastra algo.
        int layover = (24 * 60 - arrival) + ((days_diff - 1) * 24 * 60) + departure;
        return layover;
    }
}

void LayoverManager::process_journey()
{
    cout << "\nDetailed Journey Information:" << endl;
    cout << "=============================" << endl;

    double total_cost = 0;
    int total_layover = 0;

    while (!journey_queue.isEmpty()) 
    {
        FlightInformation segment = journey_queue.dequeue();
        cout << "\nFlight Details:" << endl;
        cout << "From: " << segment.from_city << " To: " << segment.to_city << endl;
        cout << "--------------------------" << endl;
        segment.flight_info.display_flight();

        total_cost += segment.flight_info.ticket_price;

        if (segment.layover_minutes > 0) 
        {
            total_layover += segment.layover_minutes;
            cout << "\nLayover Information :" << endl;
            cout << "Duration : " << segment.layover_minutes / 60 << " hours ";
            cout << segment.layover_minutes % 60 << " minutes" << endl;

            if (segment.layover_minutes > 360)
            {
                int city_index = graph->find_vertex_index(segment.to_city);
                cout << "Extended Layover - Hotel Available at " << graph->vertices[city_index].city_name << endl;
                cout << "Hotel Rate at " << segment.to_city << ": Rs. ";
                cout << graph->vertices[city_index].hotel_price << " per day" << endl;
            }
        }
        cout << "==================================" << endl;
    }

    cout << "\nJourney Summary:" << endl;
    cout << "Total Cost : Rs. " << total_cost << endl;
    if (total_layover > 0)//if there is connecting flight
    {
        cout << "Total Layover: " << total_layover / 60 << " hours " << total_layover % 60 << " minutes" << endl;
    }
}

void LayoverManager::manage_journey(string& from_city, string& to_city)
{
    int from_index = graph->find_vertex_index(from_city);
    int to_index = graph->find_vertex_index(to_city);

    if (from_index == -1 || to_index == -1) 
    {
        cout << "Cities not found!" << endl;
        return;
    }

    Node* current = graph->vertices[from_index].adj_list;
    bool found = false;

    while (current != NULL && !found) 
    {
        if (current->dest_index == to_index)
        {
            found = true;
            FlightInformation fi(current->data, from_city, to_city);
            journey_queue.enqueue(fi);
            process_journey(); 
        }
        current = current->next;
    }

    if (!found) //if there is connecting flight
    {
        graph->runLayover(from_city, to_city);
        find_best_connection(from_index, to_index);
    }
}

//for connecting flights
void LayoverManager::find_best_connection(int from_index, int to_index)
{
    double best_total_cost = INT_MAX; //intialize with arbit. larger value
    Flight best_first_leg, best_second_leg;
    string best_via_city;
    bool found = false;

    for (int via_index = 0; via_index < graph->vertex_count; via_index++)
    {
        if (via_index == from_index || via_index == to_index)
        {
            continue;
        }

        Node* first_leg = graph->vertices[from_index].adj_list;
        while (first_leg != NULL)
        {
            if (first_leg->dest_index == via_index)
            {
                Node* second_leg = graph->vertices[via_index].adj_list;
                while (second_leg != NULL)
                {
                    if (second_leg->dest_index == to_index)
                    {
                        int layover = calculate_layover_time(first_leg->data, second_leg->data);
                        if (layover >= 60)
                        {
                            double total_cost = first_leg->data.ticket_price + second_leg->data.ticket_price;

                            if (total_cost < best_total_cost)
                            {
                                best_total_cost = total_cost;
                                best_first_leg = first_leg->data;
                                best_second_leg = second_leg->data;
                                best_via_city = graph->vertices[via_index].city_name;
                                found = true;
                            }
                        }
                    }
                    second_leg = second_leg->next;
                }
            }
            first_leg = first_leg->next;
        }
    }

    if (found) 
    {
        int layover_time = calculate_layover_time(best_first_leg, best_second_leg);

        FlightInformation fi1(best_first_leg, graph->vertices[from_index].city_name, best_via_city);
        //flight2 also have layover time information but flight1 above has 0 layover time
        FlightInformation fi2(best_second_leg, best_via_city, graph->vertices[to_index].city_name, layover_time);

        journey_queue.enqueue(fi1);
        journey_queue.enqueue(fi2);
        process_journey();
    }
    else 
    {
        cout << "\nNo feasible connecting flights available" << endl;
    }
}