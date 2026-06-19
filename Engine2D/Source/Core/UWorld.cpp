#include "UWorld.h"

#include <algorithm>

#include "../Components/Base/URenderComponent.h"

void UWorld::DestroyActor(AActor* ActorToDestroy)
{
	if (!ActorToDestroy)
	{
		return;
	}

	// Unregister first so each render component removes itself from RenderComponents via OnUnregister.
	ActorToDestroy->UnregisterAllComponents();

	std::erase_if(Actors,									  //
				  [ActorToDestroy](const std::unique_ptr<AActor>& Ptr)  //
				  {
					  return Ptr.get() == ActorToDestroy;	  //
				  });
}

void UWorld::RegisterAllActors()
{
	for (const auto& Actor : Actors)
	{
		if (Actor)
		{
			Actor->RegisterAllComponents();
		}
	}
}

void UWorld::BeginPlay()
{
	for (const auto& Actor : Actors)
	{
		if (Actor)
		{
			Actor->BeginPlay();
		}
	}
}

void UWorld::Tick(float InDeltaTime)
{
	for (const auto& Actor : Actors)
	{
		if (Actor)
		{
			Actor->Tick(InDeltaTime);
		}
	}
}

void UWorld::AddRenderComponent(URenderComponent* InComponent)
{
	if (InComponent)
	{
		RenderComponents.push_back(InComponent);
	}
}

void UWorld::RemoveRenderComponent(URenderComponent* InComponent)
{
	std::erase(RenderComponents, InComponent);
}
