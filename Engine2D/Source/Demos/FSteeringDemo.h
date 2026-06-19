#pragma once

#include "IMathDemo.h"

/**
 * Smoothly rotates the square to face the target each frame (steering toward a heading).
 */
class FSteeringDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Steering Demo | Smooth Rotation Towards Target"; }

private:
	/** Multiplier on the angle error: higher turns more aggressively toward the target. */
	const float TurnBoostFactor = 5.0f;

	/** Base rotation speed in degrees per second. */
	const float SquareRotationSpeed = 180.0f;
};