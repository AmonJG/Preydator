#ifndef PREYDATOR_BARRIER_H__
#define PREYDATOR_BARRIER_H__

#include "entity.h"

class Barrier : public Entity
{
public:

    using Entity::Entity;
	void spawn() override;
	void action() override;
	void perceive(InputLayerValues perception) override;
	DrawInfo getDrawInfo() const override;
	double getPerceptionValue(EntityPtr entity) const override;
	bool attack(EntityPtr entity) override;
	EntityPtr giveBirth(Point birthLocation) override;
	std::string documentSelf() override;
	std::string createSaveString() override;

private:

};

namespace
{

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

#endif /* PREYDATOR_BARRIER_H__ */