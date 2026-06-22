#include "FMath.h"

#include <SFML/Graphics/Color.hpp>

#include "../Core/Constants.h"

float FMath::Lerp(float InA, float InB, float InT)
{
	return InA + (InB - InA) * InT;
}

float FMath::DegreeToRadian(float InDegree)
{
	return InDegree * PI / 180.0f;
}

float FMath::RadianToDegree(float InRadian)
{
	return InRadian * 180.0f / PI;
}

float FMath::FRand()
{
	// Distribution is reused across calls; only the generator carries state.
	static std::uniform_real_distribution<float> Dist(0.f, 1.f);
	return Dist(GetGenerator());
}

sf::Color FMath::RandomColor()
{
	uint8_t R = static_cast<uint8_t>(RandRange(0, 255));
	uint8_t G = static_cast<uint8_t>(RandRange(0, 255));
	uint8_t B = static_cast<uint8_t>(RandRange(0, 255));

	return sf::Color(R, G, B);
}

float FMath::FRandRange(float Min, float Max)
{
	std::uniform_real_distribution<float> Dist(Min, Max);
	return Dist(GetGenerator());
}

int32_t FMath::RandRange(int32_t Min, int32_t Max)
{
	std::uniform_int_distribution<int32_t> Dist(Min, Max);
	return Dist(GetGenerator());
}

std::mt19937& FMath::GetGenerator()
{
	// Single generator for the whole process, seeded once from a non-deterministic source.
	static std::mt19937 Generator(std::random_device{}());
	return Generator;
}
