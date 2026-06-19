#include "FCrossProductDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FCrossProductDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// Build the square's forward direction and the direction toward the circle.
	const float SquareRot = InContext.MainSquare->GetComponentRotation();
	const FVector2D SquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D SquareForwardVector = FVector2D::GetForwardVector(SquareRot);
	const FVector2D ToCircle = FVector2D::Direction(SquarePos, CirclePos);

	// 2D cross product's sign tells which side the circle is on (left vs. right of forward).
	const float Cross = SquareForwardVector.CrossProduct(ToCircle);

	// Visualize that side as the square's color.
	InContext.MainSquare->SetColorFromFactor(Cross);
}