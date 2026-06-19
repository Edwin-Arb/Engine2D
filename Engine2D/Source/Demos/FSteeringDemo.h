#pragma once

#include "IMathDemo.h"

/**
 * Calculates continuous rotational correction forces to guide an object's orientation toward a target.
 */
class FSteeringDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Steering Demo | Smooth Rotation Towards Target"; }

private:
	const float TurnBoostFactor = 5.0f;
	const float SquareRotationSpeed = 180.0f;
};