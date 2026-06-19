#pragma once

#include <vector>
#include <memory>
#include <string>
#include <SFML/Window/Event.hpp>

#include "../Components/Base/UActorComponent.h"
#include "../Components/Base/USceneComponent.h"

class UPhysicsComponent;
class UWorld;

class AActor
{
public:
	AActor(const std::string& InName = "Actor");

	/** Copy constructor - deleted to prevent unintended copying. */
	AActor(const AActor&) = delete;

	/** Copy assignment operator - deleted to prevent unintended copying. */
	AActor& operator=(const AActor&) = delete;

	/** Move constructor - deleted: an actor is an identity object referenced by raw
	 *  back-pointers (component Owner, AttachParent/Children). It must never relocate. */
	AActor(AActor&&) = delete;

	/** Move assignment operator - deleted for the same identity-stability reason. */
	AActor& operator=(AActor&&) = delete;

	virtual ~AActor() = default;

	/** Factory method for constructing and registering sub-components. */
	template <typename T, typename... Args>	 //
	T* CreateComponent(Args&&... InArgs)
	{
		// 1. Creating a component in dynamic memory
		auto NewComponent = std::make_unique<T>(std::forward<Args>(InArgs)...);
		T* RawComponentPtr = NewComponent.get();

		// 2. Set the pointer to THIS actor (this) in the Owner field of this component
		RawComponentPtr->SetOwner(this);

		// 3. We transfer ownership of the `unique_ptr` to our internal lifecycle vector
		OwnedComponents.push_back(std::move(NewComponent));

		// 4. If this is a scene component and we don't have a root yet, make it the root
		if constexpr (std::is_base_of_v<USceneComponent, T>)
		{
			if (!RootComponent)
			{
				SetRootComponent(RawComponentPtr);
			}
		}

		// 5. Run the component's own initialization hook - it now exists and knows its Owner
		RawComponentPtr->InitializeComponent();

		return RawComponentPtr;
	}

	/** Dynamic search for a component within an Actor based on its type. */
	template <typename T>  //
	T* FindComponentByClass() const
	{
		for (const auto& Component : OwnedComponents)
		{
			if (T* CastedComponent = dynamic_cast<T*>(Component.get()))
			{
				return CastedComponent;
			}
		}
		return nullptr;
	}

	/** Walks every owned component and registers it with the world (drives OnRegister). */
	void RegisterAllComponents();

	/** Walks every owned component and unregisters it from the world (drives OnUnregister). */
	void UnregisterAllComponents();

	/** Core simulation frame tick propagated down to all registered components. */
	virtual void Tick(float InDeltaTime);

	/** Propagates the gameplay-start event down to all owned components.
	 *  Overrides must call AActor::BeginPlay() to keep components driven. */
	virtual void BeginPlay();

	/** Sets the primary transformation spatial node for this actor. */
	void SetRootComponent(USceneComponent* InRootComponent);

	/** Returns the root transformation node if it exists. */
	USceneComponent* GetRootComponent() const { return RootComponent; }

	/** Helper accessor to get world position directly from Root Component. */
	FVector2D GetActorLocation() const;

	/** Helper accessor to set world position directly onto Root Component. */
	void SetActorLocation(const FVector2D& NewLocation);

	const std::string& GetName() const { return ActorName; }

	/** Binds this actor to the world that owns it (set by UWorld::SpawnActor). */
	void SetWorld(UWorld* InWorld) { World = InWorld; }

	/** Returns the world this actor lives in, or nullptr if it has not been spawned through one. */
	UWorld* GetWorld() const { return World; }

private:
	std::string ActorName;

	/** Flattened array managing the structural lifetime allocation of sub-components. */
	std::vector<std::unique_ptr<UActorComponent>> OwnedComponents;

	/** Primary spatial anchor reference evaluating world matrix positions. */
	USceneComponent* RootComponent = nullptr;

	/** Non-owning back-pointer to the world that spawned this actor. */
	UWorld* World = nullptr;
};
