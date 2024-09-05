#ifndef PREYDATOR_MATH_H__
#define PREYDATOR_MATH_H__

#include <vector>
#include <string>

int randWithExponentialBias(int max);
std::vector<int> generateUniqueRandInts(int ammount, int limit);
int generateRandomInt(int min, int max);
double generateRandomDouble(double min, double max);
std::string getCurrentTimestamp();

#endif /* PREYDATOR_MATH_H__ */