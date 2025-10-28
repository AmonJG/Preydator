#ifndef PREYDATOR_ENTITY_PREY_H__
#define PREYDATOR_ENTITY_PREY_H__

#include "entity.h"

class Prey : public Entity
{
public:

    using Entity::Entity;
	void spawn() override;
	void action() override;
	void perceive(InputLayerValues perception) override;
	DrawInfo getDrawInfo() const override;
	double getPerceptionValue() const override;
	bool attack(std::shared_ptr<Entity> entity) override;
	std::shared_ptr<Entity> giveBirth(Point birthLocation) override;
	void documentSelf() override;
	std::string createSaveString() override;

private:

};

namespace
{

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

} //unnamed namespace

#endif /* PREYDATOR_ENTITY_PREY_H__ */