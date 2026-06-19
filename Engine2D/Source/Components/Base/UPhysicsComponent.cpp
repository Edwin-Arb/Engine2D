#include "UPhysicsComponent.h"

#include <algorithm>
#include "USceneComponent.h"
#include "../AActor.h"

UPhysicsComponent::UPhysicsComponent()
{
	// Physics needs a per-frame update, so opt this component into ticking.
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhysicsComponent::TickComponent(float InDeltaTime)
{
	UActorComponent::TickComponent(InDeltaTime);

	// Physics integration requires an owning actor with a movable root component.
	AActor* ActorOwner = GetOwner();
	if (!ActorOwner)
	{
		return;
	}

	USceneComponent* Root = ActorOwner->GetRootComponent();
	if (!Root)
	{
		return;
	}

	// A non-positive mass would make acceleration undefined (division by zero).
	if (Mass <= 0.0f)
	{
		return;
	}

	// 1. Semi-implicit Euler integration: derive acceleration from F = m * a.
	FVector2D Acceleration = AccumulatedForce / Mass;

	// Integrate acceleration into velocity over this frame's time step.
	Velocity += Acceleration * InDeltaTime;

	// 2. Apply linear damping (drag), scaled by the time step so it is frame-rate independent.
	Velocity *= std::pow(LinearDamping, InDeltaTime);

	// 3. Integrate velocity into the root component's world position.
	FVector2D CurrentPos = Root->GetComponentLocation();
	FVector2D NewPos = CurrentPos + Velocity * InDeltaTime;
	Root->SetWorldLocation(NewPos);

	// 4. Clear the accumulated force so it does not carry over into the next frame.
	AccumulatedForce = { 0.0f, 0.0f };
}

void UPhysicsComponent::AddForce(const FVector2D& InForce)
{
	AccumulatedForce += InForce;
}

void UPhysicsComponent::SetMass(float NewMass)
{
	Mass = NewMass > 0.0f ? NewMass : 0.0f;
}

void UPhysicsComponent::SetLinearDamping(float NewDamping)
{
	LinearDamping = std::clamp(NewDamping, EPSILON, 1.0f);
}