#include "SFML.h"

void allFlights::initGraphics()
{
    window.create(sf::VideoMode(1920, 1080), "Flight Visualization");

    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "Font loading failed!" << endl;
        return;
    }

    vertexVisuals = new VertexVisual[vertex_count];

    const float spacing = 100;
    const float width = 1920 - 2 * spacing;
    const float height = 1080 - 2 * spacing;
    const float min_distance = 150; // min distance btw vertices

    const sf::Color vertices_colour[] = { sf::Color(65, 105, 225), sf::Color(46, 139, 87), sf::Color(188, 39, 50), sf::Color(147, 112, 219), sf::Color(255, 140, 0) };

    int gridCols = ceil(sqrt(vertex_count * 1.5));
    int gridRows = ceil(vertex_count / float(gridCols));
    float cellwidth = width / gridCols;
    float cellheight = height / gridRows;

    bool* occupiedCells = new bool[gridRows * gridCols]();

    for (int i = 0; i < vertex_count; i++)
    {
        unsigned int hash = 0;
        for (char c : vertices[i].city_name)
            hash = hash * 31 + c;

        bool positionFound = false;
        int attempts = 0;
        float finalX = 0, finalY = 0;

        while (!positionFound && attempts < gridRows * gridCols)
        {
            int cellIndex = (hash + attempts) % (gridRows * gridCols);
            int row = cellIndex / gridCols;
            int col = cellIndex % gridCols;

            if (!occupiedCells[row * gridCols + col])
            {
                float baseX = spacing + col * cellwidth + cellwidth * 0.5f;
                float baseY = spacing + row * cellheight + cellheight * 0.5f;

                float offsetX = (hash % 100) / 100.0f * (cellwidth * 0.3f);
                float offsetY = ((hash >> 8) % 100) / 100.0f * (cellheight * 0.3f);

                finalX = baseX + offsetX - (cellwidth * 0.15f);
                finalY = baseY + offsetY - (cellheight * 0.15f);

                bool tooClose = false;
                for (int j = 0; j < i; j++)
                {
                    float dx = finalX - vertexVisuals[j].position.x;
                    float dy = finalY - vertexVisuals[j].position.y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < min_distance)
                    {
                        tooClose = true;
                        break;
                    }
                }

                if (!tooClose)
                {
                    occupiedCells[row * gridCols + col] = true;
                    positionFound = true;
                }
            }
            attempts++;
        }

        vertexVisuals[i].position = sf::Vector2f(finalX, finalY);
        int colorIndex = hash % 5;

        vertexVisuals[i].shape.setRadius(8);
        vertexVisuals[i].shape.setPosition(vertexVisuals[i].position.x - 8, vertexVisuals[i].position.y - 8);
        vertexVisuals[i].shape.setFillColor(vertices_colour[colorIndex]);
        vertexVisuals[i].shape.setOutlineThickness(2);
        vertexVisuals[i].shape.setOutlineColor(sf::Color::White);

        vertexVisuals[i].label.setFont(font);
        vertexVisuals[i].label.setString(vertices[i].city_name);
        vertexVisuals[i].label.setCharacterSize(14);
        vertexVisuals[i].label.setStyle(sf::Text::Bold);
        vertexVisuals[i].label.setFillColor(sf::Color::White);
        vertexVisuals[i].label.setOutlineThickness(1);
        vertexVisuals[i].label.setOutlineColor(sf::Color::Black);
        vertexVisuals[i].label.setPosition(vertexVisuals[i].position.x - vertexVisuals[i].label.getGlobalBounds().width / 2, vertexVisuals[i].position.y - 25);
    }

    delete[] occupiedCells;
}

