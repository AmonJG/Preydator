#include "graphics_handler.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

GraphicsHandler::GraphicsHandler(int w, int h)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create a window
    m_window = SDL_CreateWindow("Graphical Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    if (!m_window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Create a renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

}

GraphicsHandler::~GraphicsHandler()
{
    // Wait for a few seconds before exiting
    SDL_Delay(2000);

    // Clean up and quit SDL
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void GraphicsHandler::drawEntity(DrawInfo drawInfo, Point location)
{
    SDL_SetRenderDrawColor(m_renderer, drawInfo.r, drawInfo.g, drawInfo.b, drawInfo.a);
    for(auto point : drawInfo.points)
    {
        SDL_RenderDrawPoint(m_renderer, location.x + point.x, location.y + point.y);
    }
}

void GraphicsHandler::render()
{
    SDL_RenderPresent(m_renderer);
    SDL_Delay(20);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    SDL_RenderClear(m_renderer);
}

