#include "FTransform2D.h"

FTransform2D::FTransform2D(const FVector2D& InPos, float InRot, const FVector2D& InScale)
	: Position(InPos)
	, Rotation(InRot)
	, Scale(InScale)
{
}
FMatrix3x3 FTransform2D::ToMatrix() const
{
	return FMatrix3x3::CreateTranslation(Position) *  //
		   FMatrix3x3::CreateRotation(Rotation) *	  //
		   FMatrix3x3::CreateScale(Scale);
}