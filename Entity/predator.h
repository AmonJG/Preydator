#ifndef PREYDATOR_ENTITY_PREDATOR_H__
#define PREYDATOR_ENTITY_PREDATOR_H__

#include "entity.h"

class Predator : public Entity
{
public:

    using Entity::Entity;
	void spawn() override;
	void action() override;
	DrawInfo getDrawInfo() const override;

private:

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

} //unnamed namespace

#endif /* PREYDATOR_ENTITY_PREDATOR_H__ */