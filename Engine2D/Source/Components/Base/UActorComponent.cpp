#include "UActorComponent.h"

void UActorComponent::RegisterComponent()
{
	if (bRegistered)
	{
		return;
	}

	bRegistered = true;
	OnRegister();
}

void UActorComponent::UnregisterComponent()
{
	if (!bRegistered)
	{
		return;
	}

	OnUnregister();
	bRegistered = false;
}