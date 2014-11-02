#include "MandelbrotRenderer.h"

#include "omp.h"


void monoThreadedMandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition, bool& isRunning, bool &finished)
{
    finished = false;

    const unsigned &resolution = detailLevel;

    const static double fractal_left = -2.1;
    const static double fractal_bottom = -1.2;
    const static double fractal_top = 1.2;

    const double zoom_y = zoom * dataSize.y / (fractal_top - fractal_bottom);
    const double zoom_x = zoom_y;

    const sf::Uint64 fractal_width  = static_cast<sf::Uint64>(dataSize.x * zoom);
    const sf::Uint64 fractal_height = static_cast<sf::Uint64>(dataSize.y * zoom);

    const sf::Uint64 baseFractal_x = static_cast<sf::Uint64>(
                                        static_cast<double>(fractal_width) * normalizedPosition.x - dataSize.x / 2);
    const sf::Uint64 baseFractal_y = static_cast<sf::Uint64>(
                                        static_cast<double>(fractal_height) * normalizedPosition.y - dataSize.y / 2);

    #pragma omp parallel for num_threads(8) schedule(dynamic,32)
    for(unsigned x = 0; x < dataSize.x; ++x)
    {
        const sf::Uint64 fractal_x = baseFractal_x + x;

        for(unsigned y = 0; y < dataSize.y && isRunning; ++y)
        {
            const sf::Uint64 fractal_y = baseFractal_y + y;

            double c_r = static_cast<double>(fractal_x) / static_cast<double>(zoom_x) + fractal_left;
            double c_i = static_cast<double>(fractal_y) / static_cast<double>(zoom_y) + fractal_bottom;
            double z_r = 0;
            double z_i = 0;

            double zi2 = z_i * z_i;
            double zr2 = z_r * z_r;

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

void mandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<double>& normalizedPosition,
                        sf::Vector2u begin, sf::Vector2u end, sf::Mutex& dataMutex, bool* isRunning)
{
    const unsigned &resolution = detailLevel;

    const static double fractal_left = -2.1;
//    const static double fractal_right = 0.6; -> unused
    const static double fractal_bottom = -1.2;
    const static double fractal_top = 1.2;

    double zoom_y = zoom * dataSize.y / (fractal_top - fractal_bottom);
    double zoom_x = zoom_y;

    const sf::Uint64 fractal_width  = static_cast<sf::Uint64>(dataSize.x * zoom);
    const sf::Uint64 fractal_height = static_cast<sf::Uint64>(dataSize.y * zoom);

    std::vector<sf::Uint8> buff((end.x - begin.x) * (end.y - begin.y) * 4, 0);

    for(unsigned x(begin.x); x < end.x && *isRunning; ++x)
    {
        for(unsigned y(begin.y); y < end.y && *isRunning; ++y)
        {
            const sf::Uint64 fractal_x = static_cast<sf::Uint64>(
                                        static_cast<double>(fractal_width) * normalizedPosition.x - dataSize.x / 2 + x );
            const sf::Uint64 fractal_y = static_cast<sf::Uint64>(
                                        static_cast<double>(fractal_height) * normalizedPosition.y - dataSize.y / 2 + y);

            double c_r = static_cast<double>(fractal_x) / static_cast<double>(zoom_x) + fractal_left;
            double c_i = static_cast<double>(fractal_y) / static_cast<double>(zoom_y) + fractal_bottom;
            double z_r = 0;
            double z_i = 0;

            unsigned i = 0;
            do
            {
                double tmp = z_r;
                z_r = z_r * z_r - z_i * z_i + c_r;
                z_i = 2 * tmp * z_i + c_i;
                i++;
            }
            while (z_r * z_r + z_i * z_i < 4 && i < resolution);

            const unsigned offset = ((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4;
            if (i == resolution)
            {
                buff[offset + 0] = static_cast<sf::Uint8>(0);
                buff[offset + 1] = static_cast<sf::Uint8>(0);
                buff[offset + 2] = static_cast<sf::Uint8>(0);
                buff[offset + 3] = static_cast<sf::Uint8>(255);
            }
            else
            {
                double t = static_cast<double>(i)/static_cast<double>(resolution);

                // Use smooth polynomials for r, g, b
                sf::Uint8 r = static_cast<sf::Uint8>(9*(1-t)*t*t*t*255);
                sf::Uint8 g = static_cast<sf::Uint8>(15*(1-t)*(1-t)*t*t*255);
                sf::Uint8 b = static_cast<sf::Uint8>(8.5*(1-t)*(1-t)*(1-t)*t*255);

                buff[offset + 0] = r;
                buff[offset + 1] = g;
                buff[offset + 2] = b;
                buff[offset + 3] = static_cast<sf::Uint8>(255);
            }
        }
    }

    if(!(*isRunning)){
        return;
    }

    dataMutex.lock();

    for(unsigned x(begin.x); x < end.x; ++x)
    {
        for(unsigned y(begin.y); y < end.y; ++y)
        {
            data[(y * dataSize.x + x) * 4 + 0] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 0];
            data[(y * dataSize.x + x) * 4 + 1] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 1];
            data[(y * dataSize.x + x) * 4 + 2] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 2];
            data[(y * dataSize.x + x) * 4 + 3] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 3];
        }
    }


    dataMutex.unlock();

}

