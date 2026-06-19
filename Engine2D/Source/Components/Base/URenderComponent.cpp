#include "URenderComponent.h"

#include "../AActor.h"
#include "../../Core/UWorld.h"

void URenderComponent::SetVisibility(bool bNewVisibility)
{
	// Only react when the visibility actually changes, to avoid redundant notifications.
	if (bIsVisible != bNewVisibility)
	{
		bIsVisible = bNewVisibility;
		OnVisibilityChanged();
	}
}

void URenderComponent::Draw(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	// Invisible components are skipped entirely.
	if (!bIsVisible)
	{
		return;
	}

	// Compose world-to-view with local-to-world to get the full model-view transform.
	FMatrix3x3 FinalMatrix = InViewMatrix * GetComponentToWorld();

	sf::RenderStates States;
	States.transform = FinalMatrix.ToSFMLTransform();

	DrawShape(InWindow, States);
}

void URenderComponent::OnRegister()
{
	// Join the owning world's render list so this component is drawn each frame.
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
	// Leave the owning world's render list so it stops drawing this component.
	if (AActor* OwnerActor = GetOwner())
	{
		if (UWorld* OwningWorld = OwnerActor->GetWorld())
		{
			OwningWorld->RemoveRenderComponent(this);
		}
	}
}