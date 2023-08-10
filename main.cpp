#include "entity.h"
#include "graphics_handler.h"
#include "preydator.h"

int main(int argc, char* const* argv) {
    
    GraphicsHandler graphicsHandler = GraphicsHandler(999, 999);

    std::vector<Entity> entities;
    entities.push_back(Entity(predatorDrawInfo));

    Preydator preydator = Preydator(entities);

    for(int i = 0; i < 720; i = i+1)
    {
        graphicsHandler.drawEntity(predatorDrawInfo, {100+i, 100+i});
        graphicsHandler.drawEntity(preyDrawInfo, {200, 100+i});
        graphicsHandler.drawEntity(plantDrawInfo, {100+i, 200});
        graphicsHandler.drawEntity(barrierDrawInfo, {200, 200});
        graphicsHandler.render();
    }

    // Start initial Agent Threads
    // Main Loop
    //   Wait for all Agents Actions
    //   Delete and Create Agents and corresponding Threads
    //   Log Agent Informations
    //   Render from atomic 2D-Grid

    return 0;
}
