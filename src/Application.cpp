#include "Application.h"

// Std include
#include <sstream>
#include <stdexcept>
#include <cstdlib> // to save screen
#include <ctime> // to save screen
#include <map>
#include <cmath>
#include <string>
#include <iostream>

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
    m_actionHappened(false),
    m_changeTexture(true)
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
    if(m_fractaleRenderer.isRenderingFinished() && m_changeTexture){
        m_fractaleSprite.setTexture(m_fractaleRenderer.getTexture());
        m_changeTexture = false;
    }


    if(m_actionHappened && doAction()){
        m_fractaleRenderer.performRendering();
        m_actionHappened = false;
        m_changeTexture = true;
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
        rect.setOutlineColor(sf::Color(200,255,200));
        rect.setOutlineThickness(1);
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

    m_actionHappened = true;
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
        // Auto adjust resolution
    case sf::Keyboard::D:
        toggleAutoAdjust();
        m_actionHappened = false; // No need to recalculate
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
        m_actionHappened = false; // No need to recalculate
        break;
        // Panel
    case sf::Keyboard::H:
        togglePanel();
        m_actionHappened = false; // No need to recalculate
        break;
        // Refresh & stop
    case sf::Keyboard::R:
        refresh();
        m_actionHappened = false; // No need to recalculate ( refresh below )
        break;
    case sf::Keyboard::V:
        video();
        m_actionHappened = false; // No need to recalculate
        break;
    // Quit
    case sf::Keyboard::Escape:
        m_window.close();
        m_actionHappened = false; // No need to recalculate
        break;
    default:
        m_actionHappened = false;
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
}

void Application::increaseDetail()
{
    unsigned step = 1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
        step = 10;
    }
    m_fractaleRenderer.setDetailLevel(m_fractaleRenderer.getDetailLevel() + step);
}

void Application::decreaseDetail()
{
    unsigned step = 1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
        step = 10;
    }
    auto detail  = m_fractaleRenderer.getDetailLevel() - step;
    if(detail == 0 || detail > m_fractaleRenderer.getDetailLevel()){ // Against under flow
        detail = 1;
    }
    m_fractaleRenderer.setDetailLevel(detail);
}

void Application::zoom()
{
    double renderZoom = m_fractaleRenderer.getZoom();
    m_fractaleRenderer.setZoom(renderZoom * 1.3);
}

