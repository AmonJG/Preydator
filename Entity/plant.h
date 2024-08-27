#ifndef PREYDATOR_ENTITY_PLANT_H__
#define PREYDATOR_ENTITY_PLANT_H__

#include "entity.h"

class Plant : public Entity
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

} //unnamed namespace


#endif /* PREYDATOR_ENTITY_PLANT_H__ */