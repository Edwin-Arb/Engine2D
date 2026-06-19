#include "FSteeringDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FSteeringDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// The demo needs both the square and the target circle to run.
	if (!InContext.MainSquare || !InContext.MainCircle)
	{
		return;
	}

	const float SquareRot = InContext.MainSquare->GetComponentRotation();
	const FVector2D SquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D SquareForwardVector = FVector2D::GetForwardVector(SquareRot);
	const FVector2D ToCircle = FVector2D::Direction(SquarePos, CirclePos);

	// atan2(cross, dot) gives the signed angle (error) from forward to the target.
	const float Dot = SquareForwardVector.DotProduct(ToCircle);
	const float Cross = SquareForwardVector.CrossProduct(ToCircle);

	const float AngleError = std::atan2(Cross, Dot);
	const float Steering = AngleError * TurnBoostFactor;

	// Rotate toward the target this frame, proportional to the error.
	float CurrentAngle = SquareRot;
	CurrentAngle += Steering * SquareRotationSpeed * InDeltaTime;

	InContext.MainSquare->SetRelativeRotation(CurrentAngle);
}