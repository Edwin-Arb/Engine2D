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

	void RegisterComponent();
	void UnregisterComponent();
	bool IsRegistered() const { return bRegistered; }

	virtual void BeginPlay() {}
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

	struct FComponentTickFunction
	{
		bool bCanEverTick = false;
	};

	FComponentTickFunction PrimaryComponentTick;

protected:
	virtual void OnRegister() {}
	virtual void OnUnregister() {}

	/** Pointer to the Owner Actor */
	AActor* Owner = nullptr;

private:
	bool bRegistered = false;
};