void allFlights::drawFlightRoute(int fromIndex, int toIndex, const Flight& flight, bool isDimmed)
{
    sf::Vector2f startPos = vertexVisuals[fromIndex].position;
    sf::Vector2f endPos = vertexVisuals[toIndex].position;
    sf::Vector2f direction = endPos - startPos;

    sf::VertexArray curve(sf::LineStrip, 30);
    sf::Vector2f control = sf::Vector2f((startPos.x + endPos.x) * 0.5f - direction.y * 0.2f, (startPos.y + endPos.y) * 0.5f + direction.x * 0.2f);

    for (size_t i = 0; i < 30; i++)
    {
        float t = i / 29.0f;
        float oneMinusT = 1.0f - t;

        sf::Vector2f point = oneMinusT * oneMinusT * startPos + 2.0f * oneMinusT * t * control + t * t * endPos;
        curve[i].position = point;
        curve[i].color = isDimmed ? sf::Color(150, 150, 150, 80) : sf::Color(150, 150, 150, 180);
    }

    window.draw(curve);

    if (!isDimmed)
    {
        // arrowhead
        sf::Vector2f lastPoint = curve[28].position;
        sf::Vector2f secondLastPoint = curve[27].position;
        sf::Vector2f arrowDirection = lastPoint - secondLastPoint;
        float arrowLength = sqrt(arrowDirection.x * arrowDirection.x + arrowDirection.y * arrowDirection.y);
        arrowDirection /= arrowLength;
        float arrowSize = 15.0f;
        sf::Vector2f perp(-arrowDirection.y, arrowDirection.x);

        sf::ConvexShape arrowhead;
        arrowhead.setPointCount(3);
        arrowhead.setPoint(0, lastPoint);
        arrowhead.setPoint(1, lastPoint - arrowDirection * arrowSize + perp * (arrowSize * 0.5f));
        arrowhead.setPoint(2, lastPoint - arrowDirection * arrowSize - perp * (arrowSize * 0.5f));
        arrowhead.setFillColor(sf::Color(200, 50, 50));
        window.draw(arrowhead);

        // flight info
        sf::Text info;
        info.setFont(font);
        string info_text = flight.airline_name + "\nRs" + to_string(int(flight.ticket_price));
        info.setString(info_text);
        info.setCharacterSize(12);
        info.setFillColor(sf::Color::Yellow);
        info.setOutlineThickness(1);
        info.setOutlineColor(sf::Color::Black);

        sf::Vector2f midpoint = curve[15].position;
        info.setPosition(midpoint.x - info.getGlobalBounds().width / 2, midpoint.y - 20);
        window.draw(info);
    }
}

void allFlights::drawHighlightedRoute(int fromIndex, int toIndex, const Flight& flight, bool isConnecting)
{
    sf::Vector2f startPos = vertexVisuals[fromIndex].position;
    sf::Vector2f endPos = vertexVisuals[toIndex].position;
    sf::Vector2f direction = endPos - startPos;
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    // thicker line with multiple passes
    for (int offset = -2; offset <= 2; offset++)
    {
        sf::VertexArray curve(sf::LineStrip, 30);
        sf::Vector2f control = sf::Vector2f((startPos.x + endPos.x) * 0.5f - direction.y * 0.2f, (startPos.y + endPos.y) * 0.5f + direction.x * 0.2f);

        for (int i = 0; i < 30; i++)
        {
            float t = i / 29.0f;
            float oneMinusT = 1.0f - t;

            sf::Vector2f point = oneMinusT * oneMinusT * startPos + 2.0f * oneMinusT * t * control + t * t * endPos;
            sf::Vector2f normal(-direction.y / length, direction.x / length);
            point += normal * (float)offset * 3.0f;

            curve[i].position = point;
            curve[i].color = isConnecting ? sf::Color(255, 165, 0, 220) : sf::Color(0, 255, 0, 220);
        }
        window.draw(curve);
    }

    // Enhanced flight info
    sf::Text info;
    info.setFont(font);
    string info_text = flight.airline_name + "\nRs" + to_string(int(flight.ticket_price));
    info.setString(info_text);
    info.setCharacterSize(16);
    info.setFillColor(sf::Color::White);
    info.setOutlineThickness(2);
    info.setOutlineColor(sf::Color::Black);

    sf::Vector2f midpoint = (startPos + endPos) * 0.5f;
    info.setPosition(midpoint.x - info.getGlobalBounds().width / 2, midpoint.y - 25);
    window.draw(info);
}

