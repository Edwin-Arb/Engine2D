#pragma once

#include "UActorComponent.h"
#include <vector>
#include "../../Math/FTransform2D.h"

/**
 * A component that has a spatial transform (position, rotation, scale) in the 2D world.
 * Scene components can be attached to one another to form a hierarchy, where each child's
 * world transform is derived from its parent's.
 */
class USceneComponent : public UActorComponent
{
public:
	/** Detaches from the parent and orphans any children before destruction. */
	~USceneComponent() override;

	/** Sets the location relative to the attach parent (or world, if unattached). */
	void SetRelativeLocation(const FVector2D& NewLocation);

	/** Sets the rotation (in degrees) relative to the attach parent. */
	void SetRelativeRotation(float NewRotation);

	/** Sets the scale relative to the attach parent. */
	void SetRelativeScale(const FVector2D& NewScale);

	/** Returns the location relative to the attach parent. */
	FVector2D GetRelativeLocation() const;

	/** Returns the rotation (in degrees) relative to the attach parent. */
	float GetRelativeRotation() const;

	/** Returns the scale relative to the attach parent. */
	FVector2D GetRelativeScale() const;

	/** Sets an absolute world location, converting it into parent-relative space. */
	void SetWorldLocation(const FVector2D& NewWorldLocation);

	/** Returns the absolute world location extracted from the world matrix. */
	FVector2D GetComponentLocation() const;

	/** Returns the absolute world rotation (in degrees) extracted from the world matrix. */
	float GetComponentRotation() const;

	/** Returns the absolute world scale extracted from the world matrix. */
	FVector2D GetComponentScale() const;

	/** Returns the cached local-to-world matrix, rebuilding it lazily when dirty. */
	const FMatrix3x3& GetComponentToWorld() const;

	/** Attaches this component under a new parent, guarding against attachment cycles. */
	void AttachToComponent(USceneComponent* NewParent);

	/** Returns the parent this component is attached to, or nullptr if it is a root. */
	USceneComponent* GetAttachParent() const { return AttachParent; }

protected:
	/** Invalidates the cached world matrix for this component and all of its descendants. */
	void MarkWorldTransformDirty();

private:
	/** Coordinate transform tracking spatial properties inside the scene graph. */
	FTransform2D RelativeTransform;

	/** Non-owning pointer to the parent component in the attachment hierarchy. */
	USceneComponent* AttachParent = nullptr;

	/** Non-owning pointers to the child components attached to this one. */
	std::vector<USceneComponent*> AttachChildren;

	/** True when CachedWorldMatrix is stale and must be recomputed on next access. */
	mutable bool bWorldTransformDirty = true;

	/** Lazily evaluated local-to-world matrix, valid only while bWorldTransformDirty is false. */
	mutable FMatrix3x3 CachedWorldMatrix;
};
