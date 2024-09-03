#include "barrier.h"

int lastX = 0;
int lastY = 0;
int dirX = 1;
int dirY = 1;

void Barrier::spawn()
{
	if (!lastX || !lastY)
	{
    	lastX = std::rand() % (WORLD_X - 9);
    	lastY = std::rand() % (WORLD_Y - 9);
	}
	if ((std::rand() % 10) < 1)
	{
		lastX = std::rand() % (WORLD_X - 9);
		lastY = std::rand() % (WORLD_Y - 9);
		dirX = (std::rand() % 2) ? 1 : -1;
		dirY = (std::rand() % 2) ? 1 : -1;
	}
	else
	{
		lastX += (std::rand() % 9) * dirX;
		lastY += (std::rand() % 9) * dirY;
	}
	if (lastX >= (WORLD_X - 9)) lastX = WORLD_X - 10;
	if (lastX < 0) lastX = 0;
    if (lastY >= (WORLD_Y - 9)) lastY = WORLD_Y - 10;
    if (lastY < 0) lastY = 0;

	m_location.x = lastX;
	m_location.y = lastY;
}

void Barrier::action()
{
}

void Barrier::perceive(InputLayerValues perception)
{
}

DrawInfo Barrier::getDrawInfo() const
{
    return barrierDrawInfo;
}

double Barrier::getPerceptionValue() const
{
	return BARRIER_INPUT_LAYER_VALUE;
}

bool Barrier::attack(std::shared_ptr<Entity> entity)
{
    return false;
}

std::shared_ptr<Entity> Barrier::giveBirth(Point birthLocation)
{
	return nullptr;
}