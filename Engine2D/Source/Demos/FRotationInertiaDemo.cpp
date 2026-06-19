#include "FRotationInertiaDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FRotationInertiaDemo::Tick(float InDeltaTime, FDemoContext& InContext)
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
	const FVector2D ToCircle = CirclePos - SquarePos;

	const float Dot = SquareForwardVector.DotProduct(ToCircle.Normalized());
	const float Cross = SquareForwardVector.CrossProduct(ToCircle.Normalized());

	// Signed angle error from forward to target drives an angular acceleration.
	const float AngleError = std::atan2(Cross, Dot);
	const float Steering = AngleError * TurnBoostFactor;

	const float Power = 100.0f;
	AngularVelocity += Steering * SquareRotationSpeed * Power * InDeltaTime;

	// Apply damping scaled by the time step (referenced to 60 FPS) so it is frame-rate independent.
	const float ActualDamping = std::pow(Damping, InDeltaTime * 60.0f);
	AngularVelocity *= ActualDamping;

	// Integrate angular velocity into the current angle.
	float CurrentAngle = SquareRot;
	CurrentAngle += AngularVelocity * InDeltaTime;

	InContext.MainSquare->SetRelativeRotation(CurrentAngle);
}