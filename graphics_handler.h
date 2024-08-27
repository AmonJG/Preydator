#ifndef PREYDATOR_GRAPHICS_HANDLER_H__
#define PREYDATOR_GRAPHICS_HANDLER_H__

#include "Entity/entity.h"
#include <SDL2/SDL.h>
#include <mutex>

class GraphicsHandler
{
public:
    GraphicsHandler(GraphicsHandler &other) = delete;
    void operator=(GraphicsHandler const&) = delete;
    static GraphicsHandler* GetInstance(int w, int h);
    void drawEntity(Entity const& entity);
    void render();

protected:
    GraphicsHandler(int w, int h);
    ~GraphicsHandler();

private:
    static GraphicsHandler* m_graphicsHandlerSingletonInstance;
    static std::mutex m_mutex;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

};

#endif /* PREYDATOR_GRAPHICS_HANDLER_H__ */