void gmp_mandelbrotRenderer(std::vector<sf::Uint8> &data, const sf::Vector2u& dataSize, const double zoom,
                        const unsigned detailLevel, const sf::Vector2<mpf_class>& normalizedPosition,
                        sf::Vector2u begin, sf::Vector2u end, sf::Mutex& dataMutex, bool* isRunning)
{
    constexpr unsigned pre = 1024; // The precision
    const unsigned &resolution = detailLevel;

    const static double fractal_left = -2.1;
//    const static double fractal_right = 0.6; -> unused
    const static double fractal_bottom = -1.2;
    const static double fractal_top = 1.2;

    mpf_class zoom_y {zoom * dataSize.y / (fractal_top - fractal_bottom), pre};
    mpf_class zoom_x = { zoom_y, pre };

    const mpz_class fractal_width  (dataSize.x * zoom);
    const mpz_class fractal_height (dataSize.y * zoom);

    std::vector<sf::Uint8> buff((end.x - begin.x) * (end.y - begin.y) * 4, 0);

    for(unsigned x(begin.x); x < end.x && *isRunning; ++x)
    {
        for(unsigned y(begin.y); y < end.y && *isRunning; ++y)
        {
            mpz_class fractal_x = 0;
            const mpf_class tempFractalX { fractal_width  * normalizedPosition.x - dataSize.x / 2 + x, pre };
            mpz_set_f(fractal_x.get_mpz_t(), tempFractalX.get_mpf_t());

            mpz_class fractal_y = 0;
            const mpf_class tempFractalY { fractal_height * normalizedPosition.y - dataSize.y / 2 + y, pre };
            mpz_set_f(fractal_y.get_mpz_t(), tempFractalY.get_mpf_t());

            mpf_class c_r = mpf_class(fractal_x, pre) / mpf_class(zoom_x, pre) + fractal_left;
            mpf_class c_i = mpf_class(fractal_y, pre) / mpf_class(zoom_y, pre) + fractal_bottom;
            mpf_class z_r { 0, pre };
            mpf_class z_i { 0, pre };

            unsigned i = 0;
            do
            {
                mpf_class tmp { z_r, pre };
                z_r = z_r * z_r - z_i * z_i + c_r;
                z_i = 2 * tmp * z_i + c_i;
                i++;
            }
            while (z_r * z_r + z_i * z_i < 4 && i < resolution);

            if (i == resolution)
            {
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 0] = static_cast<sf::Uint8>(0);
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x))* 4 + 1] = static_cast<sf::Uint8>(0);
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 2] = static_cast<sf::Uint8>(0);
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 3] = static_cast<sf::Uint8>(255);
            }
            else
            {
                mpf_class t { mpf_class(i, pre)/mpf_class(resolution, pre) };

                // Use smooth polynomials for r, g, b
                mpz_class r = 0;
                const mpf_class tempR { 9*(1-t)*t*t*t*255, pre};
                mpz_set_f(r.get_mpz_t(), tempR.get_mpf_t());

                mpz_class g = 0;
                const mpf_class tempG { 15*(1-t)*(1-t)*t*t*255, pre };
                mpz_set_f(g.get_mpz_t(), tempG.get_mpf_t());

                mpz_class b = 0;
                const mpf_class tempB { 8.5*(1-t)*(1-t)*(1-t)*t*255, pre };
                mpz_set_f(b.get_mpz_t(), tempB.get_mpf_t());

                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 0] = r.get_ui();
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 1] = g.get_ui();
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 2] = b.get_ui();
                buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 3] = static_cast<sf::Uint8>(255);
            }
        }
    }

    // Done also if isRunning = false

    if(!(*isRunning)){
        return;
    }

    dataMutex.lock();

    for(unsigned x(begin.x); x < end.x; ++x)
    {
        for(unsigned y(begin.y); y < end.y; ++y)
        {
            data[(y * dataSize.x + x) * 4 + 0] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 0];
            data[(y * dataSize.x + x) * 4 + 1] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 1];
            data[(y * dataSize.x + x) * 4 + 2] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 2];
            data[(y * dataSize.x + x) * 4 + 3] = buff[((y-begin.y) * (end.x - begin.x) + (x-begin.x)) * 4 + 3];
        }
    }

    dataMutex.unlock();
}
