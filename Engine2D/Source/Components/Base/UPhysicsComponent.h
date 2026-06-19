#pragma once
#include "UActorComponent.h"
#include "../../Math/FVector2D.h"

class UPhysicsComponent : public UActorComponent
{
public:
	UPhysicsComponent();

	/** Integrates forces and velocity into the root component's position once per frame. */
	void TickComponent(float InDeltaTime) override;

	/** Accumulates a force vector to be applied on the next physics tick. */
	void AddForce(const FVector2D& InForce);

	/** Directly overrides the current velocity vector. */
	void SetVelocity(const FVector2D& NewVelocity) { Velocity = NewVelocity; }

	/** Returns the current velocity vector. */
	const FVector2D& GetVelocity() const { return Velocity; }

	/** Sets the mass (clamped to be non-negative). */
	void SetMass(float NewMass);

	/** Returns the current mass. */
	float GetMass() const { return Mass; }

	/** Sets the linear damping factor (clamped to the (0, 1] range). */
	void SetLinearDamping(float NewDamping);

private:
	/** Current velocity in world units per second. */
	FVector2D Velocity = { 0.0f, 0.0f };

	/** Forces accumulated this frame, consumed and cleared each tick. */
	FVector2D AccumulatedForce = { 0.0f, 0.0f };

	/** Mass of the parent object. Higher mass requires more force to accelerate. */
	float Mass = 1.0f;

	/** Linear drag friction. Value of 1.0 means no friction, 0.0 means instant stop. */
	float LinearDamping = 0.95f;
};
