#include "AActor.h"

AActor::AActor(const std::string& InName)
{
	ActorName = InName;
}

void AActor::RegisterAllComponents()
{
	// Drive OnRegister on every owned component (e.g. render components join the world).
	for (const auto& Component : OwnedComponents)
	{
		if (Component)
		{
			Component->RegisterComponent();
		}
	}
}

void AActor::UnregisterAllComponents()
{
	// Reverse of RegisterAllComponents: detach every component from the world.
	for (const auto& Component : OwnedComponents)
	{
		if (Component)
		{
			Component->UnregisterComponent();
		}
	}
}

void AActor::Tick(float InDeltaTime)
{
	// Propagate the per-frame update down to every owned component.
	for (const auto& Component : OwnedComponents)
	{
		if (Component)
		{
			Component->TickComponent(InDeltaTime);
		}
	}
}

void AActor::BeginPlay()
{
	// Propagate the gameplay-start event down to every owned component.
	for (const auto& Component : OwnedComponents)
	{
		if (Component)
		{
			Component->BeginPlay();
		}
	}
}

void AActor::SetRootComponent(USceneComponent* InRootComponent)
{
	RootComponent = InRootComponent;
}

FVector2D AActor::GetActorLocation() const
{
	// The actor's location is defined by its root component; fall back to origin if none.
	return RootComponent ? RootComponent->GetComponentLocation() : FVector2D(0.0f, 0.0f);
}

void AActor::SetActorLocation(const FVector2D& NewLocation)
{
	// Moving the actor means moving its root component in world space.
	if (RootComponent)
	{
		RootComponent->SetWorldLocation(NewLocation);
	}
}
