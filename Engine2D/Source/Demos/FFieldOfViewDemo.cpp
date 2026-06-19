#include "FFieldOfViewDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"
#include <algorithm>

void FFieldOfViewDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	if (!InContext.MainSquare || !InContext.MainCircle)
	{
		return;
	}

	FOVConeVertices.clear();

	SavedSquareRot = InContext.MainSquare->GetComponentRotation();
	SavedSquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D ToCircle = CirclePos - SavedSquarePos;
	const float Distance = ToCircle.Length();

	bTargetDetected = false;

	// Fail-safe protection bounds: 100% collision if internal tracking drops inside radius limits
	if (Distance <= CIRCLE_RADIUS)
	{
		bTargetDetected = true;
	}
	else if (Distance - CIRCLE_RADIUS <= DetectionDistance)
	{
		const FVector2D Forward = FVector2D::GetForwardVector(SavedSquareRot);
		const FVector2D DirToCircle = ToCircle.Normalized();

		const float Dot = Forward.DotProduct(DirToCircle);
		const float AngleToCenterRad = std::acos(std::clamp(Dot, -1.0f, 1.0f));
		const float AngleToCenterDeg = FVector2D::RadianToDegree(AngleToCenterRad);

		// Resolve apparent angular width offsets under custom spatial gaps
		const float CircleAngularRadiusRad = std::asin(CIRCLE_RADIUS / Distance);
		const float CircleAngularRadiusDeg = FVector2D::RadianToDegree(CircleAngularRadiusRad);

		if (AngleToCenterDeg - CircleAngularRadiusDeg <= HalfFov)
		{
			bTargetDetected = true;
		}
	}

	const sf::Color ConeColor = sf::Color::Yellow;
	const sf::Color SquareColor = bTargetDetected ? GRAY_COLOR : sf::Color::Red;

	InContext.MainSquare->SetColor(SquareColor);

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

	// Prepare RenderStates payload carrying camera transformation context bindings
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

	// Fixed: Arc drawing pass now shifts elegantly alongside view modifications
	InWindow.draw(FovArc, States);

	// Fixed: Outer line vectors map correctly in camera relative space
	InWindow.draw(FOVConeVertices.data(), FOVConeVertices.size(), sf::PrimitiveType::Lines, States);
}