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
    m_location.x = lastX;
    m_location.y = lastY;
}

void Barrier::action()
{
}

DrawInfo Barrier::getDrawInfo() const
{
    return barrierDrawInfo;
}