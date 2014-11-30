#include "Render.h"

// Std include
#include <functional>    // std::bind
#include <stdexcept>
#include <quadmath.h>
#include <cmath>

// Personal include
#include "RenderThread.h"
#include "MandelbrotRenderer.h"


Render::Render(const unsigned width, const unsigned height):
    m_data(width * height * 4, 0),
    m_imageSize(width, height),
    m_texture(),
    m_isRenderingFinished(true),
    m_normalizedPosition(0.4, 0.5),
    m_gmp_normalizedPosition(),
    m_detailLevel(30),
    m_autoAdjustDetail(true),
    m_scale(1.0),
    m_renderThread(&Render::launchRendering, this),
    m_threadRun(false)
{
    if(m_texture.create(m_imageSize.x, m_imageSize.y))
    {
        m_texture.setSmooth(false);
    }
    else
    {
        throw std::runtime_error("Texture is too big for your computer.");
    }

}

Render::Render(const sf::Vector2u size):
    Render(size.x, size.y)
{}

Render::~Render()
{}

void Render::setZoom(double zoom) noexcept
{
    m_scale = zoom;
    if(m_autoAdjustDetail){
        m_detailLevel = sqrt(abs(2*sqrt(abs(1-sqrt(5*m_scale)))))*66.5;
    }if(m_detailLevel == 0){
        m_detailLevel = 30;
    }
}

double Render::getZoom() const noexcept
{
    return m_scale;
}

void Render::setDetailLevel(unsigned detailLevel) noexcept
{
    m_detailLevel = detailLevel;
}

unsigned Render::getDetailLevel() const noexcept
{
    return m_detailLevel;
}

void Render::setAutoAdjustDetail(bool autoAdj) noexcept
{
     m_autoAdjustDetail = autoAdj;
}

bool Render::autoAdjustDetail() const noexcept
{
    return m_autoAdjustDetail;
}

void Render::setNormalizedPosition(sf::Vector2<double> position) noexcept
{
    m_normalizedPosition = position;
    if(m_scale > getGmpRenderBeginning()){
            mpf_set_d(m_gmp_normalizedPosition.x.get_mpf_t(), m_normalizedPosition.x);
            mpf_set_d(m_gmp_normalizedPosition.y.get_mpf_t(), m_normalizedPosition.y);
    }
}

sf::Vector2<double> Render::getNormalizedPosition() const noexcept
{
    if(m_scale > getGmpRenderBeginning()){
        return sf::Vector2<double>(m_gmp_normalizedPosition.x.get_d(), m_gmp_normalizedPosition.y.get_d());
    }
    return m_normalizedPosition;
}

const sf::Texture& Render::getTexture()noexcept
{
    m_texture.update(m_data.data());
    return m_texture;
}

bool Render::isRenderingFinished() const noexcept
{
    return m_isRenderingFinished;
}

void Render::performRendering() noexcept
{
    terminateAllThread();
    launchAllThread();
}

void Render::performRenderingSync() noexcept
{
    launchRendering();
}

void Render::abort() noexcept
{
    terminateAllThread();
}

long double Render::getGmpRenderBeginning() const noexcept
{
    return 1e25;
}

long double Render::getLongDoubleRenderBeginning() const noexcept
{
    return 1e13;
}

double Render::getDoubleRenderBeginning() const noexcept
{
    return 2e4;
}

// PRIVATE
void Render::launchRendering() noexcept
{
    if(m_scale < getDoubleRenderBeginning())
        mandelbrotRendererPrimitive<float>(m_data, m_imageSize, m_scale, m_detailLevel, m_normalizedPosition, m_threadRun, m_isRenderingFinished);
    else if(m_scale < getLongDoubleRenderBeginning())
        mandelbrotRendererPrimitive<double>(m_data, m_imageSize, m_scale, m_detailLevel, m_normalizedPosition, m_threadRun, m_isRenderingFinished);
    else
        mandelbrotRendererPrimitive<__float128>(m_data, m_imageSize, m_scale, m_detailLevel, m_normalizedPosition, m_threadRun, m_isRenderingFinished);
}

void Render::launchAllThread()
{
    m_renderThread.launch();
    m_threadRun = true;
}

void Render::terminateAllThread()
{
    m_renderThread.wait();
    m_threadRun = false;
}
