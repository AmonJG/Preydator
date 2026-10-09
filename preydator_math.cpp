#include "preydator_math.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

std::random_device rd;
std::mt19937 generator(rd());

int outputValueToStepSize(double neural_network_output_value)
{
	return std::tanh(neural_network_output_value) * config.max_step_size;
}

double StepDistanceToInputValue(int total_step_distance)
{
	return total_step_distance / config.max_step_size;
}

// Gaussian noise for probabilistic events/decisions
double gaussianNoise(double mean, double stddev)
{
    std::normal_distribution<double> dist(mean, stddev);
    return dist(rd);
}

// Rectified Linear Unit (ReLU) activation function
double ReLU(double x)
{
    return std::max(0.0, x);
}

// Generates a vector of [ammount] unique ints between 0 and [limit - 1]
std::vector<int> generateUniqueRandInts(int ammount, int limit)
{
	std::vector<int> values;
	if (ammount > limit)
	{
		std::cerr << "Too many rand ints requested!" << std::endl;
		return values;
	}
	for (int i = 0; i < limit; i++)
	{
		values.push_back(i);
	}

	
    std::shuffle(values.begin(), values.end(), generator);

	std::vector<int> randomInts;
	for (int i = 0; i < ammount; i++)
	{
		randomInts.push_back(values[i]);
	}

	return randomInts;
}

int generateRandomInt(int min, int max)
{
	if (min == max) return min;
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

double generateRandomDouble(double min, double max)
{
	if (min == max) return min;
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

bool trueWithProb(double probability)
{
	return generateRandomDouble(0.0, 1.0) < probability;
}

int randWithExponentialBias(int min, int max)
{
	if (min == max) return min;
	if (min > max) return 0;
	int x = generateRandomInt(0, 99);
	if (x == 99 && max > 0) return max;
	if (x == 0 && min <= 0) return min;
	return std::round(std::exp(x * (std::log(max - min + 2) / 99)) + (min - 1) - 0.5);
}

std::string getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm* localTime = std::localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d_%H:%M:%S");
    return oss.str();
}

int mod(int a, int b) {
    return (a % b + b) % b;
}

int wrappedDelta(int current, int previous, int worldSize)
{
    int delta = current - previous;

    if (delta > worldSize / 2)
        delta -= worldSize;
    else if (delta < -worldSize / 2)
        delta += worldSize;

    return delta;
}