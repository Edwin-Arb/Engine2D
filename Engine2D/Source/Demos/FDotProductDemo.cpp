#include "FDotProductDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FDotProductDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	const float SquareRot = InContext.MainSquare->GetComponentRotation();
	const FVector2D SquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D SquareForwardVector = FVector2D::GetForwardVector(SquareRot);
	const FVector2D ToCircle = FVector2D::Direction(SquarePos, CirclePos);

	const float Dot = SquareForwardVector.DotProduct(ToCircle);

	// Pass dot projection scalar off directly to map color gradients
	InContext.MainSquare->SetColorFromFactor(Dot);
}