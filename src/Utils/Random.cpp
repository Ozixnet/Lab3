#include "Utils/Random.h"
#include <random>
#include <ctime>

static std::mt19937 generator;

void Random::initialize() {
    generator.seed(static_cast<unsigned>(time(0)));
}

int Random::getRange(int max) {
    if (max <= 0) return 0;
    std::uniform_int_distribution<> distribution(0, max - 1);
    return distribution(generator);
}

int Random::getRange(int min, int max) {
    if (min > max) return min;
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(generator);
}

bool Random::chance(int percent) {
    if (percent <= 0) return false;
    if (percent >= 100) return true;
    return getRange(100) < percent;
}
