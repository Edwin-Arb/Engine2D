#pragma once
#include "IMathDemo.h"
#include "../Math/FVector2D.h"

class UPhysicsComponent;
class UCircleRenderComponent;
class AActor;

/**
 * Newtonian gravity as a vector field. A static Sun, an Earth orbiting it, and a Moon
 * orbiting the Earth - a three-body setup driven purely by force superposition.
 * Bodies are advanced by the semi-implicit Euler integrator inside UPhysicsComponent.
 */
class FGravityDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Gravity Demo | Newtonian N-Body | Orbits & Superposition"; }

private:
	/**
	 * Newton's law of gravitation as a vector: the force pulling the target body toward the source.
	 * The 1/r^2 falloff and the direction normalization are folded into a single 1/r^3 (since dir = Offset / r).
	 */
	FVector2D ComputeGravitationalForce(const FVector2D& SourceLocation, float SourceMass,	 //
										const FVector2D& TargetLocation, float TargetMass) const;

	/** Gravitational constant - a free tuning knob here. Scales orbital speed, not orbit shape. */
	const float G = 100.0f;

	// --- Sun (static central body, no physics) ---
	const float SunRadius = 70.0f;
	const float SunMass = 1000000.0f;
	UCircleRenderComponent* Sun = nullptr;

	// --- Earth (orbits the Sun) ---
	const float EarthRadius = SunRadius * 0.4f;
	const float EarthMass = SunMass * 0.1f;
	UCircleRenderComponent* Earth = nullptr;
	UPhysicsComponent* EarthPhysicsComponent = nullptr;

	// --- Moon (orbits the Earth; must sit well inside Earth's Hill sphere to stay bound) ---
	const float MoonRadius = EarthRadius * 0.4f;
	const float MoonMass = EarthMass * 0.2f;
	UCircleRenderComponent* Moon = nullptr;
	UPhysicsComponent* MoonPhysicsComponent = nullptr;

	/** Bodies spawned by this demo, tracked so Exit can tear them down. */
	std::vector<AActor*> DemoActors;
};
