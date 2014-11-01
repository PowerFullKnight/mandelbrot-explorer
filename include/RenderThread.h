#ifndef RENDERTHREAD_H_INCLUDED
#define RENDERTHREAD_H_INCLUDED

// Sfml include
// - System
#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>

class RenderThread : public sf::NonCopyable
{
public:
    RenderThread(const unsigned id, sf::Vector2u begin_, sf::Vector2u end_):
        m_id(id), m_thread(nullptr), m_begin(begin_), m_end(end_), m_run(false)
    {}

    ~RenderThread()
    {
        m_thread->terminate();
        delete m_thread;
    }

    template <typename Func>
    void setFunc(Func func)
    {
        m_thread = new sf::Thread(func);
    }

    void run()
    {
        m_thread->launch();
    }

    void stop()
    {
        m_thread->terminate();
    }

    void wait()
    {
        m_thread->wait();
    }

    sf::Vector2u begin() const { return m_begin; }
    sf::Vector2u end() const { return m_end; }

private:
    const unsigned m_id;
    sf::Thread *m_thread;
    const sf::Vector2u m_begin;
    const sf::Vector2u m_end;
    bool m_run;
};

#endif // RENDERTHREAD_H_INCLUDED
