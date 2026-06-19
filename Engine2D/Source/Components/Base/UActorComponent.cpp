#include "UActorComponent.h"

void UActorComponent::RegisterComponent()
{
	// Guard against double registration: OnRegister must fire exactly once.
	if (bRegistered)
	{
		return;
	}

	bRegistered = true;
	OnRegister();
}

void UActorComponent::UnregisterComponent()
{
	// Only unregister something that is currently registered.
	if (!bRegistered)
	{
		return;
	}

	OnUnregister();
	bRegistered = false;
}
