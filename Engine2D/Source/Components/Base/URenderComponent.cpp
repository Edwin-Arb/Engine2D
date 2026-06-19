#include "URenderComponent.h"

#include "../AActor.h"
#include "../../Core/UWorld.h"

void URenderComponent::SetVisibility(bool bNewVisibility)
{
	if (bIsVisible != bNewVisibility)
	{
		bIsVisible = bNewVisibility;
		OnVisibilityChanged();
	}
}

void URenderComponent::Draw(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	if (!bIsVisible)
	{
		return;
	}

	// Concat the camera view matrix with the component's internal transform
	FMatrix3x3 FinalMatrix = InViewMatrix * GetComponentToWorld();

	sf::RenderStates States;
	States.transform = FinalMatrix.ToSFMLTransform();

	DrawShape(InWindow, States);
}

void URenderComponent::OnRegister()
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (UWorld* OwningWorld = OwnerActor->GetWorld())
		{
			OwningWorld->AddRenderComponent(this);
		}
	}
}

void URenderComponent::OnUnregister()
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (UWorld* OwningWorld = OwnerActor->GetWorld())
		{
			OwningWorld->RemoveRenderComponent(this);
		}
	}
}