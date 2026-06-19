#pragma once
#include "IMathDemo.h"

class UPhysicsComponent;
class AActor;
class UCircleRenderComponent;

/**
 * Hooke's law spring: a fixed Anchor and a free Cargo joined by an ideal spring.
 * Demonstrates the restoring force F = -k * x (simple harmonic motion) plus an explicit
 * velocity-damping force so the oscillation settles. The Cargo can be grabbed and dragged
 * with the left mouse button; releasing it lets the spring snap it back.
 */
class FSpringDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Spring Demo | Hooke's Law | Grab the Cargo with the Mouse"; }

private:
	/** True while the user is holding the cargo with the left mouse button. */
	bool bIsGrabbed = false;

	/** Spring stiffness k. Higher = snappier, faster oscillation (and harder to integrate). */
	float Stiffness = 10000.0f;

	/** Natural length the spring restores toward (rest distance between anchor and cargo). */
	float RestLength = 30.0f;

	/** Velocity-damping coefficient c. Critical damping (zero overshoot) is c = 2 * sqrt(k * m). */
	float DampingCoefficient = 100.0f;

	/** Mass of the cargo body. */
	float CargoMass = 10.0f;

	// --- Anchor: static circle (no physics), the fixed end of the spring ---
	float AnchorRadius = 20.0f;
	UCircleRenderComponent* AnchorComponent = nullptr;

	// --- Cargo: circle with physics, the free end of the spring ---
	float CargoRadius = 40.0f;
	UCircleRenderComponent* CargoComponent = nullptr;
	UPhysicsComponent* CargoPhysicsComponent = nullptr;

	/** Bodies spawned by this demo, tracked so Exit can tear them down. */
	std::vector<AActor*> DemoActors;
};