void allFlights::runAllFlights()
{
    initGraphics();

    while (window.isOpen())
    {
        sf::Event event;

        // Poll events
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); // Close window when close event is triggered

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                window.close(); // Close window when Enter key is pressed
        }

        // Clear the window with a background color
        window.clear(sf::Color(20, 20, 30));  // Dark background

        // Draw regular flight routes
        for (int i = 0; i < vertex_count; i++)
        {
            Node* current = vertices[i].adj_list;

            while (current != NULL)
            {
                drawFlightRoute(i, current->dest_index, current->data);
                current = current->next;
            }
        }

        // Draw cities
        for (int i = 0; i < vertex_count; i++)
        {
            window.draw(vertexVisuals[i].shape); // Draw city circles
            window.draw(vertexVisuals[i].label); // Draw city labels
        }

        // Display everything that was drawn on the window
        window.display();
    }
}


void allFlights::runBookingView(int from_index, int to_index)
{
    initGraphics();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(20, 20, 30));

        // Draw all routes dimmed
        for (int i = 0; i < vertex_count; i++)
        {
            Node* current = vertices[i].adj_list;
            while (current != NULL)
            {
                drawFlightRoute(i, current->dest_index, current->data, true);
                current = current->next;
            }
        }

        // Draw direct flights
        Node* direct = vertices[from_index].adj_list;
        while (direct != NULL)
        {
            if (direct->dest_index == to_index)
                drawHighlightedRoute(from_index, to_index, direct->data, false);

            direct = direct->next;
        }

        // Draw connecting flights
        for (int intermediate = 0; intermediate < vertex_count; intermediate++)
        {
            if (intermediate != from_index && intermediate != to_index)
            {
                Node* first_leg = vertices[from_index].adj_list;
                while (first_leg != NULL)
                {
                    if (first_leg->dest_index == intermediate)
                    {
                        Node* second_leg = vertices[intermediate].adj_list;

                        while (second_leg != NULL)
                        {
                            if (second_leg->dest_index == to_index)
                            {
                                drawHighlightedRoute(from_index, intermediate, first_leg->data, true);
                                drawHighlightedRoute(intermediate, to_index, second_leg->data, true);
                            }
                            second_leg = second_leg->next;
                        }
                    }
                    first_leg = first_leg->next;
                }
            }
        }

        // Draw cities with highlighted origin/destination
        for (int i = 0; i < vertex_count; i++)
        {
            if (i == from_index || i == to_index)
            {
                // Save original values
                float originalRadius = vertexVisuals[i].shape.getRadius();
                sf::Color originalColor = vertexVisuals[i].shape.getFillColor();

                // Set highlighted values
                vertexVisuals[i].shape.setRadius(12);
                vertexVisuals[i].shape.setFillColor(sf::Color::Yellow);

                // Draw highlighted city
                window.draw(vertexVisuals[i].shape);
                window.draw(vertexVisuals[i].label);

                // Restore original values
                vertexVisuals[i].shape.setRadius(originalRadius);
                vertexVisuals[i].shape.setFillColor(originalColor);
            }

            else
            {
                window.draw(vertexVisuals[i].shape);
                window.draw(vertexVisuals[i].label);
            }
        }
        window.display();
    }
}

