#include"advanceRoute.h"

RouteInformation::RouteInformation(Flight f, string from, string to, int lay, bool valid)
{
    flight_info = f;
    from_city = from;
    to_city = to;
    layover_minutes = lay;
    is_valid = valid;
}

RouteNode::RouteNode(RouteInformation ri)
{
    data = ri;
    next = NULL;
}

TransitCity::TransitCity(string name)
{
    city_name = name;
    next = NULL;
}

AdvancedRouteGenerator::AdvancedRouteGenerator(allFlights* g)
{
    graph = g;
    route_head = NULL;
    transit_count = 0;
}
bool AdvancedRouteGenerator::is_valid_date_sequence(string& date1, string& date2) const
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

int AdvancedRouteGenerator::calculate_layover_time(Flight& first_leg, Flight& second_leg)
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

        int layover = (24 * 60 - arrival) + ((days_diff - 1) * 24 * 60) + departure;
        return layover;
    }
}

int AdvancedRouteGenerator::convert_time_to_minutes(string& time) const
{
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    int total_time = hours * 60 + minutes;
    return total_time;
}

Flight AdvancedRouteGenerator::find_best_flight(int from_index, int to_index)
{
    Node* current = graph->vertices[from_index].adj_list;
    Flight best_flight;
    double min_cost = INT_MAX; //assigning arbitrary gretest value
    bool found = false;

    while (current != NULL)
    {
        if (current->dest_index == to_index)
        {
            Node* flight = current;
            while (flight != NULL)
            {
                if (flight->data.ticket_price < min_cost)
                {
                    min_cost = flight->data.ticket_price;
                    best_flight = flight->data;
                    found = true;
                }
                flight = flight->flight_next;
            }
        }
        current = current->next;
    }
    if (found)
    {
        return best_flight;
    }
    else
    {
        return Flight();
    }
}

void AdvancedRouteGenerator::add_transit_city(string city)
{
    TransitCity* newCity = new TransitCity(city);
    if (transit_head == NULL)
    {
        transit_head = newCity;
    }
    else 
    {
        TransitCity* temp = transit_head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newCity;
    }
    transit_count++;
}

void AdvancedRouteGenerator::clear_transit_cities()
{
    while (transit_head != NULL)
    {
        TransitCity* temp = transit_head;
        transit_head = transit_head->next;
        delete temp;
    }
    transit_count = 0;
}

void AdvancedRouteGenerator::clear_route()
{
    while (route_head != NULL)
    {
        RouteNode* temp = route_head;
        route_head = route_head->next;
        delete temp;
    }
}

