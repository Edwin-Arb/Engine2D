#include "UCircleCollisionComponent.h"

#include <cmath>

UCircleCollisionComponent::UCircleCollisionComponent(float InRadius)
	: Radius(InRadius)
{
}

bool UCircleCollisionComponent::Intersects(UCollisionComponent* InOther, FVector2D& OutNormal, float& OutPenetration)
{
	if (!InOther)
	{
		return false;
	}

	// TODO: Изменить RTTI, проверять указатель не каждый раз.
	// if (InOther->GetShapeType() != EShape::Circle)
	// {
	// 	return false;
	// }
	// auto* OtherCircle = static_cast<UCircleCollisionComponent*>(InOther);

	// Check if the incoming collision component is a Circle
	UCircleCollisionComponent* OtherCircle = dynamic_cast<UCircleCollisionComponent*>(InOther);
	if (!OtherCircle)
	{
		return false;
	}

	// 1. Gather global spatial coordinates from transform hierarchy nodes
	const FVector2D ThisPos = GetComponentLocation();
	const FVector2D OtherPos = InOther->GetComponentLocation();

	// 2. Compute directional offset delta vector (Points from Other towards This)
	// We point it this way so OutNormal can be directly used to push 'This' actor out of 'Other'
	FVector2D Delta = ThisPos - OtherPos;
	const float DistanceSquared = Delta.DotProduct(Delta);

	// 3. Compute structural radial threshold bounds boundaries
	const float RadiiSum = Radius + OtherCircle->GetRadius();
	const float RadiiSumSquared = RadiiSum * RadiiSum;

	// 4. Perform highly optimized square-distance intersection test pass
	if (DistanceSquared >= RadiiSumSquared)
	{
		return false;  // No collision overlap evaluated
	}

	// 5. Compute concrete spatial metrics as intersection is validated
	const float Distance = std::sqrt(DistanceSquared);

	// Edge Case Guard: Prevent division by zero if actor center coordinates perfectly overlap
	if (Distance < EPSILON)
	{
		// Arbitrary fallback displacement direction vector (Push straight up along Y-axis)
		OutNormal = FVector2D(0.0f, -1.0f);
		OutPenetration = RadiiSum;
	}
	else
	{
		// Standard calculation: Normalize directional delta offset vector
		OutNormal = Delta / Distance;
		OutPenetration = RadiiSum - Distance;
	}

	return true;
}