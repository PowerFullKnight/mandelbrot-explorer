#ifndef MANDELBROTRENDERER_H
#define MANDELBROTRENDERER_H

// Sfml include
// - System
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/Config.hpp> // For uint etc ...

// Gmp include
#include <gmpxx.h>

void mandelbrotRenderer(sf::Uint8* data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition,
                        sf::Vector2u begin, sf::Vector2u end, sf::Mutex& dataMutex, bool* isRunning);

void gmp_mandelbrotRenderer(sf::Uint8* data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<mpf_class>& normalizedPosition,
                        sf::Vector2u begin, sf::Vector2u end, sf::Mutex& dataMutex, bool* isRunning);



#endif // MANDELBROTRENDERER_H
