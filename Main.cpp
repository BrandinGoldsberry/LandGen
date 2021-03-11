#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "TerrainGen.cpp"

using namespace bgTG;

void gen_land(sf::CircleShape* dotList, int gridRes, int* screenRes) {
    //interesting seeds
    //5101
    //1610165314
    unsigned int seed = time(NULL);
    std::cout << seed << std::endl;
    srand(seed);

    int** dotGrids = generate_continents(gridRes, 10, 20, 10);

    float dotSize = 1;
    float dotSpacing_x = (screenRes[0] / gridRes);
    float dotSpacing_y = (screenRes[1] / gridRes);

    for (int i = 0; i < gridRes; i++)
    {
        for (int j = 0; j < gridRes; j++)
        {
            sf::CircleShape newDot(dotSize);
            switch (dotGrids[i][j])
            {
            case 0:
                newDot.setFillColor(sf::Color::Blue);
                break;
            case 1:
                newDot.setFillColor(sf::Color(255, 225, 176, 255));
                break;
            case 2:
                newDot.setFillColor(sf::Color::Green);
                break;
            case 3:
                newDot.setFillColor(sf::Color(138, 125, 102));
                break;
            default:
                newDot.setFillColor(sf::Color::Green);
                break;
            }
            newDot.setPosition((j * dotSpacing_y) + dotSize, (i * dotSpacing_x) + dotSize);
            dotList[(i * gridRes) + j] = newDot;
        }
    }
}

int main()
{
    int screenRes[] = { 600, 600 };
    int gridRes = 200;
    sf::CircleShape* dotList = new sf::CircleShape[gridRes * gridRes];
    sf::RenderWindow window(sf::VideoMode(screenRes[0], screenRes[1]), "Territory");
    gen_land(dotList, gridRes, screenRes);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    gen_land(dotList, gridRes, screenRes);
                }
            }
        }

        window.clear();
        for (int i = 0; i < gridRes * gridRes; i++)
        {
            window.draw(dotList[i]);
        }
        window.display();
    }

    return 0;
}