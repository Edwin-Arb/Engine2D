#include "FVector2D.h"
#include <algorithm>
#include <cmath>

FVector2D::FVector2D()
	: X(0.0f)
	, Y(0.0f)
{
}

FVector2D::FVector2D(float InX, float InY)
	: X(InX)
	, Y(InY)
{
}

FVector2D::FVector2D(const sf::Vector2f& InSFVector)
	: X(InSFVector.x)
	, Y(InSFVector.y)
{
}

void FVector2D::Set(float InX, float InY)
{
	X = InX;
	Y = InY;
}

float FVector2D::Length() const
{
	return std::sqrt(X * X + Y * Y);
}

float FVector2D::DotProduct(const FVector2D& InOther) const
{
	return X * InOther.X + Y * InOther.Y;
}

float FVector2D::CrossProduct(const FVector2D& InOther) const
{
	return X * InOther.Y - Y * InOther.X;
}

float FVector2D::Angle(const FVector2D& InOther) const
{
	const float Dot = DotProduct(InOther);
	const float MagnitudeProduct = Length() * InOther.Length();

	// Guard against division by zero for uninitialized or zero vectors
	if (MagnitudeProduct < EPSILON)
	{
		return 0.0f;
	}

	const float CosineValue = Dot / MagnitudeProduct;
	const float ClampedCosine = std::clamp(CosineValue, -1.0f, 1.0f);

	return std::acos(ClampedCosine);
}

float FVector2D::Distance(const FVector2D& InA, const FVector2D& InB)
{
	return (InB - InA).Length();
}

float FVector2D::DegreeToRadian(float InDegree)
{
	return InDegree * PI / 180.0f;
}

float FVector2D::RadianToDegree(float InRadian)
{
	return InRadian * 180.0f / PI;
}

float FVector2D::Lerp(float InA, float InB, float InT)
{
	return InA + (InB - InA) * InT;
}

bool FVector2D::IsNearlyEqual(const FVector2D& InOther, float InEpsilon) const
{
	return std::abs(X - InOther.X) < InEpsilon && std::abs(Y - InOther.Y) < InEpsilon;
}

bool FVector2D::operator==(const FVector2D& InOther) const
{
	return IsNearlyEqual(InOther);
}

bool FVector2D::operator!=(const FVector2D& InOther) const
{
	return !IsNearlyEqual(InOther);
}

std::strong_ordering FVector2D::operator<=>(const FVector2D& InOther) const
{
	// TODO: Consider squaring the lengths: LengthSquared()
	// Using Length() here also calls std::sqrt twice per comparison, which is expensive
	float ThisLength = Length();
	float OtherLength = InOther.Length();

	if (ThisLength < OtherLength)
	{
		return std::strong_ordering::less;
	}

	if (ThisLength > OtherLength)
	{
		return std::strong_ordering::greater;
	}

	return std::strong_ordering::equal;
}

FVector2D FVector2D::operator+(const FVector2D& InOther) const
{
	return FVector2D(X + InOther.X, Y + InOther.Y);
}

FVector2D FVector2D::operator-(const FVector2D& InOther) const
{
	return FVector2D(X - InOther.X, Y - InOther.Y);
}

FVector2D FVector2D::operator*(float InScalar) const
{
	return FVector2D(X * InScalar, Y * InScalar);
}

FVector2D FVector2D::operator/(float InScalar) const
{
	if (std::abs(InScalar) < EPSILON)
	{
		return FVector2D(0.0f, 0.0f);
	}

	return FVector2D(X / InScalar, Y / InScalar);
}

FVector2D& FVector2D::operator*=(float InScalar)
{
	X *= InScalar;
	Y *= InScalar;

	return *this;
}

FVector2D& FVector2D::operator/=(float InScalar)
{
	X /= InScalar;
	Y /= InScalar;

	return *this;
}

FVector2D& FVector2D::operator+=(const FVector2D& InOther)
{
	X += InOther.X;
	Y += InOther.Y;

	return *this;
}

FVector2D& FVector2D::operator-=(const FVector2D& InOther)
{
	X -= InOther.X;
	Y -= InOther.Y;

	return *this;
}

FVector2D& FVector2D::operator*=(const FVector2D& InOther)
{
	X *= InOther.X;
	Y *= InOther.Y;

	return *this;
}

FVector2D& FVector2D::operator/=(const FVector2D& InOther)
{
	X /= InOther.X;
	Y /= InOther.Y;

	return *this;
}

FVector2D::operator sf::Vector2f() const
{
	return sf::Vector2f(X, Y);
}

FVector2D FVector2D::Reflect(const FVector2D& InNormal) const
{
	const float Dot = DotProduct(InNormal);
	return *this - InNormal * (2.0f * Dot);
}

FVector2D FVector2D::Rotate(float InAngleRadians) const
{
	const float Cosine = std::cos(InAngleRadians);
	const float Sine = std::sin(InAngleRadians);

	return FVector2D(X * Cosine - Y * Sine, X * Sine + Y * Cosine);
}

FVector2D FVector2D::Normalized() const
{
	const float Len = Length();

	if (Len < EPSILON)
	{
		return FVector2D(0.0f, 0.0f);
	}

	return FVector2D(X / Len, Y / Len);
}

FVector2D FVector2D::GetForwardVector(float InAngleDegrees)
{
	const float AngleRadian = DegreeToRadian(InAngleDegrees);
	return FVector2D(std::cos(AngleRadian), std::sin(AngleRadian));
}

FVector2D FVector2D::GetRightVector(float InAngleDegrees)
{
	const float AngleRadian = DegreeToRadian(InAngleDegrees);
	return FVector2D(-std::sin(AngleRadian), std::cos(AngleRadian));
}

FVector2D FVector2D::Direction(const FVector2D& InFrom, const FVector2D& InTo)
{
	return (InTo - InFrom).Normalized();
}