#pragma once
#include "FMatrix3x3.h"

struct FTransform2D
{
public:
	FTransform2D() = default;
	explicit FTransform2D(const FVector2D& InPos, float InRot = 0.0f, const FVector2D& InScale = { 1.0f, 1.0f });

	/** Direct spatial mutation API. */
	void SetPosition(const FVector2D& NewPos) { Position = NewPos; }
	void SetRotation(float NewRot) { Rotation = NewRot; }
	void SetScale(const FVector2D& NewScale) { Scale = NewScale; }

	/** Local space parameters accessor methods. */
	FVector2D GetRelativePosition() const { return Position; }
	float GetRelativeRotation() const { return Rotation; }
	FVector2D GetRelativeScale() const { return Scale; }

	FMatrix3x3 ToMatrix() const;

private:
	/** Transform local space characteristics. */
	FVector2D Position{ 0.0f, 0.0f };
	float Rotation = 0.0f;	// Degrees
	FVector2D Scale{ 1.0f, 1.0f };
};