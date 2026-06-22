#include "FDebugDraw.h"

#include <cmath>

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include "../Math/FMath.h"
#include "../Math/FVector2D.h"
#include "../Math/FMatrix3x3.h"

void FDebugDraw::Clear()
{
	Lines.clear();
}

void FDebugDraw::AddLine(const FVector2D& InA, const FVector2D& InB, const sf::Color& Color)
{
	// A segment is just two vertices; FVector2D converts implicitly to sf::Vector2f.
	Lines.push_back(sf::Vertex(InA, Color));
	Lines.push_back(sf::Vertex(InB, Color));
}

void FDebugDraw::AddArrow(const FVector2D& InStart, const FVector2D& InVec, const sf::Color& Color)
{
	// InVec is a DISPLACEMENT (direction + length), so the tip is Start + Vec.
	const FVector2D End = InStart + InVec;

	// Head barbs: take a short vector pointing BACK from the tip, then splay it
	// left and right by HeadAngle.
	const FVector2D Direction = InVec.Normalized();
	const FVector2D Back = Direction * -HeadLength;

	const float AngleRad = FMath::DegreeToRadian(HeadAngle);
	const FVector2D LeftBarb = Back.Rotate(+AngleRad);
	const FVector2D RightBarb = Back.Rotate(-AngleRad);

	AddLine(InStart, End, Color);		   // shaft
	AddLine(End, End + LeftBarb, Color);   // head - left barb
	AddLine(End, End + RightBarb, Color);  // head - right barb
}

void FDebugDraw::AddCircle(const FVector2D& InCenter, float InRadius, const sf::Color& Color)
{
	// Walk around the circle in equal angle steps and connect each point to the next.
	// A point on the circle is the parametric form: center + (cos a, sin a) * radius.
	constexpr int32_t NumSegments = 24;

	for (int32_t i = 0; i < NumSegments; ++i)
	{
		const float AngleA = 2.0f * PI * static_cast<float>(i) / static_cast<float>(NumSegments);
		const float AngleB = 2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(NumSegments);

		const FVector2D PointA(InCenter.X + std::cos(AngleA) * InRadius, InCenter.Y + std::sin(AngleA) * InRadius);
		const FVector2D PointB(InCenter.X + std::cos(AngleB) * InRadius, InCenter.Y + std::sin(AngleB) * InRadius);

		AddLine(PointA, PointB, Color);
	}
}

void FDebugDraw::Draw(sf::RenderWindow& Window, const FMatrix3x3& ViewMatrix)
{
	if (Lines.empty())
	{
		return;
	}

	// One draw call for the whole batch. The view matrix maps world space onto the screen.
	sf::RenderStates States;
	States.transform = ViewMatrix.ToSFMLTransform();
	Window.draw(Lines.data(), Lines.size(), sf::PrimitiveType::Lines, States);
}
