#include "USceneComponent.h"
#include <cmath>

#include "../../Core/Logging/FLog.h"

USceneComponent::~USceneComponent()
{
	// Detach from the parent so it no longer references this (now dying) component.
	if (AttachParent)
	{
		std::erase(AttachParent->AttachChildren, this);
	}

	// Orphan every child and invalidate its world transform, since its parent is gone.
	for (USceneComponent* Child : AttachChildren)
	{
		Child->AttachParent = nullptr;
		Child->MarkWorldTransformDirty();
	}
}

void USceneComponent::SetRelativeLocation(const FVector2D& NewLocation)
{
	RelativeTransform.SetPosition(NewLocation);

	// Any change to the relative transform invalidates the cached world matrix.
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

	// Convert the desired world location into the parent's local space, so the
	// resulting world position matches NewWorldLocation once the hierarchy is applied.
	if (AttachParent)
	{
		NewRelative = AttachParent->GetComponentToWorld().Inverse().TransformPoint(NewWorldLocation);
	}
	else
	{
		// No parent: relative space is world space.
		NewRelative = NewWorldLocation;
	}

	RelativeTransform.SetPosition(NewRelative);
	MarkWorldTransformDirty();
}

FVector2D USceneComponent::GetComponentLocation() const
{
	// The world location is the origin transformed by the local-to-world matrix.
	return GetComponentToWorld().TransformPoint({ 0.0f, 0.0f });
}

float USceneComponent::GetComponentRotation() const
{
	const FMatrix3x3 WorldMatrix = GetComponentToWorld();

	// Recover the rotation angle from the matrix's first basis vector (the X axis).
	const float M00 = WorldMatrix.Get(0, 0);
	const float M10 = WorldMatrix.Get(1, 0);

	const float AngleRadians = std::atan2(M10, M00);
	return FVector2D::RadianToDegree(AngleRadians);
}

FVector2D USceneComponent::GetComponentScale() const
{
	const FMatrix3x3 WorldMatrix = GetComponentToWorld();

	// World scale is the length of each basis vector (column) of the world matrix.
	const float ScaleX = std::sqrt(WorldMatrix.Get(0, 0) * WorldMatrix.Get(0, 0) +	//
								   WorldMatrix.Get(1, 0) * WorldMatrix.Get(1, 0));

	const float ScaleY = std::sqrt(WorldMatrix.Get(0, 1) * WorldMatrix.Get(0, 1) +	//
								   WorldMatrix.Get(1, 1) * WorldMatrix.Get(1, 1));

	return FVector2D(ScaleX, ScaleY);
}

const FMatrix3x3& USceneComponent::GetComponentToWorld() const
{
	// Rebuild the cached world matrix only when it has been marked dirty.
	if (bWorldTransformDirty)
	{
		if (AttachParent)
		{
			// World = ParentWorld * Local, so the hierarchy composes top-down.
			CachedWorldMatrix = AttachParent->GetComponentToWorld() * RelativeTransform.ToMatrix();
		}
		else
		{
			// A root component's world matrix is just its local transform.
			CachedWorldMatrix = RelativeTransform.ToMatrix();
		}

		bWorldTransformDirty = false;
	}

	return CachedWorldMatrix;
}

void USceneComponent::AttachToComponent(USceneComponent* NewParent)
{
	// Already attached to this parent - nothing to do.
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

	// Unlink from the previous parent before rebinding to the new one.
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
	// Skip work if already dirty: descendants were invalidated by the same earlier call.
	if (!bWorldTransformDirty)
	{
		bWorldTransformDirty = true;

		// A parent change cascades down: every child's world matrix is now stale too.
		for (const auto& Child : AttachChildren)
		{
			Child->MarkWorldTransformDirty();
		}
	}
}
