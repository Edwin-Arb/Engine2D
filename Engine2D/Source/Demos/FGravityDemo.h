#pragma once
#include "IMathDemo.h"
#include "../Math/FVector2D.h"

class UPhysicsComponent;
class UCircleRenderComponent;
class AActor;

/**
 * Newtonian gravity as a vector field. A static Sun anchors the system while an Earth,
 * its Moon, and a Jupiter on a wider orbit move under their mutual gravity - a four-body
 * setup driven purely by force superposition (every body pulls on every other).
 * Masses and radii are arbitrary simulation units tuned for stable on-screen orbits.
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
	FVector2D ComputeGravitationalForce(const FVector2D& SourceLocation, float SourceMass,	//
										const FVector2D& TargetLocation, float TargetMass) const;

	/** Gravitational constant - a free tuning knob here. Scales orbital speed, not orbit shape. */
	const float G = 10.0f;

	// --- Sun (static central body, no physics) ---
	const float SunRadius = 40.0f;
	const float SunMass = 100000.0f;
	UCircleRenderComponent* SunComponent = nullptr;

	// --- Earth (orbits the Sun) ---
	const float EarthRadius = SunRadius * 0.2f;
	const float EarthMass = 5000.0f;
	UCircleRenderComponent* EarthComponent = nullptr;
	UPhysicsComponent* EarthPhysicsComponent = nullptr;

	// --- Moon (orbits the Earth; must stay well inside Earth's Hill sphere) ---
	const float MoonRadius = EarthRadius * 0.4f;
	const float MoonMass = 1.0f;
	UCircleRenderComponent* MoonComponent = nullptr;
	UPhysicsComponent* MoonPhysicsComponent = nullptr;

	// --- Jupiter (orbits the Sun, wider radius) ---
	const float JupiterRadius = SunRadius * 0.4f;
	const float JupiterMass = 1500.0f;
	UCircleRenderComponent* JupiterComponent = nullptr;
	UPhysicsComponent* JupiterPhysicsComponent = nullptr;

	/** Bodies spawned by this demo, tracked so Exit can tear them down. */
	std::vector<AActor*> DemoActors;
};
