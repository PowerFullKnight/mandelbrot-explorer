#include "Application.h"

// Std include
#include <sstream>
#include <stdexcept>
#include <cstdlib> // for save screen
#include <ctime> // for save screen

// Sfml include
// - Graphics
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

// System
#include <SFML/System/Sleep.hpp>

Application::Application(sf::RenderWindow& window):
    m_window(window),
    m_fractaleSprite(),
    m_fractaleRenderer(window.getSize()),
    m_font(),
    m_showText(true),
    m_sound(),
    m_photoBuffer(),
    m_isMousePressed(false),
    m_mouseSelection(),
    m_clock(),
    m_lastTime(),
    m_doAction(false)
{
    if(!m_font.loadFromFile("arial.ttf")){
        m_showText = false;
    }
    if(!m_photoBuffer.loadFromFile("photo.ogg")){
        throw std::runtime_error("Can not load sound \"photo.ogg\"");
    }
    m_sound.setBuffer(m_photoBuffer);

    m_fractaleRenderer.performRendering();
}

void Application::handleEvent()
{
    sf::Event event;
    while(m_window.pollEvent(event))
    {
        switch(event.type)
        {
        case sf::Event::Closed:
            m_window.close();
            break;
        case sf::Event::MouseMoved:
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
            handleMouseEvent(event);
            break;
        case sf::Event::KeyPressed:
            handleKeyPressedEvent(event);
            break;
        default:
            break;
        }
    }
}

void Application::update()
{
    if(m_fractaleRenderer.isRenderingFinished())
        m_fractaleSprite.setTexture(m_fractaleRenderer.getTexture());

    if(!m_doAction){
        if(m_clock.getElapsedTime() > sf::seconds(0.1))
        {
            m_doAction = true;
            m_fractaleRenderer.performRendering();
        }
    }
}

void Application::draw()
{
    m_window.draw(m_fractaleSprite);
    if(m_isMousePressed)
    {
        sf::RectangleShape rect(sf::Vector2f(m_mouseSelection.width, m_mouseSelection.height));
        rect.setPosition(m_mouseSelection.left, m_mouseSelection.top);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color(100,100,100));
        rect.setOutlineThickness(2);
        m_window.draw(rect);
    }
    drawInfo();
}

// PRIVATE
void Application::handleMouseEvent(sf::Event event)
{
    switch(event.type)
    {
    case sf::Event::MouseButtonPressed:
        m_isMousePressed = true;
        m_mouseSelection = sf::Rect<int>();
        m_mouseSelection.left = event.mouseButton.x;
        m_mouseSelection.top = event.mouseButton.y;
        break;
    case sf::Event::MouseButtonReleased:
        m_isMousePressed = false;
        break;
    case sf::Event::MouseMoved:
        if(m_isMousePressed)
        {
            m_mouseSelection.width  = event.mouseMove.x - m_mouseSelection.left;
            m_mouseSelection.height = event.mouseMove.y - m_mouseSelection.top;
        }
        break;
    default:
        break;
    }
}
void Application::handleKeyPressedEvent(sf::Event event)
{
    sf::Time time;
    switch(event.key.code)
    {
        // Movement
    case sf::Keyboard::Left:
    case sf::Keyboard::Right:
    case sf::Keyboard::Up:
    case sf::Keyboard::Down:
        // Resolution
    case sf::Keyboard::A:
    case sf::Keyboard::Q:
    case sf::Keyboard::LControl:
        // Zoom
    case sf::Keyboard::Z:
    case sf::Keyboard::S:
        time = m_clock.restart();
        break;
    default:
        break;
    }


    if(time - m_lastTime > sf::seconds(0.1))
    {
        m_doAction = true;
    }
    else{
        m_doAction = false;
    }
    m_lastTime = time;

    switch(event.key.code)
    {
        // Movement
    case sf::Keyboard::Left:
        move(Direction::Left);
        break;
    case sf::Keyboard::Right:
        move(Direction::Right);
        break;
    case sf::Keyboard::Up:
        move(Direction::Up);
        break;
    case sf::Keyboard::Down:
        move(Direction::Down);
        break;
        // Resolution
    case sf::Keyboard::A:
        increaseDetail();
        break;
    case sf::Keyboard::Q:
        decreaseDetail();
        break;
        // Zoom
    case sf::Keyboard::Z:
        zoom();
        break;
    case sf::Keyboard::S:
        unzoom();
        break;
        // Screen
    case sf::Keyboard::E:
        takeScreen();
        break;
        // Panel
    case sf::Keyboard::H:
        togglePanel();
        break;
        // Refresh & stop
    case sf::Keyboard::R:
        refresh();
        break;
    case sf::Keyboard::T:
        stop();
        break;
        // Quit
    case sf::Keyboard::Escape:
        m_window.close();
        break;
    default:
        break;
    }
}