void AdvancedRouteGenerator::add_route_segment(RouteInformation ri)
{
    RouteNode* newNode = new RouteNode(ri);
    if (route_head == NULL)
    {
        route_head = newNode;
    }
    else 
    {
        RouteNode* temp = route_head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void AdvancedRouteGenerator::display_route()
{
    cout << "\nComplete Route Information:" << endl;
    cout << "==========================" << endl;

    RouteNode* current = route_head;
    double total_cost = 0;
    int total_layover = 0;
    bool has_invalid_segment = false;

    while (current != NULL)
    {
        cout << "\nFlight Segment: " << current->data.from_city << " -> " << current->data.to_city << endl;
        cout << "---------------------------" << endl;

        if (current->data.is_valid)
        {
            current->data.flight_info.display_flight();
            total_cost += current->data.flight_info.ticket_price;

            if (current->next != NULL && current->data.layover_minutes > 0)
            {
                total_layover += current->data.layover_minutes;
                cout << "\nLayover Information:" << endl;
                cout << "Location: " << current->data.to_city << endl;
                cout << "Duration: " << current->data.layover_minutes / 60 << " hours ";
                cout << current->data.layover_minutes % 60 << " minutes" << endl;

                if (current->data.layover_minutes > 360)
                {
                    int city_index = graph->find_vertex_index(current->data.to_city);
                    cout << "Extended Layover - Hotel Available at "<<graph->vertices[city_index].hotel_price << endl;
                    cout << "Hotel Rate : Rs. " << graph->vertices[city_index].hotel_price << " per day" << endl;
                }
            }
        }
        else
        {
            cout << "No feasible flight available for this segment!" << endl;
            has_invalid_segment = true;
        }
        current = current->next;
    }

    cout << "\nOverall Route Summary:" << endl;
    cout << "-------------------" << endl;
    if (has_invalid_segment)
    {
        cout << "Warning!! Complete route not possible! Some segments unavailable" << endl;
    }
    cout << "Total Flight Cost: Rs. " << total_cost << endl;
    if (total_layover > 0)
    {
        cout << "Total Layover Time: " << total_layover / 60 << " hours " << total_layover % 60 << " minutes" << endl;
    }
    cout << endl << endl;
}

bool AdvancedRouteGenerator::verify_cities(string& origin, string& destination)
{
    TransitCity* current = transit_head;
    string prev_city = origin;

    while (current != NULL)
    {
        if (current->city_name == prev_city || current->city_name == destination)
        {
            cout << "Error: Transit city cannot be same as origin/destination" << endl;
            return false;
        }
        TransitCity* check = current->next;
        while (check != NULL)
        {
            //if duplicate transit city
            if (check->city_name == current->city_name)
            {
                cout << "Error! Duplicate transit city found" << endl;
                return false;
            }
            check = check->next;
        }
        prev_city = current->city_name;
        current = current->next;
    }
    return true;
}

void AdvancedRouteGenerator::generate_route()
{
    string origin, destination;
    int num_transit;

    cout << "\nAdvanced Route Generation" << endl;
    cout << "-------------------------" << endl;
    cout << "Enter origin city : ";
    cin >> origin;
    cout << "Enter destination city : ";
    cin >> destination;

    int origin_index = graph->find_vertex_index(origin);
    int dest_index = graph->find_vertex_index(destination);

    if (origin_index == -1 || dest_index == -1)
    {
        cout << "Invalid origin or destination city" << endl;
        return;
    }
    cout << "Enter number of transit cities : ";
    cin >> num_transit;

    if (num_transit < 0)
    {
        cout << "Invalid number of transit cities! Please try again" << endl;
        return;
    }
    //clearing previous data if searched
    clear_transit_cities();
    clear_route();

    for (int i = 0; i < num_transit; i++)
    {
        string city;
        cout << "Enter transit city " << (i + 1) << " : ";
        cin >> city;

        if (graph->find_vertex_index(city) == -1)
        {
            cout << "Error: Invalid transit city -> " << city << endl;
            clear_transit_cities(); //clear and return
            return;
        }
        //if valid then add to the linked list
        add_transit_city(city);
    }

    if (!verify_cities(origin, destination)) //this ftn check if the transit city same as origin and dest.
    {
        clear_transit_cities();
        return;
    }

    string from_city = origin;
    TransitCity* current = transit_head;

    while (true)
    {
        string to_city;
        if (current != NULL)
        {
            to_city = current->city_name;
            current = current->next;
        }
        else 
        {
            to_city = destination;
        }

        int from_index = graph->find_vertex_index(from_city);
        int to_index = graph->find_vertex_index(to_city);

        Flight best_flight = find_best_flight(from_index, to_index);
        
        bool is_valid = best_flight.ticket_price > 0;

        RouteInformation ri(best_flight, from_city, to_city, 0, is_valid);

        if (is_valid && to_city != destination)
        {
            string next_city;
            if (current != NULL)
            {
                next_city = current->city_name;
            }
            else
            {
                next_city = destination;
            }
            
            int next_from = graph->find_vertex_index(to_city);
            int next_to = graph->find_vertex_index(next_city);

            Flight next_flight = find_best_flight(next_from, next_to);

            if (next_flight.ticket_price > 0)
            {
                int layover = calculate_layover_time(best_flight, next_flight);
                if (layover >= 60)
                {
                    ri.layover_minutes = layover;
                }
                else
                {
                    ri.is_valid = false; //if not feasible flight
                }
            }
        }
        //if pass all checks then add the route segment
        add_route_segment(ri);

        if (to_city == destination)
        {
            break;
        }
        from_city = to_city;
    }

    display_route();
}

AdvancedRouteGenerator::~AdvancedRouteGenerator()
{
    clear_route();
    clear_transit_cities();
}
