#pragma once

#include <compare>
#include <SFML/System/Vector2.hpp>
#include "../Core/Constants.h"

struct FVector2D
{
public:
	/** Vector components */
	float X;
	float Y;

public:
	/** Default constructor (initializes components to zero). */
	FVector2D();

	/** Initializes vector components from explicitly provided coordinates. */
	FVector2D(float InX, float InY);

	/** Conversion constructor from SFML's vector type. */
	FVector2D(const sf::Vector2f& InSFVector);

	/** Sets the vector coordinates directly. */
	void Set(float InX, float InY);

	/** Vector addition. */
	FVector2D operator+(const FVector2D& InOther) const;

	/** Vector subtraction. */
	FVector2D operator-(const FVector2D& InOther) const;

	/** Scalar multiplication. */
	FVector2D operator*(float InScalar) const;

	/** Scalar division. */
	FVector2D operator/(float InScalar) const;

	/** Scalar multiplication and assignment. */
	FVector2D& operator*=(float InScalar);

	/** Scalar division and assignment. */
	FVector2D& operator/=(float InScalar);

	/** Vector addition and assignment. */
	FVector2D& operator+=(const FVector2D& InOther);

	/** Vector subtraction and assignment. */
	FVector2D& operator-=(const FVector2D& InOther);

	/** Vector multiplication and assignment. */
	FVector2D& operator*=(const FVector2D& InOther);

	/** Division and assignment of vectors. */
	FVector2D& operator/=(const FVector2D& InOther);

	/** Exact equality check. Prefer IsNearlyEqual for floating-point calculations. */
	bool operator==(const FVector2D& InOther) const;

	/** Compares two vectors for equality within a given tolerance threshold. */
	bool IsNearlyEqual(const FVector2D& InOther, float InEpsilon = EPSILON) const;

	/** Exact inequality check. */
	bool operator!=(const FVector2D& InOther) const;

	/** Three-way comparison operator (spaceship operator) for ordering. */
	std::strong_ordering operator<=>(const FVector2D& InOther) const;

	/** Implicit conversion operator to SFML vector type. */
	operator sf::Vector2f() const;

	/** Returns the length (magnitude) of this vector. */
	float Length() const;

	/** Returns a normalized copy of this vector (unit vector direction). */
	FVector2D Normalized() const;

	/** Calculates the dot product between this vector and another. */
	float DotProduct(const FVector2D& InOther) const;

	/** Calculates the 2D cross product scalar (Z-component of 3D cross product). */
	float CrossProduct(const FVector2D& InOther) const;

	/** Calculates the angle between this vector and another in radians. */
	float Angle(const FVector2D& InOther) const;

	/** Reflects this vector against a given surface normal vector. */
	FVector2D Reflect(const FVector2D& InNormal) const;

	/** Rotates this vector by a specified angle in radians. */
	FVector2D Rotate(float InAngleRadians) const;

public:
	/** Calculates the Euclidean distance between two points. */
	static float Distance(const FVector2D& InA, const FVector2D& InB);

	/** Generates a direction vector pointing forward based on an orientation angle. */
	static FVector2D GetForwardVector(float InAngleDegrees);

	/** Generates a direction vector pointing right based on an orientation angle. */
	static FVector2D GetRightVector(float InAngleDegrees);

	/** Returns the normalized direction vector from one point to another. */
	static FVector2D Direction(const FVector2D& InFrom, const FVector2D& InTo);
};