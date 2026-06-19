#include "AActor.h"

AActor::AActor(const std::string& InName)
{
	ActorName = InName;
}

void AActor::RegisterAllComponents()
{
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
	// If the actor has a physical body (root), we take the position from there
	return RootComponent ? RootComponent->GetComponentLocation() : FVector2D(0.0f, 0.0f);
}

void AActor::SetActorLocation(const FVector2D& NewLocation)
{
	if (RootComponent)
	{
		RootComponent->SetWorldLocation(NewLocation);
	}
}