// KEY EVENT
void Application::move(Direction dir)
{
    sf::Vector2<double> position = m_fractaleRenderer.getNormalizedPosition();
    double renderZoom = m_fractaleRenderer.getZoom();
    double offset = 0.1 / renderZoom;
    switch(dir)
    {
        case Direction::Left  : position.x -= offset; break;
        case Direction::Right : position.x += offset; break;
        case Direction::Up    : position.y -= offset; break;
        case Direction::Down  : position.y += offset; break;
        default: break;
    }

    m_fractaleRenderer.setNormalizedPosition(position);
    m_fractaleRenderer.performRendering();
}

void Application::increaseDetail()
{
    unsigned step = 1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
        step = 10;
    }
    if(step > m_fractaleRenderer.getDetailLevel()){
        m_fractaleRenderer.setDetailLevel(1);
    }
    else{
        m_fractaleRenderer.setDetailLevel(m_fractaleRenderer.getDetailLevel() + step);
    }
    if(m_doAction)
        m_fractaleRenderer.performRendering();
}

void Application::decreaseDetail()
{
    unsigned step = 1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
        step = 10;
    }
    auto detail  = m_fractaleRenderer.getDetailLevel() - step;
    if(detail == 0){
        detail = 1;
    }
    m_fractaleRenderer.setDetailLevel(detail);
    if(m_doAction)
        m_fractaleRenderer.performRendering();
}

void Application::zoom()
{
    double renderZoom = m_fractaleRenderer.getZoom();
    m_fractaleRenderer.setZoom(renderZoom * 1.3);
    if(m_doAction)
        m_fractaleRenderer.performRendering();
}

void Application::unzoom()
{
    double renderZoom = m_fractaleRenderer.getZoom();
    m_fractaleRenderer.setZoom(renderZoom / 1.3);
    if(m_doAction)
        m_fractaleRenderer.performRendering();
}

void Application::takeScreen()
{
    m_sound.play();
    sf::Image screen = m_window.capture();
    std::ostringstream fileName;
    fileName << "screen-" << time(nullptr) << "-" << rand() % 1000 << ".png";
    screen.saveToFile(fileName.str());

    while (m_sound.getStatus() == sf::Sound::Playing)
            sf::sleep(sf::milliseconds(10));
}

void Application::togglePanel()
{
    m_showText = !m_showText;
}

void Application::refresh()
{
    m_fractaleRenderer.performRendering();
}

void Application::stop()
{
    m_fractaleRenderer.abort();
}

void Application::drawInfo() noexcept
{
    if(!m_showText)
        return;

    sf::Text infoText;
    infoText.setFont(m_font);
    infoText.setCharacterSize(20);
    infoText.setColor(sf::Color::Blue);
    infoText.setPosition(5, 5);

    std::ostringstream oss;
    oss << "Z/ S : Zoom : x" << m_fractaleRenderer.getZoom() << "\n"
           "A / Q : Niveau de détails : " << m_fractaleRenderer.getDetailLevel() << "\n"
           "Position : " << m_fractaleRenderer.getNormalizedPosition().x << "; " << m_fractaleRenderer.getNormalizedPosition().y << "\n"
           "E : Prendre une photo\n"
           "H : Texte visible\n"
           "R : Rafraichir ( si ça bug )\n"
           "T : Arreter le rendu en cours";

    if(m_fractaleRenderer.getZoom() > m_fractaleRenderer.getGmpRenderBeginning()){
        oss<<"\nUsing GMP";
    }

    infoText.setString(oss.str());

    sf::RectangleShape background(sf::Vector2f(300, 200));
    background.setFillColor(sf::Color(50, 50, 50, 150));

    m_window.draw(background);
    m_window.draw(infoText);

}
