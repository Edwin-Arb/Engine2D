#pragma once

#include "IMathDemo.h"

/**
 * Like the steering demo, but the rotation has angular momentum: the square builds up
 * angular velocity, overshoots, and settles via frame-rate independent damping.
 */
class FRotationInertiaDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Rotation Inertia Demo | Angular Velocity & Damping"; }

	/** Disabled: this demo only spins the square in place, so collision response would just get in the way. */
	bool UsesCollision() const override { return false; }

private:
	/** Per-second fraction of angular velocity retained (0.8 = loses 20% each reference frame). */
	const float Damping = 0.80f;

	/** Multiplier on the angle error driving the angular acceleration. */
	const float TurnBoostFactor = 2.0f;

	/** Base rotation speed in degrees per second. */
	const float SquareRotationSpeed = 180.0f;

	/** Current angular velocity, carried between frames to produce inertia. */
	float AngularVelocity = 0.0f;
};