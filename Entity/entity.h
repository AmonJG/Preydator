#ifndef PREYDATOR_ENTITY_H__
#define PREYDATOR_ENTITY_H__

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <atomic>
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
    Entity();
    Entity(Point location);
    virtual ~Entity() = default;
    void run(std::atomic<int>& stopFlag);
    void stop();
    int check();
    void setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2);
    void sendSignal(int signal);
    virtual DrawInfo getDrawInfo() const = 0;
    int getId() const;
    Point getLocation() const;

private:
    int m_id;
    Point m_location;
    int m_health;
    int m_reproduction;
    std::condition_variable* mp_cv;
    std::mutex* mp_mtx;
    bool* mp_haltAgents;
    bool* mp_haltAgents2;
    int m_signal;

};

#endif /* PREYDATOR_ENTITY_H__ */