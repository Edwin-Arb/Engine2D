#include "FMatrix3x3.h"
#include <cmath>

#include "FMath.h"

FMatrix3x3::FMatrix3x3()
{
	// Identity Matrix configuration
	// clang-format off
	M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f;
	M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f;
	M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f;
	// clang-format on
}

FMatrix3x3 FMatrix3x3::CreateTranslation(const FVector2D& InPosition)
{
	FMatrix3x3 Result;

	Result.M[0][2] = InPosition.X;
	Result.M[1][2] = InPosition.Y;

	return Result;
}

FMatrix3x3 FMatrix3x3::CreateRotation(float InAngleDegrees)
{
	FMatrix3x3 Result;
	const float AngleRad = FMath::DegreeToRadian(InAngleDegrees);

	const float Cos = std::cos(AngleRad);
	const float Sin = std::sin(AngleRad);

	Result.M[0][0] = Cos;
	Result.M[0][1] = -Sin;
	Result.M[1][0] = Sin;
	Result.M[1][1] = Cos;

	return Result;
}

FMatrix3x3 FMatrix3x3::CreateScale(const FVector2D& InScale)
{
	FMatrix3x3 Result;

	Result.M[0][0] = InScale.X;
	Result.M[1][1] = InScale.Y;

	return Result;
}

FMatrix3x3 FMatrix3x3::Inverse() const
{
	// Map local aliases for analytical matrix transposition/inversion legibility
	const float A = M[0][0], B = M[0][1], C = M[0][2];
	const float D = M[1][0], E = M[1][1], F = M[1][2];
	const float G = M[2][0], H = M[2][1], I = M[2][2];

	// Compute matrix determinant
	const float Det = A * (E * I - F * H) - B * (D * I - F * G) + C * (D * H - E * G);

	// Singular check to protect against catastrophic float division-by-zero crashes
	if (std::abs(Det) < EPSILON)
	{
		return FMatrix3x3();
	}

	const float InvDet = 1.0f / Det;
	FMatrix3x3 Result;

	// Adjugate matrix computation scaling directly with reciprocal determinant
	Result.M[0][0] = (E * I - F * H) * InvDet;
	Result.M[0][1] = (C * H - B * I) * InvDet;
	Result.M[0][2] = (B * F - C * E) * InvDet;

	Result.M[1][0] = (F * G - D * I) * InvDet;
	Result.M[1][1] = (A * I - C * G) * InvDet;
	Result.M[1][2] = (C * D - A * F) * InvDet;

	Result.M[2][0] = (D * H - E * G) * InvDet;
	Result.M[2][1] = (B * G - A * H) * InvDet;
	Result.M[2][2] = (A * E - B * D) * InvDet;

	return Result;
}

FMatrix3x3 FMatrix3x3::operator*(const FMatrix3x3& InOther) const
{
	FMatrix3x3 Result;

	for (int32_t Row = 0; Row < 3; Row++)
	{
		for (int32_t Col = 0; Col < 3; Col++)
		{
			Result.M[Row][Col] = M[Row][0] * InOther.M[0][Col] +  //
								 M[Row][1] * InOther.M[1][Col] +  //
								 M[Row][2] * InOther.M[2][Col];
		}
	}

	return Result;
}

FVector2D FMatrix3x3::TransformPoint(const FVector2D& InPoint) const
{
	FVector2D Result;

	Result.X = M[0][0] * InPoint.X +  //
			   M[0][1] * InPoint.Y +  //
			   M[0][2];

	Result.Y = M[1][0] * InPoint.X +  //
			   M[1][1] * InPoint.Y +  //
			   M[1][2];

	return Result;
}

FVector2D FMatrix3x3::TransformVector(const FVector2D& InVector) const
{
	FVector2D Result;

	Result.X = M[0][0] * InVector.X +  //
			   M[0][1] * InVector.Y;

	Result.Y = M[1][0] * InVector.X +  //
			   M[1][1] * InVector.Y;

	return Result;
}

sf::Transform FMatrix3x3::ToSFMLTransform() const
{
	return sf::Transform(M[0][0], M[0][1], M[0][2],	 //
						 M[1][0], M[1][1], M[1][2],	 //
						 M[2][0], M[2][1], M[2][2]);
}