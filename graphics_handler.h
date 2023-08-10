#pragma once
#include "entity.h"
#include <SDL2/SDL.h>

class GraphicsHandler
{
public:
    GraphicsHandler(int w, int h);
    ~GraphicsHandler();
    void drawEntity(DrawInfo drawInfo, Point location);
    void render();

private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

};

