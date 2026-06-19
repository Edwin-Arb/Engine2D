#include "FReflectionDemo.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <SFML/Graphics/Text.hpp>

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void FReflectionDemo::Enter(FDemoContext& InContext)
{
	// Reset cached state before the default scene setup runs.
	ReflectionLines.clear();

	bHasHit = false;
	SavedFont = nullptr;
	SavedReflectionNormal = FVector2D(0.0f, 0.0f);

	IMathDemo::Enter(InContext);
}

void FReflectionDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	if (!InContext.MainSquare || !InContext.MainCircle)
	{
		return;
	}

	SavedFont = InContext.Font;

	// The incident ray points along the square's forward direction.
	const float SquareRot = InContext.MainSquare->GetComponentRotation();
	const FVector2D SquarePos = InContext.MainSquare->GetComponentLocation();
	const FVector2D CirclePos = InContext.MainCircle->GetComponentLocation();

	const FVector2D IncidentRay = FVector2D::GetForwardVector(SquareRot);
	SavedIncidentRay = IncidentRay;

	// Start the ray at the square's edge (not its center) and cast it at the circle.
	const float Offset = SQUARE_SIZE_X / 2.0f;
	const FVector2D ReflectionRayStart = SquarePos + (IncidentRay * Offset);
	FVector2D ReflectionRayEnd;
	const FVector2D MirrorCenter = CirclePos;

	ReflectionLines.clear();
	float T = 0.0f;
	const bool bHit = RayCircleIntersection(ReflectionRayStart, IncidentRay, MirrorCenter, CIRCLE_RADIUS, T);

	if (bHit && T > 0.0f && T <= RayLength)
	{
		bHasHit = true;
		const FVector2D HitPoint = ReflectionRayStart + (IncidentRay * T);
		ReflectionRayEnd = HitPoint;

		// On a circle, the surface normal at the hit point is the direction from the center outward.
		const FVector2D Normal = HitPoint - MirrorCenter;
		if (Normal.Length() > EPSILON)
		{
			SavedReflectionNormal = Normal.Normalized();
		}

		// Reflect the incident ray about that normal (R = I - 2*(I.N)*N, done inside Reflect).
		SavedReflectedRay = IncidentRay.Reflect(SavedReflectionNormal);
		const FVector2D ReflectedRayEnd = HitPoint + (SavedReflectedRay * RayLength);

		// Populate colorized line segments (Red = Input, Green = Reflection, Blue = Contact Surface Normal)
		ReflectionLines.emplace_back(sf::Vector2f(ReflectionRayStart.X, ReflectionRayStart.Y), sf::Color::Red);
		ReflectionLines.emplace_back(sf::Vector2f(ReflectionRayEnd.X, ReflectionRayEnd.Y), sf::Color::Red);

		ReflectionLines.emplace_back(sf::Vector2f(HitPoint.X, HitPoint.Y), sf::Color::Green);
		ReflectionLines.emplace_back(sf::Vector2f(ReflectedRayEnd.X, ReflectedRayEnd.Y), sf::Color::Green);

		const FVector2D NormalEnd = HitPoint + (SavedReflectionNormal * (RayLength * 0.5f));
		ReflectionLines.emplace_back(sf::Vector2f(HitPoint.X, HitPoint.Y), sf::Color::Blue);
		ReflectionLines.emplace_back(sf::Vector2f(NormalEnd.X, NormalEnd.Y), sf::Color::Blue);
	}
	else
	{
		// No hit: draw the incident ray at full length and nothing else.
		bHasHit = false;
		ReflectionRayEnd = ReflectionRayStart + IncidentRay * RayLength;
		ReflectionLines.emplace_back(sf::Vector2f(ReflectionRayStart.X, ReflectionRayStart.Y), sf::Color::Red);
		ReflectionLines.emplace_back(sf::Vector2f(ReflectionRayEnd.X, ReflectionRayEnd.Y), sf::Color::Red);
	}

	// Cache the dot product and incidence angle for the on-screen HUD.
	SavedDotProduct = SavedIncidentRay.DotProduct(SavedReflectionNormal);
	const float AngleWithNormal = std::acos(std::clamp(std::abs(SavedDotProduct), 0.0f, 1.0f));
	SavedIncidenceAngle = 90.0f - FVector2D::RadianToDegree(AngleWithNormal);
}

