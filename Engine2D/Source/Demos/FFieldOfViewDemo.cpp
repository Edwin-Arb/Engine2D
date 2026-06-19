#include "FFieldOfViewDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"
#include <algorithm>

void FFieldOfViewDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// The demo needs both the square and the target circle to run.
	if (!InContext.MainSquare || !InContext.MainCircle)
	{
		return;
	}

	// Rebuild the cone geometry from scratch each frame.
	FOVConeVertices.clear();

	SavedSquareRot = InContext.MainSquare->GetComponentRotation();
	SavedSquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D ToCircle = CirclePos - SavedSquarePos;
	const float Distance = ToCircle.Length();

	bTargetDetected = false;

	// If the target overlaps the square's center, it always counts as detected.
	if (Distance <= CIRCLE_RADIUS)
	{
		bTargetDetected = true;
	}
	// Otherwise it must be within view range AND within the cone's angle.
	else if (Distance - CIRCLE_RADIUS <= DetectionDistance)
	{
		const FVector2D Forward = FVector2D::GetForwardVector(SavedSquareRot);
		const FVector2D DirToCircle = ToCircle.Normalized();

		// Angle between the square's forward direction and the direction to the target center.
		const float Dot = Forward.DotProduct(DirToCircle);
		const float AngleToCenterRad = std::acos(std::clamp(Dot, -1.0f, 1.0f));
		const float AngleToCenterDeg = FVector2D::RadianToDegree(AngleToCenterRad);

		// The circle has an apparent angular radius; subtracting it lets an edge count as visible.
		const float CircleAngularRadiusRad = std::asin(CIRCLE_RADIUS / Distance);
		const float CircleAngularRadiusDeg = FVector2D::RadianToDegree(CircleAngularRadiusRad);

		if (AngleToCenterDeg - CircleAngularRadiusDeg <= HalfFov)
		{
			bTargetDetected = true;
		}
	}

	// Color the square gray when it sees the target, red otherwise.
	const sf::Color ConeColor = sf::Color::Yellow;
	const sf::Color SquareColor = bTargetDetected ? GRAY_COLOR : sf::Color::Red;

	InContext.MainSquare->SetColor(SquareColor);

	// Build the two cone edges (at +/- half the FOV) as line segments from the square.
	const FVector2D LeftDir = FVector2D::GetForwardVector(SavedSquareRot - HalfFov);
	const FVector2D RightDir = FVector2D::GetForwardVector(SavedSquareRot + HalfFov);

	const FVector2D LeftEdge = SavedSquarePos + LeftDir * DetectionDistance;
	const FVector2D RightEdge = SavedSquarePos + RightDir * DetectionDistance;

	FOVConeVertices.emplace_back(sf::Vector2f(SavedSquarePos.X, SavedSquarePos.Y), ConeColor);
	FOVConeVertices.emplace_back(sf::Vector2f(LeftEdge.X, LeftEdge.Y), ConeColor);

	FOVConeVertices.emplace_back(sf::Vector2f(SavedSquarePos.X, SavedSquarePos.Y), ConeColor);
	FOVConeVertices.emplace_back(sf::Vector2f(RightEdge.X, RightEdge.Y), ConeColor);
}

void FFieldOfViewDemo::Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	if (FOVConeVertices.empty())
	{
		return;
	}

	// Apply the camera view matrix so the overlay moves with the rest of the scene.
	sf::RenderStates States;
	States.transform = InViewMatrix.ToSFMLTransform();

	sf::CircleShape FovArc;
	FovArc.setRadius(DetectionDistance);
	FovArc.setOrigin({ DetectionDistance, DetectionDistance });
	FovArc.setPosition({ SavedSquarePos.X, SavedSquarePos.Y });
	FovArc.setFillColor(sf::Color::Transparent);
	FovArc.setOutlineColor(sf::Color::Yellow);
	FovArc.setOutlineThickness(2.0f);

	const float RotationAngle = SavedSquareRot - HalfFov;
	FovArc.setRotation(sf::degrees(RotationAngle));

	// Draw the range circle, then the two cone edge lines, both in view space.
	InWindow.draw(FovArc, States);
	InWindow.draw(FOVConeVertices.data(), FOVConeVertices.size(), sf::PrimitiveType::Lines, States);
}