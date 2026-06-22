#pragma once

#include "IMathDemo.h"
#include "../Core/FDebugDraw.h"

namespace sf
{
	class Font;
}

/** Which draggable handle the mouse is currently holding. */
enum class EDotGrab : uint8_t
{
	EDG_None,
	EDG_VectorA,
	EDG_VectorB,
	EDG_Origin
};

/**
 * Dot product, made visible. Two draggable vectors A and B share an origin; the demo draws
 * the PROJECTION of B onto A (B's "shadow" on A's line) plus a live read-out of A . B.
 * The shadow is long when the vectors align, zero when they are perpendicular, and negative
 * (drawn red) when they oppose - the same quantity that drives diffuse lighting (N . L).
 */
class FDotProductDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Dot Product Demo | Projection of B onto A"; }

private:
	/** Shared origin of both vectors (set to the screen centre in Enter). */
	FVector2D Origin;

	/** World-space tips of A and B - these are the draggable handles. */
	FVector2D PointA;
	FVector2D PointB;

	/** Which handle is currently being dragged. */
	EDotGrab Grabbed = EDotGrab::EDG_None;

	/** Click radius (px) within which a handle can be grabbed. */
	float GrabRadius = 30.0f;

	/** Radius (px) of the marker circles drawn on the points. */
	float MarkerRadius = 10.0f;

	// --- Cached scalars, refreshed every Tick, shown in the HUD ---
	float DotValue = 0.0f;		 // A . B
	float ProjLength = 0.0f;	 // signed length of B's shadow on A (= B . normalize(A))
	float AngleDegrees = 0.0f;	 // angle between A and B

	/** HUD font, borrowed from the demo context. */
	const sf::Font* Font = nullptr;

	/** Immediate-mode debug renderer used to draw the arrows and markers. */
	FDebugDraw DebugDraw;
};
