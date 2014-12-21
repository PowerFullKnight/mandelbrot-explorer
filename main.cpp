// Sfml include
// Graphics & window
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

// Personal include
#include "Application.h"

#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Fractale", sf::Style::Fullscreen);
   // sf::RenderWindow window(sf::VideoMode(160*2, 90*2), "Fractale");

    Application app(window);

    sf::Clock clock;

    while (window.isOpen())
    {
        app.handleEvent();
        app.update();

        window.clear();
        app.draw();
        window.display();

       // std::cout << 1.0 / clock.restart().asSeconds()<< '\n';
    }

    return 0;
}