void allFlights::runShortestPath(int dest, int* parent, Flight** flights, double total_value, bool by_price)
{
    initGraphics();

    // Calculate the length of the path
    int path_length = 0;
    for (int current = dest; current != -1; current = parent[current])
        path_length++;

    // Allocate an array to store the path
    int* path = new int[path_length];
    int index = path_length - 1;

    // Build the shortest path by backtracking from the destination
    for (int current = dest; current != -1; current = parent[current])
        path[index--] = current;

    // Main loop to visualize the shortest path
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter))
                window.close();
        }

        window.clear(sf::Color(20, 20, 30));

        // Draw all routes dimmed
        for (int i = 0; i < vertex_count; i++)
        {
            Node* current = vertices[i].adj_list;

            while (current != NULL)
            {
                drawFlightRoute(i, current->dest_index, current->data, true);
                current = current->next;
            }
        }

        // Highlight the shortest path
        for (int i = 0; i < path_length - 1; i++)
        {
            int from = path[i];
            int to = path[i + 1];
            drawHighlightedRoute(from, to, *flights[to], false);  // Assuming flights[to] contains the data
        }

        // Draw cities with special styling for the path
        for (int i = 0; i < vertex_count; i++)
        {
            bool in_path = false;
            for (int j = 0; j < path_length; ++j)
            {
                if (path[j] == i)
                {
                    in_path = true;
                    break;
                }
            }

            if (in_path)
            {
                // Highlight cities in the path
                float originalRadius = vertexVisuals[i].shape.getRadius();
                sf::Color originalColor = vertexVisuals[i].shape.getFillColor();

                vertexVisuals[i].shape.setRadius(12);
                vertexVisuals[i].shape.setFillColor(sf::Color::Yellow);

                window.draw(vertexVisuals[i].shape);
                window.draw(vertexVisuals[i].label);

                vertexVisuals[i].shape.setRadius(originalRadius);
                vertexVisuals[i].shape.setFillColor(originalColor);
            }

            else
            {
                window.draw(vertexVisuals[i].shape);
                window.draw(vertexVisuals[i].label);
            }
        }

        // Display total value at the top
        sf::Text info;
        info.setFont(font);
        string infoText = "Total " + string(by_price ? "Price: Rs " : "Time: ") + to_string(static_cast<int>(total_value));
        info.setString(infoText);
        info.setCharacterSize(20);
        info.setFillColor(sf::Color::White);
        info.setPosition(50, 20);
        window.draw(info);

        window.display();
    }

    // Clean up dynamically allocated memory
    delete[] path;
}

void allFlights::runLayover(string origin, string destination)
{
    // Initialize graphics for displaying all cities and connections
    initGraphics();

    // Find the indices of the origin and destination cities in the graph
    int originIndex = -1, destinationIndex = -1;
    for (int i = 0; i < vertex_count; i++)
    {
        if (vertices[i].city_name == origin)
            originIndex = i;

        if (vertices[i].city_name == destination)
            destinationIndex = i;
    }

    // If either the origin or destination is not found, exit early
    if (originIndex == -1 || destinationIndex == -1)
        return; // Exit the function if the cities are invalid

    // Main loop to display the layover flights and their connections
    while (window.isOpen())
    {
        sf::Event event;

        // Poll events (handle window close or keypress events)
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); // Close window when close event is triggered

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                window.close(); // Close window when Enter key is pressed
        }

        // Clear the window with a background color
        window.clear(sf::Color(20, 20, 30));  // Dark background

        // Draw all cities manually
        for (int i = 0; i < vertex_count; i++)
        {
            // Draw the cities as circles (city shape)
            sf::CircleShape cityShape(10);  // Circle with radius of 10
            cityShape.setPosition(vertexVisuals[i].position);  // Use pre-calculated position
            cityShape.setFillColor(sf::Color::White);  // Default city color
            window.draw(cityShape);  // Draw the city

            // Draw the city labels (names)
            sf::Text cityLabel(vertices[i].city_name, font, 15);  // Create text for the city
            cityLabel.setPosition(vertexVisuals[i].position.x - 10, vertexVisuals[i].position.y - 15);  // Adjust label position
            cityLabel.setFillColor(sf::Color::White);  // Set label color
            window.draw(cityLabel);  // Draw the city name
        }

        // Highlight the origin and destination cities with different colors
        sf::CircleShape originCityShape(10);  // Origin city circle
        originCityShape.setPosition(vertexVisuals[originIndex].position);
        originCityShape.setFillColor(sf::Color::Green);  // Green for origin city
        window.draw(originCityShape);

        sf::Text originLabel(vertices[originIndex].city_name, font, 15);
        originLabel.setPosition(vertexVisuals[originIndex].position.x - 10, vertexVisuals[originIndex].position.y - 15);
        originLabel.setFillColor(sf::Color::Green);  // Green label
        window.draw(originLabel);

        sf::CircleShape destinationCityShape(10);  // Destination city circle
        destinationCityShape.setPosition(vertexVisuals[destinationIndex].position);
        destinationCityShape.setFillColor(sf::Color::Red);  // Red for destination city
        window.draw(destinationCityShape);

        sf::Text destinationLabel(vertices[destinationIndex].city_name, font, 15);
        destinationLabel.setPosition(vertexVisuals[destinationIndex].position.x - 10, vertexVisuals[destinationIndex].position.y - 15);
        destinationLabel.setFillColor(sf::Color::Red);  // Red label
        window.draw(destinationLabel);

        // Draw layover connections only for cities connected to destination
        for (int i = 0; i < vertex_count; i++)
        {
            // Skip if the city is neither origin nor destination
            if (i == originIndex || i == destinationIndex)
                continue;

            // Check for valid layover flights: origin -> intermediate -> destination
            Node* first_leg = vertices[originIndex].adj_list;  // Get adjacency list of the origin city

            while (first_leg != NULL)
            {
                // If the first leg ends at the intermediate city (layover city)
                if (first_leg->dest_index == i)
                {
                    // Check if this intermediate city connects directly to the destination
                    Node* second_leg = vertices[i].adj_list;  // Get adjacency list for the layover city

                    while (second_leg != NULL)
                    {
                        // If the second leg ends at the destination city
                        if (second_leg->dest_index == destinationIndex)
                        {
                            // Get the positions for the cities (you can extract these from your vertexVisuals)
                            sf::Vector2f originPosition = vertexVisuals[originIndex].position;
                            sf::Vector2f layoverPosition = vertexVisuals[i].position;
                            sf::Vector2f destinationPosition = vertexVisuals[destinationIndex].position;

                            // Draw the dotted flight route from origin to the layover city
                            drawDottedLine(originPosition, layoverPosition);

                            // Draw the dotted flight route from the layover city to the destination city
                            drawDottedLine(layoverPosition, destinationPosition);

                            // Highlight this layover city with yellow
                            sf::CircleShape layoverCityShape(10);  // Layover city circle
                            layoverCityShape.setPosition(vertexVisuals[i].position);
                            layoverCityShape.setFillColor(sf::Color::Yellow);  // Yellow for layover city
                            window.draw(layoverCityShape);

                            sf::Text layoverLabel(vertices[i].city_name, font, 15);
                            layoverLabel.setPosition(vertexVisuals[i].position.x - 10, vertexVisuals[i].position.y - 15);
                            layoverLabel.setFillColor(sf::Color::Yellow);  // Yellow label
                            window.draw(layoverLabel);
                        }
                        second_leg = second_leg->next;
                    }
                }
                first_leg = first_leg->next;
            }
        }

        // Display everything that was drawn on the window
        window.display();
    }
}

