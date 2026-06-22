#include "FLinesDemo.h"

#include <SFML/Window/Mouse.hpp>

#include "../Components/Base/URenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"

void FLinesDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

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
	PointA = Origin + FVector2D(-200.0f, -300.0f);
	PointB = Origin + FVector2D(0.0f, 350.0f);
	Grabbed = EGrabbedVector::EGV_None;
}

void FLinesDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	DebugDraw.Clear();

	const FVector2D MousePos = InContext.MousePosition;
	const bool bMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	// Vectors measured FROM the shared origin (a tip is a point; subtract the origin to get
	// the displacement). Computed up front so the grab test below can hit the sum handle.
	FVector2D A = PointA - Origin;
	FVector2D B = PointB - Origin;
	FVector2D Sum = A + B;

	// --- Begin a grab: on a fresh press, grab whichever handle is under the cursor ---
	// Each test uses the EXACT point drawn as that handle, so "what you see is what you grab".
	if (Grabbed == EGrabbedVector::EGV_None && bMouseDown)
	{
		if ((PointA - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EGrabbedVector::EGV_VectorA;
		}
		else if ((PointB - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EGrabbedVector::EGV_VectorB;
		}
		else if ((Origin - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EGrabbedVector::EGV_VectorCenter;
		}
		else if ((Origin + Sum - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EGrabbedVector::EGV_VectorSum;
		}
	}

	// --- Drag the grabbed handle to the cursor, or release it on mouse-up ---
	if (Grabbed != EGrabbedVector::EGV_None)
	{
		if (!bMouseDown)
		{
			Grabbed = EGrabbedVector::EGV_None;
		}
		else if (Grabbed == EGrabbedVector::EGV_VectorA)
		{
			PointA = MousePos;
		}
		else if (Grabbed == EGrabbedVector::EGV_VectorB)
		{
			PointB = MousePos;
		}
		else if (Grabbed == EGrabbedVector::EGV_VectorCenter)
		{
			// Move ONLY the origin; the tips stay put, so A and B (= tip - origin) restretch.
			Origin = MousePos;
		}
		else if (Grabbed == EGrabbedVector::EGV_VectorSum)
		{
			// Dragging the result: pick the new A so that A + B reaches the cursor (B stays).
			// From A + B = DesiredSum we get A = DesiredSum - B.
			const FVector2D DesiredSum = MousePos - Origin;
			const FVector2D NewA = DesiredSum - B;
			PointA = Origin + NewA;
		}
	}

	// A drag above may have moved a point, so recompute before drawing. This keeps the arrows
	// and the handle markers in sync within the same frame (no one-frame lag).
	A = PointA - Origin;
	B = PointB - Origin;
	Sum = A + B;

	constexpr sf::Color ColorA = sf::Color::Cyan;
	constexpr sf::Color ColorB = sf::Color(255, 140, 0);  // orange
	constexpr sf::Color ColorSum = sf::Color::Green;

	// The two vectors A and B, both drawn from the shared origin.
	DebugDraw.AddArrow(Origin, A, ColorA);
	DebugDraw.AddArrow(Origin, B, ColorB);

	// Parallelogram: copy B tip-to-tail from A's tip, and copy A from B's tip. Both copies
	// land on the same far corner - which is exactly where the sum arrow points.
	DebugDraw.AddArrow(PointA, B, ColorB);
	DebugDraw.AddArrow(PointB, A, ColorA);

	// The result A + B: from the origin straight to that far corner.
	DebugDraw.AddArrow(Origin, Sum, ColorSum);

	// Handle markers (all draggable): yellow on A's & B's tips, white on the origin, green on the sum tip.
	DebugDraw.AddCircle(PointA, MarkerRadius, sf::Color::Yellow);
	DebugDraw.AddCircle(PointB, MarkerRadius, sf::Color::Yellow);
	DebugDraw.AddCircle(Origin, MarkerRadius, sf::Color::White);
	DebugDraw.AddCircle(Origin + Sum, MarkerRadius * 2.0f, ColorSum);
}

void FLinesDemo::Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	DebugDraw.Draw(InWindow, InViewMatrix);
}

void FLinesDemo::Exit(FDemoContext& InContext)
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
