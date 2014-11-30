#ifndef APPLICATION_H
#define APPLICATION_H

// Sfml include
// - Graphics
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Font.hpp>

// - Window
#include <SFML/Window/Event.hpp>

// - Audio
#include<SFML/Audio/Sound.hpp>
#include<SFML/Audio/SoundBuffer.hpp>

// - System
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

// Personal include
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

        void handleMouseEvent(sf::Event event);
        void handleKeyPressedEvent(sf::Event event);

        void drawInfo() noexcept;
        std::string getZoomText(double zoom) const noexcept;

        // Key event
        void move(Direction dir);
        void increaseDetail();
        void decreaseDetail();
        void zoom();
        void unzoom();
        void takeScreen();
        void togglePanel();
        void toggleAutoAdjust();
        void refresh();
        void video();

        bool isControlKeyPressed() const;
        bool doAction() const;

        // Rendering
        sf::RenderWindow& m_window;
        sf::Sprite m_fractaleSprite;
        Render m_fractaleRenderer;

        sf::Font m_font;
        bool m_showText;

        // Audio
        sf::Sound m_sound;
        sf::SoundBuffer m_photoBuffer;

        // Mouse
        bool m_isMousePressed;
        sf::Rect<int> m_mouseSelection;

        // Time
        sf::Clock m_clock;
        sf::Time m_lastTime;
        bool m_actionHappened;
        bool m_changeTexture;
};

#endif // APPLICATION_H
