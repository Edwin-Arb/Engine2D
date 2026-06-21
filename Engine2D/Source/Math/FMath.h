#pragma once
#include <cstdint>
#include <random>

/**
 * Static library of general-purpose math utilities, in the spirit of Unreal's FMath.
 * Holds scalar helpers (interpolation, angle conversion) and random-number generation
 * that are not tied to any particular vector or matrix type. Vector-specific math lives
 * on FVector2D itself.
 */
class FMath
{
public:
	// ===== Scalar math =====

	/** Linearly interpolates between two values by an alpha factor (InT=0 -> InA, InT=1 -> InB). */
	static float Lerp(float InA, float InB, float InT);

	/** Converts an angle from degrees to radians. */
	static float DegreeToRadian(float InDegree);

	/** Converts an angle from radians to degrees. */
	static float RadianToDegree(float InRadian);

	// ===== Random =====

	/** Returns a random integer in the inclusive range [Min, Max]. */
	static int32_t RandRange(int32_t Min, int32_t Max);

	/** Returns a random float in the range [Min, Max]. */
	static float FRandRange(float Min, float Max);

	/** Returns a random float in the range [0, 1]. */
	static float FRand();

private:
	/** Shared Mersenne Twister generator, seeded once on first use. */
	static std::mt19937& GetGenerator();
};
