#pragma once
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <vector>

struct Point
{
    int x, y;
};

struct DrawInfo
{
    std::vector<Point> points;
    uint8_t r, g, b, a;
};

class Entity
{
public:
    Entity(DrawInfo const& drawInfo);
    Entity(DrawInfo const& drawInfo, Point location);
    ~Entity();
    void run();
    void setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2);
    DrawInfo getDrawInfo() const;
    Point getLocation() const;

private:
    const DrawInfo m_drawInfo;
    Point m_location;
    std::condition_variable* mp_cv;
    std::mutex* mp_mtx;
    bool* mp_haltAgents;
    bool* mp_haltAgents2;

};

namespace
{

DrawInfo const predatorDrawInfo =
{
    {
               {1,0},                                    {7,0},
               {1,1}, {2,1},                      {6,1}, {7,1},
               {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2},
               {1,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3},
               {1,4},               {4,4},               {7,4},
               {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5},
                      {2,6}, {3,6}, {4,6}, {5,6}, {6,6},
                      {2,7}, {3,7},        {5,7}, {6,7},
                             {3,8}, {4,8}, {5,8}
    },
    255, 0, 0, 255
};

DrawInfo const preyDrawInfo =
{
    {
                      {2,0},                      {6,0},
                      {2,1},                      {6,1},
                      {2,2}, {3,2},        {5,2}, {6,2},
                      {2,3}, {3,3}, {4,3}, {5,3}, {6,3},
                      {2,4},        {4,4},        {6,4},
                      {2,5}, {3,5}, {4,5}, {5,5}, {6,5},
                      {2,6}, {3,6}, {4,6}, {5,6}, {6,6},
                             {3,7}, {4,7}, {5,7},
                                    {4,8}
    },
    255, 128, 0, 255
};

DrawInfo const plantDrawInfo =
{
    {
                      {2,0},                      {6,0},
                      {2,1}, {3,1},        {5,1}, {6,1},
        {0,2}, {1,2},        {3,2}, {4,2}, {5,2},        {7,2}, {8,2},
               {1,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3},
                      {2,4}, {3,4},        {5,4}, {6,4},
               {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5},
        {0,6}, {1,6},        {3,6}, {4,6}, {5,6},        {7,6}, {8,6},
                      {2,7}, {3,7},        {5,7}, {6,7},
                      {2,8},                      {6,8}

    },
    0, 128, 0, 255
};

DrawInfo const barrierDrawInfo =
{
    {
                             {3,0}, {4,0}, {5,0}, {6,0},
               {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1},
        {0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2},
        {0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3}, {8,3},
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4},
        {0,5}, {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5},
               {1,6}, {2,6}, {3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6},
               {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7},
                      {2,8}, {3,8}, {4,8}, {5,8}

    },
    128, 128, 128, 255
};

} //unnamed namespace

