#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
using namespace std;


class Flight 
{
public:
    string airline_name;
    string flight_date;
    string departure_time;
    string arrival_time;
    double ticket_price;

    Flight(string airline = "", string date = "", string departure = "", string arrival = "", double price = 0.0);

    void display_flight() const;
};

//node class
class Node
{
public:
    int dest_index;
    Flight data;
    Node* next;
    Node* flight_next; //if we have multiple flight from same origin to same dest. then we have to make a linked list of flight
    Node();
};

//vertex class
class vertex
{
public:
    string city_name;
    double hotel_price;
    Node* adj_list;      //head of adj list
    vertex();
};

#endif