void FReflectionDemo::Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	if (ReflectionLines.empty())
	{
		return;
	}

	// Draw the ray lines in world space by applying the camera view matrix.
	sf::RenderStates LineStates;
	LineStates.transform = InViewMatrix.ToSFMLTransform();
	InWindow.draw(ReflectionLines.data(), ReflectionLines.size(), sf::PrimitiveType::Lines, LineStates);

	if (!SavedFont)
	{
		return;
	}

	auto Format = [](float InValue) -> std::string {
		std::ostringstream Oss;
		Oss << std::fixed << std::setprecision(2) << InValue;
		return Oss.str();
	};

	auto CreateText = [this](const std::string& InString, int32_t InSize, sf::Color InColor, sf::Vector2f InPosition) {
		sf::Text Text(*SavedFont);
		Text.setString(sf::String::fromUtf8(InString.begin(), InString.end()));
		Text.setCharacterSize(InSize);
		Text.setFillColor(InColor);
		Text.setPosition(InPosition);
		return Text;
	};

	// HUD overlay (screen space): legend plus the live reflection numbers.
	InWindow.draw(CreateText(
		"White: Square Direction | Red: Incident Ray (I) | Green: Reflected Ray (R)\nBlue: Normal (N) | Yellow: Mirror | Use WASD to rotate square", 16, sf::Color::White, { 10.0f, 40.0f }));
	InWindow.draw(CreateText("Formula: R = I - 2 * (I . N) * N", 16, sf::Color::Cyan, { 10.0f, 80.0f }));
	InWindow.draw(CreateText("I . N = " + Format(SavedDotProduct) + " | 2 * (I . N) = " + Format(2.0f * SavedDotProduct), 16, sf::Color::Cyan, { 10.0f, 110.0f }));
	InWindow.draw(CreateText("Incident Ray (I): (" + Format(SavedIncidentRay.X) + ", " + Format(SavedIncidentRay.Y) + ")", 16, sf::Color::Red, { 10.0f, 140.0f }));
	InWindow.draw(CreateText("Normal (N): (" + Format(SavedReflectionNormal.X) + ", " + Format(SavedReflectionNormal.Y) + ")", 16, sf::Color::Blue, { 10.0f, 160.0f }));
	InWindow.draw(CreateText("Reflected Ray (R): (" + Format(SavedReflectedRay.X) + ", " + Format(SavedReflectedRay.Y) + ")", 16, sf::Color::Green, { 10.0f, 180.0f }));

	if (bHasHit)
	{
		InWindow.draw(CreateText("Incidence Angle = Reflection Angle = " + Format(SavedIncidenceAngle) + "°", 16, sf::Color::Magenta, { 10.0f, 210.0f }));
	}
	else
	{
		InWindow.draw(CreateText("No Intersection", 16, sf::Color::Red, { 10.0f, 210.0f }));
	}
}

bool FReflectionDemo::RayCircleIntersection(const FVector2D& InRayStart, const FVector2D& InRayDir, const FVector2D& InCircleCenter, float InRadius, float& InOutT)
{
	// Substituting the ray P(t) = start + t*dir into |P - center|^2 = r^2 yields the
	// quadratic A*t^2 + B*t + C = 0 with the coefficients below.
	const FVector2D CenterToRayStart = InRayStart - InCircleCenter;

	const float A = InRayDir.DotProduct(InRayDir);
	const float B = 2.0f * CenterToRayStart.DotProduct(InRayDir);
	const float C = CenterToRayStart.DotProduct(CenterToRayStart) - (InRadius * InRadius);

	// A negative discriminant means the ray's line misses the circle entirely.
	const float Discriminant = B * B - 4.0f * A * C;

	if (Discriminant < 0.0f)
	{
		return false;
	}

	// Two roots: T1 is the nearer one. Pick the closest intersection in front of the ray.
	const float SqrtD = std::sqrt(Discriminant);
	const float T1 = (-B - SqrtD) / (2.0f * A);
	const float T2 = (-B + SqrtD) / (2.0f * A);

	if (T1 > 0.0f)
	{
		InOutT = T1;
	}
	else if (T2 > 0.0f)
	{
		InOutT = T2;
	}
	else
	{
		// Both intersections are behind the ray start.
		return false;
	}

	return true;
}