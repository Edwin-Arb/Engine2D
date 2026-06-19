#pragma once
#include "UCollisionComponent.h"

/**
 * Specialized 2D bounding circle volume component.
 * Performs fast radial intersection calculations.
 */
class UCircleCollisionComponent : public UCollisionComponent
{
public:
	/** Constructor initializing structural geometric radius specifications. */
	UCircleCollisionComponent(float InRadius);

	~UCircleCollisionComponent() override = default;

	/** Overridden structural evaluation method parsing out shape-specific intersections. */
	bool Intersects(UCollisionComponent* InOther, FVector2D& OutNormal, float& OutPenetration) override;

	/** Accessor for bounding extents. */
	float GetRadius() const { return Radius; }

private:
	/** Radial size definition boundary of this collision space. */
	float Radius = 0.0f;
};
