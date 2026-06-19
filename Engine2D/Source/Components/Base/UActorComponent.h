#pragma once

class AActor;

class UActorComponent
{
public:
	UActorComponent() = default;

	/** Copy constructor - deleted to prevent unintended copying. */
	UActorComponent(const UActorComponent&) = delete;

	/** Copy assignment operator - deleted to prevent unintended copying. */
	UActorComponent& operator=(const UActorComponent&) = delete;

	/** Virtual destructor to ensure correct cleanup of derived components. */
	virtual ~UActorComponent() = default;

	/** Called immediately after the component is successfully created within the Actor. */
	virtual void InitializeComponent() {}

	/** Called before the component is destroyed. */
	virtual void UninitializeComponent() {}

	/** Registers the component, firing OnRegister once (idempotent). */
	void RegisterComponent();

	/** Unregisters the component, firing OnUnregister once (idempotent). */
	void UnregisterComponent();

	/** Returns true if the component is currently registered with the world. */
	bool IsRegistered() const { return bRegistered; }

	/** Called once when gameplay starts for this component. */
	virtual void BeginPlay() {}

	/** Called once when gameplay ends for this component. */
	virtual void EndPlay() {}

	/**
	 * Main update tick for the component.
	 * @param InDeltaTime Time elapsed since the last frame in seconds.
	 */
	virtual void TickComponent(float InDeltaTime) {}

	/** Sets the owner of this component (called automatically upon creation) */
	void SetOwner(AActor* InOwner) { Owner = InOwner; }

	/** Returns a pointer to the Actor to which this component belongs */
	AActor* GetOwner() const { return Owner; }

	/** Tick configuration for this component (mirrors Unreal's tick function struct). */
	struct FComponentTickFunction
	{
		/** When false, TickComponent is never called for this component. */
		bool bCanEverTick = false;
	};

	/** Primary tick settings controlling whether this component ticks each frame. */
	FComponentTickFunction PrimaryComponentTick;

protected:
	/** Hook called when the component is registered (override to join world systems). */
	virtual void OnRegister() {}

	/** Hook called when the component is unregistered (override to leave world systems). */
	virtual void OnUnregister() {}

	/** Pointer to the Owner Actor */
	AActor* Owner = nullptr;

private:
	bool bRegistered = false;
};