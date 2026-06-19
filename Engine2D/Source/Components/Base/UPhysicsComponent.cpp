#include "UPhysicsComponent.h"

#include <algorithm>
#include "USceneComponent.h"
#include "../AActor.h"

UPhysicsComponent::UPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhysicsComponent::TickComponent(float InDeltaTime)
{
	UActorComponent::TickComponent(InDeltaTime);

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

	if (Mass <= 0.0f)
	{
		return;
	}

	// 1. Calculate semi-implicit Euler integration mechanics
	FVector2D Acceleration = AccumulatedForce / Mass;

	// Update velocity based on acceleration output over time sequence
	Velocity += Acceleration * InDeltaTime;

	// 2. Apply environmental atmospheric drag damping mechanics
	Velocity *= std::pow(LinearDamping, InDeltaTime);

	// 3. Translate spatial location updates directly into the NodeTransform layer
	FVector2D CurrentPos = Root->GetComponentLocation();
	FVector2D NewPos = CurrentPos + Velocity * InDeltaTime;
	Root->SetWorldLocation(NewPos);

	// 4. Reset force accumulator cache buffer for the subsequent frame graph
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