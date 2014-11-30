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

void mandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished);

void gmp_mandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                            const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished);


template<typename T>
unsigned getEscapeIterationFor(sf::Uint64 fractal_x, sf::Uint64 fractal_y, T zoom_x, T zoom_y,
                               const unsigned detailLevel)

{
    constexpr static T fractal_left = -2.1;
    constexpr static T fractal_bottom = -1.2;

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
    while (zi2 + zr2 < 4 && i < detailLevel);

    return i;
}

template <typename T>
void mandelbrotRendererPrimitive(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                                 const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished)
{
    finished = false;

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

            unsigned i = 0;

            // Optimization accorded to
            //http://en.wikibooks.org/wiki/Fractals/Iterations_in_the_complex_plane/Mandelbrot_set#Cardioid_and_period-2_checking
            // Check if the point is in the main cardioid
            // The point is in main cardioid if :
            // q = (x-1/4)²+y²
            // q(q+(x-1/4)) < 1/4 * y²
            // And in period 2 if
            // (x+1)² + y² < 1/16
            auto x_ = static_cast<T>(fractal_x) / static_cast<T>(zoom_x) + fractal_left;
            auto y_ = static_cast<T>(fractal_y) / static_cast<T>(zoom_y) + fractal_bottom;
            auto q_ = (x_ - 0.25) * (x_ - 0.25) + y_*y_;
            auto r1_ = q_ * (q_ + (x_ - 0.25 ));
            auto r2_ = (x_+1) * (x_ +1) + y_*y_;

            if(!(r1_ < 0.25*y_*y_) && !( r2_ < 1/16))
                i = getEscapeIterationFor(fractal_x, fractal_y, zoom_x, zoom_y, detailLevel);

            unsigned offset = (y * dataSize.x + x) * 4;
            if (i == detailLevel)
            {
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(0);
                data[offset++] = static_cast<sf::Uint8>(255);
            }
            else
            {
                const double t = static_cast<double>(i)/static_cast<double>(detailLevel);

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
