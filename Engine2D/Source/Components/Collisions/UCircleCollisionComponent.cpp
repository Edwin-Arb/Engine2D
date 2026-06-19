#include "UCircleCollisionComponent.h"

#include <cmath>

UCircleCollisionComponent::UCircleCollisionComponent(float InRadius)
	: Radius(InRadius)
{
}

bool UCircleCollisionComponent::Intersects(UCollisionComponent* InOther, FVector2D& OutNormal, float& OutPenetration)
{
	// Nothing to test against.
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

	// Only circle-vs-circle is supported, so the other shape must also be a circle.
	UCircleCollisionComponent* OtherCircle = dynamic_cast<UCircleCollisionComponent*>(InOther);
	if (!OtherCircle)
	{
		return false;
	}

	// 1. Read both circles' world-space centers.
	const FVector2D ThisPos = GetComponentLocation();
	const FVector2D OtherPos = InOther->GetComponentLocation();

	// 2. Offset from Other to This, so OutNormal can directly push This out of Other.
	FVector2D Delta = ThisPos - OtherPos;
	const float DistanceSquared = Delta.DotProduct(Delta);

	// 3. Two circles overlap when the distance between centers is less than the sum of radii.
	const float RadiiSum = Radius + OtherCircle->GetRadius();
	const float RadiiSumSquared = RadiiSum * RadiiSum;

	// 4. Compare squared distances to avoid the sqrt while only testing for overlap.
	if (DistanceSquared >= RadiiSumSquared)
	{
		return false;  // No overlap.
	}

	// 5. Overlap confirmed: now compute the real distance for the normal and penetration.
	const float Distance = std::sqrt(DistanceSquared);

	// Guard against division by zero when both centers coincide exactly.
	if (Distance < EPSILON)
	{
		// Arbitrary fallback push direction (straight up along -Y).
		OutNormal = FVector2D(0.0f, -1.0f);
		OutPenetration = RadiiSum;
	}
	else
	{
		// Normalize the offset to get the push-out direction; penetration is the overlap depth.
		OutNormal = Delta / Distance;
		OutPenetration = RadiiSum - Distance;
	}

	return true;
}