#include "Render.h"

// Std include
#include <functional>    // std::bind
#include <stdexcept>

// Personal include
#include "RenderThread.h"
#include "MandelbrotRenderer.h"


Render::Render(const unsigned width, const unsigned height):
    m_data(width * height * 4, 0),
    m_imageSize(width, height),
    m_texture(),
    m_normalizedPosition(0.4, 0.5),
    m_gmp_normalizedPosition(),
    m_detailLevel(30),
    m_scale(1.0),
    m_threads(),
    m_dataMutex(),
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

    initialize4Thread();
}

Render::Render(const sf::Vector2u size):
    Render(size.x, size.y)
{
}

// PRIVATE
void Render::initialize4Thread()
{
    m_threads.clear();

    const unsigned w = m_imageSize.x;
    const unsigned h = m_imageSize.y;


    m_threads.push_back( new RenderThread(0, sf::Vector2u(0, 0), sf::Vector2u(w / 2, h / 2)) );
    m_threads.push_back( new RenderThread(1, sf::Vector2u(w / 2, 0), sf::Vector2u(w, h / 2)) );
    m_threads.push_back( new RenderThread(2, sf::Vector2u(0, h / 2), sf::Vector2u(w / 2, h)) );
    m_threads.push_back( new RenderThread(3, sf::Vector2u(w / 2, h / 2), sf::Vector2u(w, h)) );

    for(std::size_t i(0); i< m_threads.size();++i){
       m_threads.at(i)->setFunc(std::bind(&Render::performRenderingImpl, this, m_threads.at(i)->begin(), m_threads.at(i)->end()));
    }

}
// PUBLIC

Render::~Render()
{
    for(auto thread : m_threads){
        delete thread;
    }
}

void Render::setZoom(double zoom) noexcept
{
    m_scale = zoom;
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

void Render::performRendering() noexcept
{
    terminateAllThread();
    launchAllThread();
}

void Render::abort() noexcept
{
    terminateAllThread();
}

double Render::getGmpRenderBeginning() const noexcept
{
    return 1e13;
}

// PRIVATE
void Render::performRenderingImpl(sf::Vector2u begin, sf::Vector2u end) noexcept
{
    if(m_scale > getGmpRenderBeginning() && m_scale / 1.3 < getGmpRenderBeginning()){
            mpf_set_d(m_gmp_normalizedPosition.x.get_mpf_t(), m_normalizedPosition.x);
            mpf_set_d(m_gmp_normalizedPosition.y.get_mpf_t(), m_normalizedPosition.y);
    }
    if(m_scale > getGmpRenderBeginning()){
        gmp_mandelbrotRenderer(m_data, m_imageSize, m_scale, m_detailLevel, m_gmp_normalizedPosition, begin, end, m_dataMutex, &m_threadRun);
    }
    else{
        mandelbrotRenderer(m_data, m_imageSize, m_scale, m_detailLevel, m_normalizedPosition, begin, end, m_dataMutex, &m_threadRun);
    }
}

void Render::launchAllThread()
{
    m_threadRun = true;
    for(auto *thread : m_threads){
        thread->run();
    }
}

void Render::terminateAllThread()
{
    m_threadRun = false;
    for(auto *thread : m_threads)
    {
        thread->wait();
    }
}
