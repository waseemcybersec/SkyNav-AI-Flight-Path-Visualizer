#include "customFlightSearch.h"

//customFlighrSearch class definntions
CustomFlightSearch::CustomFlightSearch(allFlights* g)
{
    graph = g;
}

// Helper function to check if flight matches airline preference
bool CustomFlightSearch::matches_airline_preference(Flight& flight, string& preferred_airline) const 
{
    if (flight.airline_name == preferred_airline)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//converting time to mints
int CustomFlightSearch::convert_time_to_minutes(string& time) const 
{
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    int total_time = hours * 60 + minutes;
    return total_time;
}

//for comparing dates, return true if the date 1 is before or equal to date2
bool CustomFlightSearch::is_valid_date_sequence(string& date1, string& date2) const 
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

//for checking feasible connection
bool CustomFlightSearch::is_feasible_connection(Flight& leg1, Flight& leg2) const 
{
    if (!is_valid_date_sequence(leg1.flight_date, leg2.flight_date))
    {
        return false;
    }
    if (leg1.flight_date != leg2.flight_date)
    {
        return true;
    }

    int arrival_time = convert_time_to_minutes(leg1.arrival_time);
    int departure_time = convert_time_to_minutes(leg2.departure_time);

    int calc_time = departure_time - arrival_time;
    if (calc_time >= 60)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Display connection details
void CustomFlightSearch::display_connection(string& from_city, string& transit_city, string& to_city, Flight& leg1, Flight& leg2) const
{
    cout << "\nConnection Found:\n";
    cout << "================\n";
    cout << "First Flight:\nFrom: " << from_city << " To: " << transit_city << endl;
    leg1.display_flight();

    cout << "\nSecond Flight:\nFrom: " << transit_city << " To: " << to_city << endl;
    leg2.display_flight();

    if (leg1.flight_date == leg2.flight_date) 
    {
        int arrival = convert_time_to_minutes(leg1.arrival_time);
        int departure = convert_time_to_minutes(leg2.departure_time);
        int layover = departure - arrival;

        cout << "\nLayover Time: " << layover << "Minutes" << endl;

        if (layover > 360) //if layover greater than 6 hourse
        {
            int transit_index = graph->find_vertex_index(transit_city);
            cout << "Long layover - Hotel available at " << transit_city;
            cout << " (Rs. " << graph->vertices[transit_index].hotel_price << " per day)\n";
        }
    }
    else 
    {
        cout << "\nOvernight/Multi day Connection\n";
        int transit_index = graph->find_vertex_index(transit_city);
        cout << "Hotel available at " << transit_city;
        cout << " (Rs. " << graph->vertices[transit_index].hotel_price << " per day)\n";
    }

    double total_cost = leg1.ticket_price + leg2.ticket_price;
    cout << "\nTotal Flight Cost: Rs : " << total_cost << endl;
    cout << "----------------------------------------\n";
}

//main serach ftn
void CustomFlightSearch::search_preferred_flights() 
{
    string origin, destination, preferred_airline, transit_city;
    char use_transit;

    cout << "\nEnter origin city : ";
    cin >> origin;
    cout << "Enter destination city : ";
    cin >> destination;
label0:
    cout << "Enter preferred airline name : ";
    cin >> preferred_airline;

    if (preferred_airline.empty()) 
    {
        cout << "Airline name is required! Please try again\n";
        goto label0;
    }

label1:
    cout << "\nDo you want a transit city (y/n): ";
    cin >> use_transit;
    if (use_transit == 'y' || use_transit == 'Y')
    {
        cout << "Enter transit city : ";
        cin >> transit_city;

        if (transit_city == origin || transit_city == destination) 
        {
            cout << "Transit city cannot be same as origin or destination\n";
            return;
        }

        bool found = find_connecting_flights(origin, destination, preferred_airline, transit_city);
        if (!found) 
        {
            cout << "\nNo feasible connections found\n";
            return;
        }
        graph->runCustomFlightSearch(origin, destination, preferred_airline, transit_city, true);
    }
    else if(use_transit == 'n' || use_transit == 'N')
    {
        bool found = find_direct_flights(origin, destination, preferred_airline);
        if (!found) 
        {
            cout << "\nNo direct flights available.\n";
            return;
        }
        graph->runCustomFlightSearch(origin, destination, preferred_airline, transit_city, false);
    }
    else
    {
        cout << "Invalid choice entered! Please Enter only y/n\n";
        goto label1;
    }
}

// Search for direct flights
bool CustomFlightSearch::find_direct_flights(string& from_city, string& to_city, string& preferred_airline) const 
{
    int from_index = graph->find_vertex_index(from_city);
    int to_index = graph->find_vertex_index(to_city);
    bool found = false;

    if (from_index == -1 || to_index == -1) 
    {
        cout << "One or both cities not found in the system\n";
        return false;
    }

    Node* current = graph->vertices[from_index].adj_list;
    while (current != NULL)
    {
        if (current->dest_index == to_index)
        {
            Node* flight = current;
            while (flight != NULL)
            {
                if (matches_airline_preference(flight->data, preferred_airline))
                {
                    if (!found) 
                    {
                        cout << "\nDirect Flights Available:\n";
                        cout << "========================\n";
                    }
                    found = true;
                    cout << "\nFlight Details:\n";
                    flight->data.display_flight();
                    cout << "----------------------------------------\n";
                }
                flight = flight->flight_next;
            }
        }
        current = current->next;
    }
    return found;
}

//search for connecting flights
bool CustomFlightSearch::find_connecting_flights(string& from_city, string& to_city, string& preferred_airline, string& transit_city)
{
    int from_index = graph->find_vertex_index(from_city);
    int to_index = graph->find_vertex_index(to_city);
    int transit_index = graph->find_vertex_index(transit_city); //layover city
    bool found = false;

    if (from_index == -1 || to_index == -1 || transit_index == -1)
    {
        cout << "One or more cities not found in the system\n";
        return false;
    }

    //iterating to all citites from origin to transit city
    Node* curr_first_leg = graph->vertices[from_index].adj_list;
    while (curr_first_leg != NULL)
    {
        if (curr_first_leg->dest_index == transit_index)
        {
            //now checking all flights that goes to transit city from origin
            Node* flight1 = curr_first_leg;
            while (flight1 != NULL)
            {
                if (matches_airline_preference(flight1->data, preferred_airline)) //checking airline prefernce
                {
                    //now checking for possible leg2 flight from transit city to the destinaton
                    Node* curr_second_leg = graph->vertices[transit_index].adj_list;
                    while (curr_second_leg != NULL)
                    {
                        if (curr_second_leg->dest_index == to_index)
                        {
                            //now checking for all possible flights
                            Node* flight2 = curr_second_leg;
                            while (flight2 != NULL)
                            {
                                if (matches_airline_preference(flight2->data, preferred_airline))
                                {
                                    //checking for fligt fleasibility
                                    if (is_feasible_connection(flight1->data, flight2->data)) 
                                    {
                                        found = true;
                                        display_connection(from_city, transit_city, to_city, flight1->data, flight2->data);
                                    }
                                }
                                flight2 = flight2->flight_next;
                            }
                        }
                        curr_second_leg = curr_second_leg->next;
                    }
                }
                flight1 = flight1->flight_next;
            }
        }
        curr_first_leg = curr_first_leg->next;
    }
    return found;
}