#pragma once

#include "IMathDemo.h"

/**
 * Simulates structural physical rotation mechanics by injecting mass, torque, and frame-rate independent damping.
 */
class FRotationInertiaDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Rotation Inertia Demo | Angular Velocity & Damping"; }

private:
	const float Damping = 0.80f;
	const float TurnBoostFactor = 2.0f;
	const float SquareRotationSpeed = 180.0f;

	float AngularVelocity = 0.0f;
};