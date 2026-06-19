#pragma once

#include "UActorComponent.h"
#include <vector>
#include "../../Math/FTransform2D.h"

/**
 * A component that has a spatial transform (position, rotation, scale) in the 2D world.
 */
class USceneComponent : public UActorComponent
{
public:
	~USceneComponent() override;

	void SetRelativeLocation(const FVector2D& NewLocation);
	void SetRelativeRotation(float NewRotation);
	void SetRelativeScale(const FVector2D& NewScale);

	FVector2D GetRelativeLocation() const;
	float GetRelativeRotation() const;
	FVector2D GetRelativeScale() const;

	void SetWorldLocation(const FVector2D& NewWorldLocation);

	FVector2D GetComponentLocation() const;
	float GetComponentRotation() const;
	FVector2D GetComponentScale() const;
	const FMatrix3x3& GetComponentToWorld() const;

	void AttachToComponent(USceneComponent* NewParent);
	USceneComponent* GetAttachParent() const { return AttachParent; }

protected:
	void MarkWorldTransformDirty();

private:
	/** Coordinate transform tracking spatial properties inside the scene graph. */
	FTransform2D RelativeTransform;
	USceneComponent* AttachParent = nullptr;
	std::vector<USceneComponent*> AttachChildren;

	mutable bool bWorldTransformDirty = true;
	mutable FMatrix3x3 CachedWorldMatrix;
};
