#ifndef MANDELBROTRENDERER_H
#define MANDELBROTRENDERER_H

// Std include
#include <vector>

// Sfml include
// - System
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/Config.hpp> // For uint etc ...

// Gmp include
#include <gmpxx.h>

void monoThreadedMandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished);

void gmp_mandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<mpf_class>& normalizedPosition,
                        sf::Vector2u begin, sf::Vector2u end, sf::Mutex& dataMutex, bool* isRunning);


template <typename T>
void monoThreadedMandelbrotRendererPrimitive(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished)
{
     finished = false;

    const unsigned &resolution = detailLevel;

    constexpr static T fractal_left = -2.1;
    constexpr static T fractal_bottom = -1.2;
    constexpr static T fractal_top = 1.2;

    const T zoom_y = zoom * dataSize.y / (fractal_top - fractal_bottom);
    const T zoom_x = zoom_y;

    const sf::Uint64 fractal_width  = static_cast<sf::Uint64>(dataSize.x * zoom);
    const sf::Uint64 fractal_height = static_cast<sf::Uint64>(dataSize.y * zoom);

    const sf::Uint64 baseFractal_x = static_cast<sf::Uint64>(
                                        static_cast<T>(fractal_width) * normalizedPosition.x - dataSize.x / 2);
    const sf::Uint64 baseFractal_y = static_cast<sf::Uint64>(
                                        static_cast<T>(fractal_height) * normalizedPosition.y - dataSize.y / 2);

    #pragma omp parallel for num_threads(8) schedule(dynamic,32)
    for(unsigned x = 0; x < dataSize.x; ++x)
    {
        const sf::Uint64 fractal_x = baseFractal_x + x;

        for(unsigned y = 0; y < dataSize.y && isRunning; ++y)
        {
            const sf::Uint64 fractal_y = baseFractal_y + y;

            T c_r = static_cast<T>(fractal_x) / static_cast<T>(zoom_x) + fractal_left;
            T c_i = static_cast<T>(fractal_y) / static_cast<T>(zoom_y) + fractal_bottom;
            T z_r = 0;
            T z_i = 0;

            T zi2 = z_i * z_i;
            T zr2 = z_r * z_r;

            unsigned i = 0;
            do
            {
                z_i = (z_r + z_r) * z_i + c_i;
                z_r = zr2 - zi2 + c_r;

                zi2 = z_i * z_i;
                zr2 = z_r * z_r;

                i++;
            }
            while (zi2 + zr2 < 4 && i < resolution);

            unsigned offset = (y * dataSize.x + x) * 4;
            if (i == resolution)
            {
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(255);
            }
            else
            {
                const double t = static_cast<double>(i)/static_cast<double>(resolution);

                // Use smooth polynomials for r, g, b
                sf::Uint8 r = static_cast<sf::Uint8>(9*(1-t)*t*t*t*255);
                sf::Uint8 g = static_cast<sf::Uint8>(15*(1-t)*(1-t)*t*t*255);
                sf::Uint8 b = static_cast<sf::Uint8>(8.5*(1-t)*(1-t)*(1-t)*t*255);

                data[offset++] = r;
                data[offset++] = g;
                data[offset++] = b;
                data[offset++] = static_cast<sf::Uint8>(255);
            }
        }
    }
    finished = true;
}

#endif // MANDELBROTRENDERER_H