void allFlights::drawDottedLine(sf::Vector2f start, sf::Vector2f end)
{
    // Calculate the total distance between start and end points
    float dotSpacing = 5.0f;
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Calculate the number of dots based on the distance and dot spacing
    int numDots = static_cast<int>(distance / dotSpacing);

    // Calculate the unit vector for the direction from start to end
    float unitDx = dx / distance;
    float unitDy = dy / distance;

    // Parameters for the projectile motion (you can adjust these for more pronounced curves)
    float gravity = 0.5f;  // Simulating gravity effect
    float initialheight = start.y;  // Start height (initial y position)
    float maxheight = 50.0f;  // Maximum height of the projectile arc

    // Draw dots along the projectile curve
    for (int i = 0; i < numDots; i++)
    {
        // Calculate the position for each dot along the straight line path
        float x = start.x + unitDx * dotSpacing * i;
        float y = start.y + unitDy * dotSpacing * i;

        // Apply the projectile curve to the y position (simulate gravity)
        float timeFactor = static_cast<float>(i) / numDots;
        y += maxheight * std::sin(timeFactor * 3.14159f) - gravity * (timeFactor * timeFactor);

        // Create a circle to represent the dot
        sf::CircleShape dot(2);  // Dot size (2px radius)
        dot.setPosition(x - 2, y - 2);  // Center the dot on the calculated position
        dot.setFillColor(sf::Color::White);  // White for the dot (you can change color if needed)

        // Draw the dot
        window.draw(dot);
    }
}

