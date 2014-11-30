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
    std::vector<sf::Uint8> m_data;
    sf::Vector2u m_imageSize;
    sf::Texture m_texture;
    bool m_isRenderingFinished;

    sf::Vector2<double> m_normalizedPosition;
    sf::Vector2<mpf_class> m_gmp_normalizedPosition;
    unsigned m_detailLevel;
    bool m_autoAdjustDetail;
    double m_scale;

    sf::Thread m_renderThread;
    bool m_threadRun;

    void launchRendering() noexcept;

    void launchAllThread();
    void terminateAllThread();

public:

    Render(const unsigned width, const unsigned height);
    Render(const sf::Vector2u size);

    Render(const Render& ) = delete;

    ~Render();

    void setZoom(double zoom) noexcept;
    double getZoom() const noexcept;

    void setDetailLevel(unsigned detailLevel) noexcept;
    unsigned getDetailLevel() const noexcept;

    void setAutoAdjustDetail(bool autoAdj) noexcept;
    bool autoAdjustDetail()  const noexcept;

    void setNormalizedPosition(sf::Vector2<double> position) noexcept;
    sf::Vector2<double> getNormalizedPosition() const noexcept;

    const sf::Texture& getTexture() noexcept;

    long double getGmpRenderBeginning() const noexcept;
    long double getLongDoubleRenderBeginning() const noexcept;
    double getDoubleRenderBeginning() const noexcept;

    bool isRenderingFinished() const noexcept;

    void performRendering() noexcept;
    void performRenderingSync() noexcept; // Blockant version
    void abort() noexcept;
};

#endif // RENDER_H
