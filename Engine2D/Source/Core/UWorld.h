#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include <utility>

#include "../Components/AActor.h"

class URenderComponent;

/**
 * Owns the lifetime of every actor in a scene and the flat render registry.
 * Actors are created through SpawnActor so they receive a back-pointer to the world;
 * render components follow that link to register themselves into the draw list.
 */
class UWorld
{
public:
	/** Creates an actor of type T, takes ownership, and wires its World back-pointer. */
	template <typename T = AActor, typename... Args>  //
	T* SpawnActor(Args&&... InArgs)
	{
		static_assert(std::is_base_of_v<AActor, T>, "SpawnActor<T>: T must derive from AActor");

		auto NewActor = std::make_unique<T>(std::forward<Args>(InArgs)...);
		T* RawActorPtr = NewActor.get();

		RawActorPtr->SetWorld(this);
		Actors.push_back(std::move(NewActor));

		return RawActorPtr;
	}

	/** Unregisters an actor's components (pulling them out of the render list) and frees it. */
	void DestroyActor(AActor* ActorToDestroy);

	/** Registration phase: drives RegisterAllComponents on every actor currently in the world. */
	void RegisterAllActors();

	/** Gameplay-start phase: drives BeginPlay on every actor currently in the world. */
	void BeginPlay();

	/** Per-frame update: ticks every actor in the world. */
	void Tick(float InDeltaTime);

	/** Render-registry plumbing used by URenderComponent's register/unregister hooks. */
	void AddRenderComponent(URenderComponent* InComponent);
	void RemoveRenderComponent(URenderComponent* InComponent);
	const std::vector<URenderComponent*>& GetRenderComponents() const { return RenderComponents; }

private:
	/** Master ownership store for all actors living in this world. */
	std::vector<std::unique_ptr<AActor>> Actors;

	/** Flat draw list consumed by the render pass; populated automatically via OnRegister. */
	std::vector<URenderComponent*> RenderComponents;
};
