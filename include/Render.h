#ifndef RENDER_H
#define RENDER_H

// Std include
#include <vector>

// Sfml include
// - Graphics
#include <SFML/Graphics/Texture.hpp>

// - System
#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Vector2.hpp>

// Gmp include
#include <gmpxx.h>

// Personal include
#include "RenderThread.h"

typedef double real;

class Render
{
    sf::Uint8* m_data;
    sf::Vector2u m_imageSize;
    sf::Texture m_texture;

    sf::Vector2<double> m_normalizedPosition;
    sf::Vector2<mpf_class> m_gmp_normalizedPosition;
    unsigned m_detailLevel;
    double m_scale;

    std::vector< RenderThread* > m_threads;
    sf::Mutex m_dataMutex;
    bool m_threadRun;

    void performRenderingImpl(sf::Vector2u begin, sf::Vector2u end) noexcept;
    void launchAllThread();
    void terminateAllThread();

    void initialize4Thread();

public:

    Render(const unsigned width, const unsigned height);
    Render(const sf::Vector2u size);

    Render(const Render& ) = delete;

    ~Render();

    void setZoom(double zoom) noexcept;
    double getZoom() const noexcept;

    void setDetailLevel(unsigned detailLevel) noexcept;
    unsigned getDetailLevel() const noexcept;

    void setNormalizedPosition(sf::Vector2<double> position) noexcept;
    sf::Vector2<double> getNormalizedPosition() const noexcept;

    const sf::Texture& getTexture() noexcept;

    double getGmpRenderBeginning() const noexcept;

    void performRendering() noexcept;
    void abort() noexcept;
};

#endif // RENDER_H
