#include "entity.h"
#include "preydator.h"
#include <SDL2/SDL.h>

int main(int argc, char* const* argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Graphical Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 999, 999, 0);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<Entity> entities;
    entities.push_back(Entity(predatorDrawInfo));

    Preydator preydator = Preydator(entities);
    //drawEntities();

    // Start initial Agent Threads
    // Main Loop
    //   Wait for all Agents Actions
    //   Delete and Create Agents and corresponding Threads
    //   Log Agent Informations
    //   Render from atomic 2D-Grid

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, 201, 200);
    SDL_RenderDrawPoint(renderer, 201, 201);
    SDL_RenderDrawPoint(renderer, 201, 202);
    SDL_RenderDrawPoint(renderer, 201, 203);
    SDL_RenderDrawPoint(renderer, 201, 204);
    SDL_RenderDrawPoint(renderer, 201, 205);

    SDL_RenderDrawPoint(renderer, 202, 201);
    SDL_RenderDrawPoint(renderer, 202, 202);
    SDL_RenderDrawPoint(renderer, 202, 203);
    SDL_RenderDrawPoint(renderer, 202, 205);
    SDL_RenderDrawPoint(renderer, 202, 206);
    SDL_RenderDrawPoint(renderer, 202, 207);

    SDL_RenderDrawPoint(renderer, 203, 202);
    SDL_RenderDrawPoint(renderer, 203, 203);
    SDL_RenderDrawPoint(renderer, 203, 205);
    SDL_RenderDrawPoint(renderer, 203, 206);
    SDL_RenderDrawPoint(renderer, 203, 207);
    SDL_RenderDrawPoint(renderer, 203, 208);

    SDL_RenderDrawPoint(renderer, 204, 202);
    SDL_RenderDrawPoint(renderer, 204, 203);
    SDL_RenderDrawPoint(renderer, 204, 204);
    SDL_RenderDrawPoint(renderer, 204, 205);
    SDL_RenderDrawPoint(renderer, 204, 206);
    SDL_RenderDrawPoint(renderer, 204, 208);

    SDL_RenderDrawPoint(renderer, 205, 202);
    SDL_RenderDrawPoint(renderer, 205, 203);
    SDL_RenderDrawPoint(renderer, 205, 205);
    SDL_RenderDrawPoint(renderer, 205, 206);
    SDL_RenderDrawPoint(renderer, 205, 207);
    SDL_RenderDrawPoint(renderer, 205, 208);

    SDL_RenderDrawPoint(renderer, 206, 201);
    SDL_RenderDrawPoint(renderer, 206, 202);
    SDL_RenderDrawPoint(renderer, 206, 203);
    SDL_RenderDrawPoint(renderer, 206, 205);
    SDL_RenderDrawPoint(renderer, 206, 206);
    SDL_RenderDrawPoint(renderer, 206, 207);

    SDL_RenderDrawPoint(renderer, 207, 200);
    SDL_RenderDrawPoint(renderer, 207, 201);
    SDL_RenderDrawPoint(renderer, 207, 202);
    SDL_RenderDrawPoint(renderer, 207, 203);
    SDL_RenderDrawPoint(renderer, 207, 204);
    SDL_RenderDrawPoint(renderer, 207, 205);

    SDL_RenderPresent(renderer);

    // Wait for a few seconds before exiting
    SDL_Delay(5000);

    // Clean up and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
