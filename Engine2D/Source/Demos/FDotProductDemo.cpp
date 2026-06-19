#include "FDotProductDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FDotProductDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// Build the square's forward direction and the direction toward the circle.
	const float SquareRot = InContext.MainSquare->GetComponentRotation();
	const FVector2D SquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D SquareForwardVector = FVector2D::GetForwardVector(SquareRot);
	const FVector2D ToCircle = FVector2D::Direction(SquarePos, CirclePos);

	// Dot of two unit vectors measures alignment: +1 facing the circle, -1 facing away.
	const float Dot = SquareForwardVector.DotProduct(ToCircle);

	// Visualize that alignment as the square's color.
	InContext.MainSquare->SetColorFromFactor(Dot);
}