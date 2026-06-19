#pragma once

#include <SFML/Graphics/Transform.hpp>
#include "FVector2D.h"

struct FMatrix3x3
{
public:
	/** Initializes this matrix to the Identity matrix. */
	FMatrix3x3();

	/** Creates a translation matrix from a 2D offset vector. */
	static FMatrix3x3 CreateTranslation(const FVector2D& InPosition);

	/** Creates a rotation matrix from an orientation specified in degrees. */
	static FMatrix3x3 CreateRotation(float InAngleDegrees);

	/** Creates a scale matrix from 2D scale multipliers. */
	static FMatrix3x3 CreateScale(const FVector2D& InScale);

	/** Returns the analytical inverse of this matrix. Returns identity if singular. */
	FMatrix3x3 Inverse() const;

	/** Matrix multiplication operator. Concatenates transformations. */
	FMatrix3x3 operator*(const FMatrix3x3& InOther) const;

	/** Transforms a positional point (applies translation, rotation, and scale). */
	FVector2D TransformPoint(const FVector2D& InPoint) const;

	/** Transforms a directional vector (applies rotation and scale only, ignores translation). */
	FVector2D TransformVector(const FVector2D& InVector) const;

	/** Gets a specific matrix coefficient by row and column indices. */
	float Get(int32_t InRow, int32_t InCol) const { return M[InRow][InCol]; }

	/** Converts this representation into an SFML-compatible native object. */
	sf::Transform ToSFMLTransform() const;

public:
	/** Internal matrix elements storage. */
	float M[3][3];
};