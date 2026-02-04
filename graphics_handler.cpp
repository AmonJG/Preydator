#include "Entity/entity.h"
#include "graphics_handler.h"
#include <SDL2/SDL.h>
#include <mutex>
#include <stdlib.h>

GraphicsHandler* GraphicsHandler::m_graphicsHandlerSingletonInstance = nullptr;
std::mutex GraphicsHandler::m_mutex;

GraphicsHandler* GraphicsHandler::GetInstance(int w, int h)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_graphicsHandlerSingletonInstance == nullptr)
    {
        m_graphicsHandlerSingletonInstance = new GraphicsHandler(w, h);
    }
    return m_graphicsHandlerSingletonInstance;
}

GraphicsHandler::GraphicsHandler(int w, int h)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create a window
    m_window = SDL_CreateWindow("Graphical Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    if (!m_window)
	{
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Create a renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer)
	{
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

void GraphicsHandler::drawEntity(Entity const& entity) const
{
    DrawInfo drawInfo = entity.getDrawInfo();
    Point location = entity.getLocation();
    SDL_SetRenderDrawColor(m_renderer, drawInfo.color.r, drawInfo.color.g, drawInfo.color.b, drawInfo.color.a);
    for(auto point : drawInfo.points)
    {
        SDL_RenderDrawPoint(m_renderer, location.x + point.x, location.y + point.y);
    }
}

void GraphicsHandler::drawPoints(std::vector<SDL_Point> const& points, Color color) const
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoints(m_renderer, points.data(), points.size());
}

void GraphicsHandler::render() const
{
	SDL_RenderPresent(m_renderer);
	SDL_Delay(1);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);
}

