#pragma once
#include "UCollisionComponent.h"

/**
 * Specialized 2D bounding circle volume component.
 * Performs fast radial intersection calculations.
 */
class UCircleCollisionComponent : public UCollisionComponent
{
public:
	/** Constructs a circle collider with the given radius. */
	UCircleCollisionComponent(float InRadius);

	~UCircleCollisionComponent() override = default;

	/** Tests this circle against another collision shape (currently circle-vs-circle). */
	bool Intersects(UCollisionComponent* InOther, FVector2D& OutNormal, float& OutPenetration) override;

	/** Returns the circle's radius. */
	float GetRadius() const { return Radius; }

private:
	/** Radius of the bounding circle, in world units. */
	float Radius = 0.0f;
};
