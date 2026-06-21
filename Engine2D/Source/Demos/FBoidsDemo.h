#pragma once
#include <cstdint>
#include <vector>
#include "IMathDemo.h"

class UCircleRenderComponent;
class UPhysicsComponent;
class AActor;

/**
 * Reynolds boids: emergent flocking from three simple local steering rules -
 *   - Cohesion:   steer toward the average POSITION of nearby flockmates.
 *   - Alignment:  steer to match the average VELOCITY (heading) of nearby flockmates.
 *   - Separation: steer AWAY from flockmates that are too close.
 * Each rule produces a vector; their weighted sum is applied as a steering force.
 */
class FBoidsDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Boids Demo | Flocking | Cohesion + Alignment + Separation"; }

private:
	const int32_t BoidCount = 100;
	const float BoidMaxSpeed = 120.0f;
	const float BoidRadius = 8.0f;

	// Each rule has its own neighbourhood radius and weight - these are tuning knobs.
	// The weights must be on the order of BoidMaxSpeed, otherwise the steering force is
	// too weak to actually turn a boid that is already moving at top speed.
	const float CohesionRadius = 150.0f;
	const float CohesionSpeed = 200.0f;

	const float AlignmentRadius = 100.0f;
	const float AlignmentSpeed = 50.0f;

	const float SeparationRadius = 40.0f;
	const float SeparationSpeed = 450.0f;

	/** Cached physics components, parallel to DemoActors (same index = same boid). */
	std::vector<UPhysicsComponent*> BoidPhysicsComponents;

	/** Bodies spawned by this demo, tracked so Exit can tear them down. */
	std::vector<AActor*> DemoActors;
};
