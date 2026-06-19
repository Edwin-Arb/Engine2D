#include "USceneComponent.h"
#include <cmath>

#include "../../Core/Logging/FLog.h"

USceneComponent::~USceneComponent()
{
	if (AttachParent)
	{
		std::erase(AttachParent->AttachChildren, this);
	}

	for (USceneComponent* Child : AttachChildren)
	{
		Child->AttachParent = nullptr;
		Child->MarkWorldTransformDirty();
	}
}

void USceneComponent::SetRelativeLocation(const FVector2D& NewLocation)
{
	RelativeTransform.SetPosition(NewLocation);
	MarkWorldTransformDirty();
}

void USceneComponent::SetRelativeRotation(float NewRotation)
{
	RelativeTransform.SetRotation(NewRotation);
	MarkWorldTransformDirty();
}

void USceneComponent::SetRelativeScale(const FVector2D& NewScale)
{
	RelativeTransform.SetScale(NewScale);
	MarkWorldTransformDirty();
}

FVector2D USceneComponent::GetRelativeLocation() const
{
	return RelativeTransform.GetRelativePosition();
}

float USceneComponent::GetRelativeRotation() const
{
	return RelativeTransform.GetRelativeRotation();
}

FVector2D USceneComponent::GetRelativeScale() const
{
	return RelativeTransform.GetRelativeScale();
}

void USceneComponent::SetWorldLocation(const FVector2D& NewWorldLocation)
{
	FVector2D NewRelative;

	if (AttachParent)
	{
		NewRelative = AttachParent->GetComponentToWorld().Inverse().TransformPoint(NewWorldLocation);
	}
	else
	{
		NewRelative = NewWorldLocation;
	}

	RelativeTransform.SetPosition(NewRelative);
	MarkWorldTransformDirty();
}

FVector2D USceneComponent::GetComponentLocation() const
{
	return GetComponentToWorld().TransformPoint({ 0.0f, 0.0f });
}

float USceneComponent::GetComponentRotation() const
{
	const FMatrix3x3 WorldMatrix = GetComponentToWorld();

	// Extract components from the basis vector tracking X direction tracking scale
	const float M00 = WorldMatrix.Get(0, 0);
	const float M10 = WorldMatrix.Get(1, 0);

	const float AngleRadians = std::atan2(M10, M00);
	return FVector2D::RadianToDegree(AngleRadians);
}

FVector2D USceneComponent::GetComponentScale() const
{
	const FMatrix3x3 WorldMatrix = GetComponentToWorld();

	// Compute Euclidean magnitude lengths across explicit column vectors inside the matrix state
	const float ScaleX = std::sqrt(WorldMatrix.Get(0, 0) * WorldMatrix.Get(0, 0) +	//
								   WorldMatrix.Get(1, 0) * WorldMatrix.Get(1, 0));

	const float ScaleY = std::sqrt(WorldMatrix.Get(0, 1) * WorldMatrix.Get(0, 1) +	//
								   WorldMatrix.Get(1, 1) * WorldMatrix.Get(1, 1));

	return FVector2D(ScaleX, ScaleY);
}

const FMatrix3x3& USceneComponent::GetComponentToWorld() const
{
	if (bWorldTransformDirty)
	{
		if (AttachParent)
		{
			CachedWorldMatrix = AttachParent->GetComponentToWorld() * RelativeTransform.ToMatrix();
		}
		else
		{
			CachedWorldMatrix = RelativeTransform.ToMatrix();
		}

		bWorldTransformDirty = false;
	}

	return CachedWorldMatrix;
}

void USceneComponent::AttachToComponent(USceneComponent* NewParent)
{
	if (AttachParent == NewParent)
	{
		return;
	}

	if (NewParent == this)
	{
		UE_LOG(LogEngine, Warning, "Cannot attach component to itself");
		return;
	}

	// Walk up the prospective parent chain. Attaching to one of our own
	// descendants would form a cycle and break world-transform propagation.
	for (USceneComponent* Ancestor = NewParent; Ancestor; Ancestor = Ancestor->AttachParent)
	{
		if (Ancestor == this)
		{
			UE_LOG(LogEngine, Error,
				   "Cyclic dependency detected. "
				   "A component cannot be attached to one of its own descendants.");

			return;
		}
	}

	if (AttachParent)
	{
		std::erase(AttachParent->AttachChildren, this);
	}

	AttachParent = NewParent;

	if (AttachParent)
	{
		// Register *this* component as a child of its new parent (not the parent
		// itself - that would corrupt the hierarchy and create a self-reference).
		AttachParent->AttachChildren.push_back(this);
	}

	MarkWorldTransformDirty();
}

void USceneComponent::MarkWorldTransformDirty()
{
	if (!bWorldTransformDirty)
	{
		bWorldTransformDirty = true;

		for (const auto& Child : AttachChildren)
		{
			Child->MarkWorldTransformDirty();
		}
	}
}