#include "FMatrixDemo.h"

#include "../Components/Base/URenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"
#include "../Math/FMath.h"

void FMatrixDemo::Enter(FDemoContext& InContext)
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

	Origin = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
	BasisX = FVector2D(1.0f, 0.0f);
	BasisY = FVector2D(0.0f, 1.0f);

	Grabbed = EMatrixGrab::EMG_None;
}

void FMatrixDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	DebugDraw.Clear();

	const FVector2D MousePos = InContext.MousePosition;
	const bool bMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	// --- Begin a grab: on a fresh press, grab whichever handle is under the cursor ---
	if (Grabbed == EMatrixGrab::EMG_None && bMouseDown)
	{
		const FVector2D BasisXTip = Origin + BasisX * UnitScale;
		const FVector2D BasisYTip = Origin + BasisY * UnitScale;

		if ((BasisXTip - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EMatrixGrab::EMG_BasicX;
		}
		else if ((BasisYTip - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EMatrixGrab::EMG_BasicY;
		}
		else if ((Origin - MousePos).Length() <= GrabRadius)
		{
			Grabbed = EMatrixGrab::EMG_Origin;
		}
	}

	// --- Drag the grabbed handle to the cursor, or release it on mouse-up ---
	if (Grabbed != EMatrixGrab::EMG_None)
	{
		if (!bMouseDown)
		{
			Grabbed = EMatrixGrab::EMG_None;
		}
		else if (Grabbed == EMatrixGrab::EMG_BasicX)
		{
			BasisX = (MousePos - Origin) / UnitScale;
		}
		else if (Grabbed == EMatrixGrab::EMG_BasicY)
		{
			BasisY = (MousePos - Origin) / UnitScale;
		}
		else if (Grabbed == EMatrixGrab::EMG_Origin)
		{
			Origin = MousePos;	// move only the origin; tips stay, so A and B restretch
		}
	}

	// The two vectors, measured from the shared origin.
	FVector2D LocalX = BasisX * UnitScale;
	FVector2D LocalY = BasisY * UnitScale;

	DebugDraw.AddArrow(Origin, LocalX, sf::Color::Red);
	DebugDraw.AddArrow(Origin, LocalY, sf::Color::Green);

	const FVector2D WorldLine1 = LocalToWorld(FVector2D(0.0f, 0.0f));
	const FVector2D WorldLine2 = LocalToWorld(FVector2D(1.0f, 0.0f));
	const FVector2D WorldLine3 = LocalToWorld(FVector2D(1.0f, 1.0f));
	const FVector2D WorldLine4 = LocalToWorld(FVector2D(0.0f, 1.0f));

	DebugDraw.AddLine(WorldLine1, WorldLine2, sf::Color::White);
	DebugDraw.AddLine(WorldLine2, WorldLine3, sf::Color::White);
	DebugDraw.AddLine(WorldLine3, WorldLine4, sf::Color::White);
	DebugDraw.AddLine(WorldLine4, WorldLine1, sf::Color::White);

	DebugDraw.AddCircle(WorldLine1, MarkerRadius, sf::Color::Red);
	DebugDraw.AddCircle(WorldLine2, MarkerRadius, sf::Color::Green);
	DebugDraw.AddCircle(WorldLine3, MarkerRadius, sf::Color::Green);
	DebugDraw.AddCircle(WorldLine4, MarkerRadius, sf::Color::Green);
}

void FMatrixDemo::Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix)
{
	DebugDraw.Draw(InWindow, InViewMatrix);
}

void FMatrixDemo::Exit(FDemoContext& InContext)
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

FVector2D FMatrixDemo::LocalToWorld(const FVector2D& InOutVec) const
{
	return Origin + (BasisX * InOutVec.X + BasisY * InOutVec.Y) * UnitScale;
}