void Application::unzoom()
{
    double renderZoom = m_fractaleRenderer.getZoom();
    m_fractaleRenderer.setZoom(renderZoom / 1.3);
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

void Application::toggleAutoAdjust()
{
    m_fractaleRenderer.setAutoAdjustDetail(!m_fractaleRenderer.autoAdjustDetail());
}

void Application::refresh()
{
    m_fractaleRenderer.performRendering();
}

void Application::video()
{
    m_window.close();
    auto date = time(nullptr);
    auto fractZoom = m_fractaleRenderer.getZoom();
    const auto factor = 1.05;
    const auto maxImg = floor(exp(log(fractZoom) / factor)); // log = logarithm neperien

    unsigned j {0};
    for(decltype(fractZoom) i { 1 }; i < fractZoom; i *= factor)
    {
        m_fractaleRenderer.setZoom(i);
        m_fractaleRenderer.performRenderingSync();
        std::cout << j << " / " << maxImg << '\n';

        sf::Image screen = m_fractaleRenderer.getTexture().copyToImage();
        std::ostringstream fileName;
        fileName << "video/video-" << date << "-" << j << ".png";
        screen.saveToFile(fileName.str());
        ++j;
    }
}

bool Application::isControlKeyPressed() const
{
    sf::Keyboard::Key controlKey[9] = {
        sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up,
        sf::Keyboard::Down, sf::Keyboard::A, sf::Keyboard::Q,
        sf::Keyboard::LControl, sf::Keyboard::Z, sf::Keyboard::S};

    for(int i {0}; i < 9; ++i){
        if(sf::Keyboard::isKeyPressed(controlKey[i]))
            return true;
    }
    return false;
}

bool Application::doAction() const
{
    return !isControlKeyPressed();
}

void Application::drawInfo() noexcept
{
    if(!m_showText)
        return;

    sf::Text infoText;
    infoText.setFont(m_font);
    infoText.setCharacterSize(18);
    infoText.setColor(sf::Color::Blue);
    infoText.setPosition(5, 5);

    // Info
    std::ostringstream oss;
    oss << "Z / S : Zoom ; A / Q Details; D Ajustement auto\n"
           "E : Prendre une photo\n"
           "H : Texte visible\n"
           "R : Rafraichir ( si ça bug )";

    if(m_fractaleRenderer.getZoom() > m_fractaleRenderer.getGmpRenderBeginning()){
        oss<<"\nUsing GMP";
    }else if(m_fractaleRenderer.getZoom() > m_fractaleRenderer.getLongDoubleRenderBeginning()){
        oss<<"\nUsing Long Double";
    }else if(m_fractaleRenderer.getZoom() > m_fractaleRenderer.getDoubleRenderBeginning()){
        oss<<"\nUsing Double";
    }else{
        oss <<"\nUsing Float";
    }


    if(m_fractaleRenderer.isRenderingFinished()){
        oss << "\nRender finished";
    }else{
        oss << "\nRendering ...";
    }

    infoText.setString(oss.str());

    sf::RectangleShape background(sf::Vector2f(infoText.getGlobalBounds().width+10, infoText.getGlobalBounds().height+15));
    background.setFillColor(sf::Color(50, 50, 50, 150));

    m_window.draw(background);
    m_window.draw(infoText);

    // Zoom info
    oss.str("");

    const std::string zoomText = getZoomText(m_fractaleRenderer.getZoom());
    oss << "\nZoom : " << m_fractaleRenderer.getZoom() <<
           "\nDétails : " << m_fractaleRenderer.getDetailLevel();
    if(m_fractaleRenderer.autoAdjustDetail()){
        oss << " Auto";
    }
    oss << "\nPosition : " << m_fractaleRenderer.getNormalizedPosition().x << "; " << m_fractaleRenderer.getNormalizedPosition().y;
    if(!zoomText.empty()){
        oss << "\nVous regardez " << zoomText;
    }
    infoText.setString(oss.str());
    infoText.setPosition(m_window.getSize().x - infoText.getGlobalBounds().width,
                         m_window.getSize().y - infoText.getGlobalBounds().height-30);
    sf::RectangleShape backgroundZoomInfo(sf::Vector2f(infoText.getGlobalBounds().width+2, infoText.getGlobalBounds().height+10));
    backgroundZoomInfo.setPosition(m_window.getSize().x - infoText.getGlobalBounds().width - 2,
                                   m_window.getSize().y - infoText.getGlobalBounds().height - 12);
    backgroundZoomInfo.setFillColor(sf::Color(50, 50, 50, 150));

    m_window.draw(backgroundZoomInfo);
    m_window.draw(infoText);


}

std::string Application::getZoomText(double zoom) const noexcept
{
    static const std::map<int, std::string> rapport =
    {
        {-19, "un quark"},
        {-14, "un noyau atomique"},
        {-13, "le vide dans l'atome"},
        {-12, "le vide dans l'atome"},
        {-11, "des éléctrons"},
        {-10, "un atome"},
        {-9,  "une molécule"},
        {-8,  "de l'adn" },
        {-7,  "l'intérieur du noyau d'une cellule"},
        {-6,  "l'intérieur d'une cellule"},
        {-5,  "une cellule"},
        {-4,  "l'oeil d'une mouche"},
        {-3,  "un grain de sable"},
        {-2,  "une mouche"},
        {-1,  "une feuille"}
    };

    unsigned exposant(0);
    // Getting nbr of number in zoom
    do
    {
        zoom /= 10;
        ++exposant;
    }while(zoom > 1);

    std::string text;
    auto found = rapport.find(exposant*-1);
    if(found != rapport.end()){
        text = found->second;
    }
    return text;
}
