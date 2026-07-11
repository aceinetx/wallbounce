#include "wallbounce.hh"

using namespace wb;

void Wallbounce::RandSeed(uint64_t seed) {
	randSeed = seed;
	std::srand(seed);
}

uint64_t Wallbounce::Rand() {
	return std::rand();
}
