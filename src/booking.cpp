#include "booking.h"


BookFlight::BookFlight(allFlights* g)
{
    graph = g;
}

//this ftn extract time string and convert into minutes
int BookFlight::time_to_minutes(string& time) const 
{
    int t = stoi(time.substr(0, 2)) * 60 + stoi(time.substr(3, 2));
    return t;
}

int BookFlight::time_difference(string& time1, string& time2) const 
{
    int t1 = time_to_minutes(time1);
    int t2 = time_to_minutes(time2);
    return t2 - t1;
}

void BookFlight::display_direct_flights(int from_index, int to_index) const 
{
    Node* current = graph->vertices[from_index].adj_list;
    bool found = false;
    int flight_num = 1;

    cout << "\nDirect Flights from " << graph->vertices[from_index].city_name << " to " 
         << graph->vertices[to_index].city_name << ":\n";
    cout << "----------------------------------\n";

    while (current != NULL) 
    {
        if (current->dest_index == to_index) 
        {
            found = true;
            Node* flight = current;
            while (flight != NULL) 
            {
                cout << "\nFlight " << flight_num++ << ":\n";
                cout << "----------\n";
                flight->data.display_flight();
                flight = flight->flight_next;
            }
            break;
        }
        current = current->next;
    }

    if (!found) 
    {
        cout << "\nNo direct flights available between these cities\n";
    }
}

void BookFlight::display_connecting_flights(int from_index, int to_index) const 
{
    cout << "\nSearching for connecting flights...\n";
    bool found = false;

    for (int intermediate_index = 0; intermediate_index < graph->vertex_count; intermediate_index++) 
    {
        //skip if intermediate is same origin or dest
        if (intermediate_index == from_index || intermediate_index == to_index)
        {
            continue;
        }

        //checking all flights from origin to interm. city
        Node* first_leg = graph->vertices[from_index].adj_list;
        while (first_leg != NULL) 
        {
            if (first_leg->dest_index == intermediate_index) 
            {
                //checking all flights from interm. to dest.
                Node* second_leg = graph->vertices[intermediate_index].adj_list;
                while (second_leg != NULL) 
                {
                    if (second_leg->dest_index == to_index) 
                    {
                        //checkig all possible flights
                        Node* flight1 = first_leg;
                        while (flight1 != NULL) 
                        {
                            Node* flight2 = second_leg;
                            while (flight2 != NULL) 
                            {
                                //extracting dates
                                int leg1_date = stoi(flight1->data.flight_date.substr(0, 2));

                                int leg2_date = stoi(flight2->data.flight_date.substr(0, 2));

                                bool valid_connection = false;
                                int layover_time = 0;

                                //if same day connection
                                if (leg1_date == leg2_date) 
                                {
                                    int leg1_arrival = time_to_minutes(flight1->data.arrival_time);
                                    int leg2_departure = time_to_minutes(flight2->data.departure_time);

                                    //checking if leg2 departs after leg1
                                    if (leg2_departure > leg1_arrival) 
                                    {
                                        layover_time = leg2_departure - leg1_arrival;
                                        valid_connection = (layover_time >= 60);
                                    }
                                }
                                //if theree is next day or subsequent day connection
                                else if (leg2_date > leg1_date) 
                                {
                                    int leg1_arrival = time_to_minutes(flight1->data.arrival_time);
                                    int leg2_departure = time_to_minutes(flight2->data.departure_time);

                                    //calculating days differenece
                                    int days_diff = leg2_date - leg1_date;
                                   
                                    //calculating leg1 remainning mints and leg 2 minutes
                                    layover_time = (24 * 60 - leg1_arrival) + ((days_diff - 1) * 24 * 60) + leg2_departure;              // Minutes until leg2's departure

                                    valid_connection = true; //always true for subsequent dates
                                }
                                if (valid_connection && layover_time >= 60) 
                                {
                                    found = true;
                                    cout << "\nConnecting Flights:\n";
                                    cout << "-------------------\n";

                                    cout << "Leg 1: " << graph->vertices[from_index].city_name;
                                    cout << " to " << graph->vertices[intermediate_index].city_name << "\n";
                                    flight1->data.display_flight();

                                    cout << "\nLeg 2: " << graph->vertices[intermediate_index].city_name;
                                    cout << " to " << graph->vertices[to_index].city_name << "\n";
                                    flight2->data.display_flight();

                                    double total_price = flight1->data.ticket_price + flight2->data.ticket_price;
                                    cout << "\nTotal flight cost: Rs " << total_price;

                                    //converting layover time to hours and minutes
                                    int layover_hours = layover_time / 60;
                                    int layover_minutes = layover_time % 60;
                                    cout << "\nLayover time : " << layover_hours << " hours and "<< layover_minutes << "minutes";

                                    //if layover time is greater than 6 hrs then show the hotel price also
                                    if (layover_time > 360) 
                                    {
                                        cout << "\nLayover exceeds 6 hours. Hotel price at ";
                                        cout << graph->vertices[intermediate_index].city_name;
                                        cout << ": Rs. " << graph->vertices[intermediate_index].hotel_price << " per day\n";
                                    }

                                    cout << "\n----------------------------------\n";
                                }
                                flight2 = flight2->flight_next;
                            }
                            flight1 = flight1->flight_next;
                        }
                    }
                    second_leg = second_leg->next;
                }
            }
            first_leg = first_leg->next;
        }
    }

    if (!found) 
    {
        cout << "\nNo feasible connecting flights available between these cities\n";
    }
}

void BookFlight::book_flight() 
{
    string origin, destination;

    cout << "\nEnter the origin city : ";
    cin >> origin;
    cout << "Enter the destination city : ";
    cin >> destination;

    int from_index = graph->find_vertex_index(origin);
    int to_index = graph->find_vertex_index(destination);

    if (from_index == -1 || to_index == -1)
    {
        cout << "\nOne or both cities are not found!\n";
        return;
    }
    graph->runBookingView(from_index,to_index);

    //displaying both direct and conneccting fights
    display_direct_flights(from_index, to_index);
    display_connecting_flights(from_index, to_index);
}