#include "Graph.h"

class allFlights : public Graph
{
public:
    sf::RenderWindow window;
    sf::Font font;

    class VertexVisual
    {
    public:
        sf::Vector2f position;
        sf::CircleShape shape;
        sf::Text label;
    };

    VertexVisual* vertexVisuals;
    bool is_booking_view;
    int booking_from_index;
    int booking_to_index;

    void drawFlightRoute(int fromIndex, int toIndex, const Flight& flight, bool isDimmed = false);

    void drawHighlightedRoute(int fromIndex, int toIndex, const Flight& flight, bool isConnecting);

    void drawDottedLine(sf::Vector2f start, sf::Vector2f end);

    allFlights() : is_booking_view(false), booking_from_index(-1), booking_to_index(-1) {}
    ~allFlights();

    void initGraphics();
    void runAllFlights();   // For viewing all flights
    void runBookingView(int from_index, int to_index); // For viewing all the book flights between origin and destination
    void runShortestPath(int dest, int* parent, Flight** flights, double total_value, bool by_price); // For viewing all the shortest path flight between origin and destination
    void runLayover(string origin, string destination); //displaying layover cities
    void runSubGraph(string airline);
    void runCustomFlightSearch(string origin, string destination, string preferred_airline, string transit_city, bool transitCityFound);
};