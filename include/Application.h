#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include<SFML/Audio/Sound.hpp>
#include<SFML/Audio/SoundBuffer.hpp>

#include "Render.h"

class Application
{
    enum class Direction{
        Up,
        Down,
        Right,
        Left
    };
    public:
        Application(sf::RenderWindow& window);

        void handleEvent();

        void update();

        void draw();

    private:

        void handleKeyPressedEvent(sf::Event event);

        void drawInfo() noexcept;

        // Key event
        void move(Direction dir);
        void increaseDetail();
        void decreaseDetail();
        void zoom();
        void unzoom();
        void takeScreen();
        void togglePanel();
        void refresh();
        void stop();

        // Rendering
        sf::RenderWindow& m_window;
        sf::Sprite m_fractaleSprite;
        Render m_fractaleRenderer;

        sf::Font m_font;
        bool m_showText;

        sf::Sound m_sound;
        sf::SoundBuffer m_photoBuffer;

        // Time
        sf::Clock m_clock;
        sf::Time m_lastTime;
        bool m_doAction;
};

#endif // APPLICATION_H