void allFlights::runSubGraph(string airline)
{
    // Initialize the graphics for displaying all cities and connections
    initGraphics();

    // Main loop to display the subgraph and the flight connections
    while (window.isOpen())
    {
        sf::Event event;

        // Poll events (handle window close or keypress events)
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); // Close window when close event is triggered

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                window.close(); // Close window when Enter key is pressed
        }

        // Clear the window with a background color
        window.clear(sf::Color(20, 20, 30));  // Dark background

        // Draw all cities
        for (int i = 0; i < vertex_count; i++)
        {
            window.draw(vertexVisuals[i].shape);  // Draw city circles
            window.draw(vertexVisuals[i].label);  // Draw city labels

            // Draw only the flights for this city that belong to the specified airline
            Node* current = vertices[i].adj_list;
            while (current != NULL)
            {
                if (current->data.airline_name == airline)
                    drawFlightRoute(i, current->dest_index, current->data); // Draw the flight route for the specified airline

                current = current->next;
            }
        }

        // Display everything that was drawn on the window
        window.display();
    }
}

void allFlights::runCustomFlightSearch(string origin, string destination, string preferred_airline, string transit_city, bool transitCityFound)
{
    // Initialize SFML window
    initGraphics();

    bool directFound = false;
    bool connectingFound = false;

    int originIndex = find_vertex_index(origin);
    int destinationIndex = find_vertex_index(destination);

    if (originIndex == -1 || destinationIndex == -1)
        return;

    // Step 1: Check for direct flights from origin to destination
    Node* current = vertices[originIndex].adj_list;

    while (current != NULL)
    {
        if (current->dest_index == destinationIndex)
        {
            // Check if the airline matches the preferred airline
            if (current->data.airline_name == preferred_airline)
            {
                directFound = true;
                // Draw the direct flight route as a line using vertex positions
                sf::Vertex line[] = { sf::Vertex(vertexVisuals[originIndex].position, sf::Color::Green), sf::Vertex(vertexVisuals[destinationIndex].position, sf::Color::Green) };
                window.draw(line, 2, sf::Lines);
                break;
            }
        }
        current = current->next;
    }

    // Step 2: Check for connecting flights if a transit city is provided
    if (transitCityFound)
    {
        int transitIndex = find_vertex_index(transit_city);
        if (transitIndex == -1)
            return;

        bool firstLegFound = false;
        bool secondLegFound = false;

        // Check the first leg (origin -> transit city)
        Node* firstLeg = vertices[originIndex].adj_list;
        while (firstLeg != NULL)
        {
            if (firstLeg->data.airline_name == preferred_airline && firstLeg->dest_index == transitIndex)
            {
                firstLegFound = true;
                // Draw the first leg of the connecting flight as a line using vertex positions
                sf::Vertex line[] = { sf::Vertex(vertexVisuals[originIndex].position, sf::Color::Yellow), sf::Vertex(vertexVisuals[transitIndex].position, sf::Color::Yellow) };
                window.draw(line, 2, sf::Lines);
                break;
            }
            firstLeg = firstLeg->next;
        }

        // Check the second leg (transit city -> destination)
        if (firstLegFound)
        {
            Node* secondLeg = vertices[transitIndex].adj_list;
            while (secondLeg != NULL)
            {
                if (secondLeg->data.airline_name == preferred_airline && secondLeg->dest_index == destinationIndex)
                {
                    secondLegFound = true;
                    // Draw the second leg of the connecting flight as a line using vertex positions
                    sf::Vertex line[] = { sf::Vertex(vertexVisuals[transitIndex].position, sf::Color::Yellow), sf::Vertex(vertexVisuals[destinationIndex].position, sf::Color::Yellow) };
                    window.draw(line, 2, sf::Lines);
                    break;
                }
                secondLeg = secondLeg->next;
            }
        }
    }

    // Render cities and their labels (already in `vertexVisuals` from `initGraphics`)
    for (int i = 0; i < vertex_count; i++)
    {
        window.draw(vertexVisuals[i].shape);  // Draw city circles
        window.draw(vertexVisuals[i].label);  // Draw city labels
    }

    // Update the window
    window.display();

    // Wait for the user to close the window
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}

allFlights::~allFlights()
{
    delete[] vertexVisuals;
}