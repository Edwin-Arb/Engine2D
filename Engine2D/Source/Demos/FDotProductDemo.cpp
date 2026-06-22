#include "FDotProductDemo.h"

#include <sstream>
#include <iomanip>

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include "../Components/Base/URenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"
#include "../Math/FMath.h"

void FDotProductDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

	Font = InContext.Font;

	// Hide the persistent world scene so only our debug drawing is on screen.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(false);
		}
	}

	// Both vectors share an origin at the screen centre; give A and B two distinct default tips.
	Origin = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
	PointA = Origin + FVector2D(250.0f, 0.0f);
	PointB = Origin + FVector2D(150.0f, -200.0f);
	Grabbed = EDotGrab::EDG_None;
}

void FDotProductDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	DebugDraw.Clear();

	const FVector2D MousePos = InContext.MousePosition;
	const bool bMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	// --- Begin a grab: on a fresh press, grab whichever handle is under the cursor ---
	if (Grabbed == EDotGrab::EDG_None && bMouseDown)
	{
		if ((PointA - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EDotGrab::EDG_VectorA;
		}
		else if ((PointB - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EDotGrab::EDG_VectorB;
		}
		else if ((Origin - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EDotGrab::EDG_Origin;
		}
	}

	// --- Drag the grabbed handle to the cursor, or release it on mouse-up ---
	if (Grabbed != EDotGrab::EDG_None)
	{
		if (!bMouseDown)
		{
			Grabbed = EDotGrab::EDG_None;
		}
		else if (Grabbed == EDotGrab::EDG_VectorA)
		{
			PointA = MousePos;
		}
		else if (Grabbed == EDotGrab::EDG_VectorB)
		{
			PointB = MousePos;
		}
		else if (Grabbed == EDotGrab::EDG_Origin)
		{
			Origin = MousePos;  // move only the origin; tips stay, so A and B restretch
		}
	}

	// The two vectors, measured from the shared origin.
	const FVector2D A = PointA - Origin;
	const FVector2D B = PointB - Origin;

	// --- Projection of B onto A (the dot product, made visible) ---
	const FVector2D DirA = A.Normalized();			  // unit direction of A
	const FVector2D ProjVector = DirA * B.DotProduct(DirA);  // B's shadow as a vector along A

	// Cache the numbers for the HUD.
	DotValue = A.DotProduct(B);
	ProjLength = B.DotProduct(DirA);
	AngleDegrees = FMath::RadianToDegree(A.Angle(B));

	constexpr sf::Color ColorA = sf::Color::Cyan;
	constexpr sf::Color ColorB = sf::Color(255, 140, 0);  // orange

	// The two vectors from the shared origin.
	DebugDraw.AddArrow(Origin, A, ColorA);
	DebugDraw.AddArrow(Origin, B, ColorB);

	// The projection, coloured by the SIGN of the dot product: green = same direction (dot > 0),
	// red = opposite (dot < 0). At 90 degrees it collapses to zero.
	const sf::Color ProjColor = (ProjLength >= 0.0f) ? sf::Color::Green : sf::Color::Red;
	DebugDraw.AddArrow(Origin, ProjVector, ProjColor);

	// Drop line from B's tip down to its shadow (connects two POINTS, so it is a line).
	DebugDraw.AddLine(PointB, Origin + ProjVector, sf::Color(120, 120, 120));

	// Handle markers (draggable): yellow on the two tips, white on the origin.
	DebugDraw.AddCircle(PointA, MarkerRadius, sf::Color::Yellow);
	DebugDraw.AddCircle(PointB, MarkerRadius, sf::Color::Yellow);
	DebugDraw.AddCircle(Origin, MarkerRadius, sf::Color::White);
}

void FDotProductDemo::Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	// Draw the arrows/markers in world space.
	DebugDraw.Draw(InWindow, InViewMatrix);

	if (!Font)
	{
		return;
	}

	// Format helper for the numeric read-out.
	auto Format = [](float InValue) -> std::string {
		std::ostringstream Oss;
		Oss << std::fixed << std::setprecision(1) << InValue;
		return Oss.str();
	};

	auto MakeText = [this](const std::string& InString, sf::Color InColor, float InY) {
		sf::Text Text(*Font);
		Text.setString(InString);
		Text.setCharacterSize(18);
		Text.setFillColor(InColor);
		Text.setPosition({ 10.0f, InY });
		return Text;
	};

	// HUD (screen space): legend + the live dot-product numbers.
	InWindow.draw(MakeText("Cyan: A   Orange: B   Green/Red: projection of B onto A   (drag A, B or the white origin)", sf::Color::White, 40.0f));
	InWindow.draw(MakeText("A . B = " + Format(DotValue), sf::Color::Cyan, 70.0f));
	InWindow.draw(MakeText("Projection length (B . normalize(A)) = " + Format(ProjLength), sf::Color::Green, 95.0f));
	InWindow.draw(MakeText("Angle between A and B = " + Format(AngleDegrees) + " deg", sf::Color::Yellow, 120.0f));
}

void FDotProductDemo::Exit(FDemoContext& InContext)
{
	// Restore the persistent world scene we hid on Enter.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(true);
		}
	}

	DebugDraw.Clear();
}
