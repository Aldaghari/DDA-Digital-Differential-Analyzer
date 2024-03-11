#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SelbaWard/Line.hpp>


class World
{
private:
    sf::Vector2u vecWindowSize;
    std::vector<bool> bCells;

    int nCellWidth;
    int nWorldWidth;
    int nWolrdHeight;

    std::vector<sw::Line> vGrid;

    // Player
    sf::Vector2f vecPlayerPos;


public:
    World(sf::RenderWindow& window)
    {
        // settings
        vecWindowSize = window.getSize();
        nCellWidth = 16;
        nWorldWidth = vecWindowSize.x / nCellWidth;
        nWolrdHeight = vecWindowSize.y / nCellWidth;
        bCells.resize(nWorldWidth * nWolrdHeight);

        // grid
        sw::Line line;

        // vertical
        for (int y = 1; y < nWolrdHeight; y++)
        {
            line.setPoint(0, sf::Vector2f(0.0f, y * nCellWidth));
            line.setPoint(1, sf::Vector2f(vecWindowSize.x, y * nCellWidth));
            vGrid.push_back(line);
        }
        // Horizontal
        for (int x = 1; x < nWorldWidth; x++)
        {
            line.setPoint(0, sf::Vector2f(x * nCellWidth, 0.0f));
            line.setPoint(1, sf::Vector2f(x * nCellWidth, vecWindowSize.y));
            vGrid.push_back(line);
        }
        
        // Player
        vecPlayerPos = sf::Vector2f(0.0f, 0.0f);

    }

    void update(sf::RenderWindow& window)
    {
        updateCells(window);
        updatePlayerPos();
    }

    void draw(sf::RenderWindow& window)
    {
        // Player
        sf::CircleShape circle(15.0f);
        circle.setFillColor(sf::Color::Magenta);
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setPosition(vecPlayerPos);
        window.draw(circle);

        // Grid
        for (int i = 0; i < vGrid.size(); i++)
        {
            window.draw(vGrid[i]);
        }

        // Cells
        for (int x = 0; x < nWorldWidth; x++)
        {
            for (int y = 0; y < nWolrdHeight; y++)
            {
                if (bCells[y * nWorldWidth + x])
                {
                    sf::RectangleShape rect(sf::Vector2f(nCellWidth - 1.0f, nCellWidth - 1.0f));
                    rect.setFillColor(sf::Color::Red);
                    rect.setPosition(nCellWidth * x, nCellWidth * y);
                    window.draw(rect);
                }
            }
        }

        // Line from circle to mouse
        drawLineUsingDDA(window);
    }

    void updateCells(sf::RenderWindow& window)
    {
        sf::Vector2i vecMousePos = sf::Mouse::getPosition(window);
        // y * width + x
        int i = (vecMousePos.y / nCellWidth) * nWorldWidth + (vecMousePos.x / nCellWidth);

        if (vecMousePos.x > 0 && vecMousePos.y > 0 && vecMousePos.x < vecWindowSize.x && vecMousePos.y < vecWindowSize.y)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                bCells[i] = true;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                bCells[i] = false;            
        }
    }

    void updatePlayerPos()
    {
        float speed = 5.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            vecPlayerPos.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            vecPlayerPos.x += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            vecPlayerPos.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            vecPlayerPos.y += speed;
        
    }

    void drawLineUsingDDA(sf::RenderWindow& window)
    {
        sf::Vector2i vecMousePos = sf::Mouse::getPosition(window);
            
        sw::Line line;
        line.setColor(sf::Color::Green);
        line.setPoints(vecPlayerPos, sf::Vector2f(vecMousePos));
        window.draw(line);

        // DDA
        int x0 = vecPlayerPos.x, y0 = vecPlayerPos.y;
        int dx = vecMousePos.x - vecPlayerPos.x, dy = vecMousePos.y - vecPlayerPos.y;
        int steps;
        float xIncremental, yIncremental, x = x0, y = y0;

        // Check if m < 1
        if (abs(dx) > abs(dy))
            steps = abs(dx);
        else
            steps = abs(dy);

        xIncremental = ((float)dx / (float)steps);
        yIncremental = ((float)dy / (float)steps);
        
        sf::CircleShape circle(6.0f);
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(0.9f);

        //std::cout << (float)dy / (float)dx << "\n";
        for (int i = 0; i < steps; i++)
        {
            x += xIncremental;
            y += yIncremental;


            if (vecMousePos.x > 0 && vecMousePos.y> 0 && vecMousePos.x < vecWindowSize.x && vecMousePos.y < vecWindowSize.y)
            {
                int i = ((int)(y + 0.5f) / nCellWidth) * nWorldWidth + ((int)(x + 0.5f) / nCellWidth);
                //std::cout << "x: " << x << " y: " << y << std::endl;
                if (bCells[i])
                {
                    circle.setPosition(x, y);
                    window.draw(circle);
                    break;
                }
            }
        }

    }

};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 640), "My window");
    window.setFramerateLimit(60);

    World world(window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        world.update(window);
        world.draw(window);
        window.display();
    }

    return 0;
}