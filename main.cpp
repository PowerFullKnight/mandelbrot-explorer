// Sfml include
// Graphics & window
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

// Personal include
#include "Application.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Fractale", sf::Style::Fullscreen);

    Application app(window);

    while (window.isOpen())
    {
        app.handleEvent();
        app.update();

        window.clear();
        app.draw();
        window.display();
    }
    return 0;

}
