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

int getEucldeanDistance(int x, int y, int prev_x, int prev_y)
{
	return static_cast<int>(std::sqrt(
		(x - prev_x) * (x - prev_x) +
		(y - prev_y) * (y - prev_y)
	));
}

// Gaussian noise for probabilistic events/decisions
double gaussianNoise(double mean, double stddev)
{
    static std::normal_distribution<double> dist(mean, stddev);
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

// expondential function normalized between min and max
// e^(x * (ln(max - min + 2) / 99)) + (min - 1) - 0.5
// die funktion nimmt ein prob. Wert von [0-99] und normalisiert
// die exp fkt s.d. x=0 genau min und x=99 genau max ist
//   -> kleinere Werte sind expondentiell wahrscheinlicher
// Um Rundungsfehler auszugleich wird das intervall der exp fkt um
// 1 erhöht und um 0.5 nach unten gesetzt also ist das inervall vorm
// runden: [min - 0.5, max + 0.5]. Da max + 0.5 auf max + 1 gerundet
// werden würde ist explizit definiert dass max returned wird wenn x=99 ist
// return interval = [min, max]

/*
	if (min > max) return 0;
	int x = std::rand() % 100;
	if (x == 99) return max;
	return std::round(std::exp(x * (std::log(max - min + 2) / 99)) + (min - 1) - 0.5);
*/

// TODO: fix broken function: wrong bias
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