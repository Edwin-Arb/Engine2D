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
	// Registration phase: let every actor's components hook into world systems.
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
	// Fire the gameplay-start event across all actors once the world is set up.
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
	// Advance the simulation one step by ticking every actor.
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
	// Called from URenderComponent::OnRegister to join the draw list.
	if (InComponent)
	{
		RenderComponents.push_back(InComponent);
	}
}

void UWorld::RemoveRenderComponent(URenderComponent* InComponent)
{
	// Called from URenderComponent::OnUnregister to leave the draw list.
	std::erase(RenderComponents, InComponent);
}
