#include "preydator_math.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

std::random_device rd;
std::mt19937 generator(rd());

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
int randWithExponentialBias(int min, int max)
{
	if (min > max) return 0;
	int x = std::rand() % 100;
	if (x == 99 && max > 0) return max;
	if (x == 0 && min <= 0) return min;
	return std::round(std::exp(x * (std::log(max - min + 2) / 99)) + (min - 1) - 0.5);
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

std::string getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm* localTime = std::localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d_%H:%M:%S");
    return oss.str();
}