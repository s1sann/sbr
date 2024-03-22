#include <SDL.h>
#include <cmath>
#include <complex>
#include <iostream>
#include <numeric>

int width = 1000;
int height = 1000;

int is_in_set(std::complex< double > c)
{
    std::complex< double > z(0, 0);
    for (int i = 0; i < 100; i++)
    {
        z = std::pow(z, 2) + c;
        if (std::norm(z) > 10)
        {
            return i;
        }
    }
    return 0;
}

void save(SDL_Renderer* renderer, int width, int height, const char* filename)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface)
    {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, filename);

    SDL_FreeSurface(surface);
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, 2, 2);


    for (double i = 0.0; i < 1.0; i += 0.001)
    {
        for (double j = 0.0; j < 1.0; j += 0.001)
        {
            double x = std::lerp(-2.0, 2.0, i);
            double y = std::lerp(-2.0, 2.0, j);

            int res = is_in_set(std::complex< double >(x, y));
            if (res == false)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawPointF(renderer, i * width / 2, j * height / 2);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 10 * res % 255, 10 * res % 255, 10 * res % 255, 255);
                SDL_RenderDrawPointF(renderer, i * width / 2, j * height / 2);
            }
        }
    }

    save(renderer, width, height, "fractal.bmp");

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
