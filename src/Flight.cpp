#include "Flight.h"

//flight class definitions
Flight::Flight(string airline, string date, string departure, string arrival, double price)
{
    airline_name = airline;
    flight_date = date;
    departure_time = departure;
    arrival_time = arrival;
    ticket_price = price;
}

void Flight::display_flight() const 
{
    cout << "  Airline Name   : " << airline_name << endl;
    cout << "  Flight Date    : " << flight_date << endl;
    cout << "  Departure Time : " << departure_time << endl;
    cout << "  Arrival Time   : " << arrival_time << endl;
    cout << "  Ticket Price   : Rs " << ticket_price << endl;

}
//------------------------------------------------------------------

//node class definitions
Node::Node()
{
    dest_index = -1;
    next = NULL;
    flight_next = NULL;
}
//-------------------------------------------------------------------

//vertex class defintions

vertex::vertex()
{
    hotel_price = 0.0;
    adj_list = NULL;
}
//---------------------------------------------------------------------