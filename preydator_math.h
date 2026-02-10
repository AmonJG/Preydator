#ifndef PREYDATOR_MATH_H__
#define PREYDATOR_MATH_H__

#include "preydator_config.h"
#include <vector>
#include <string>

int outputValueToStepSize(double neural_network_output_value);
double StepDistanceToInputValue(int total_step_distance);
int getEucldeanDistance(int x, int y, int prev_x, int prev_y);
double gaussianNoise(double mean, double stddev);
std::vector<int> generateUniqueRandInts(int ammount, int limit);
int generateRandomInt(int min, int max);
double generateRandomDouble(double min, double max);
bool trueWithProb(double probability);
int randWithExponentialBias(int min, int max);
std::string getCurrentTimestamp();

#endif /* PREYDATOR_MATH_H__ */