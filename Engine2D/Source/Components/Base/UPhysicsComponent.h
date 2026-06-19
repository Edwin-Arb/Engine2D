#pragma once
#include "UActorComponent.h"
#include "../../Math/FVector2D.h"

class UPhysicsComponent : public UActorComponent
{
public:
	UPhysicsComponent();

	/** Core physics execution loop evaluated once per frame. */
	void TickComponent(float InDeltaTime) override;

	/** Adds a directional force vector onto the accumulated frame buffer. */
	void AddForce(const FVector2D& InForce);

	/** Instantly overrides the structural movement velocity vector. */
	void SetVelocity(const FVector2D& NewVelocity) { Velocity = NewVelocity; }
	const FVector2D& GetVelocity() const { return Velocity; }

	void SetMass(float NewMass);
	float GetMass() const { return Mass; }
	void SetLinearDamping(float NewDamping);

private:
	FVector2D Velocity = { 0.0f, 0.0f };
	FVector2D AccumulatedForce = { 0.0f, 0.0f };

	/** Mass of the parent object. Higher mass requires more force to accelerate. */
	float Mass = 1.0f;

	/** Linear drag friction. Value of 1.0 means no friction, 0.0 means instant stop. */
	float LinearDamping = 0.95f;
};
