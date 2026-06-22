#pragma once

#include "IMathDemo.h"
#include "../Core/FDebugDraw.h"

/** Which draggable handle of the diagram the mouse is currently holding. */
enum class EGrabbedVector : uint8_t
{
	EGV_VectorA,	   // tip of A
	EGV_VectorB,	   // tip of B
	EGV_VectorCenter,  // the shared origin
	EGV_VectorSum,	   // tip of A + B (dragging it adjusts A, keeping B)
	EGV_None
};

/**
 * Vector playground. Two vectors A and B share a common origin; you can drag the tip of A,
 * the tip of B, the origin, or the tip of the sum. The demo draws A, B, their sum A + B with
 * the full parallelogram, plus the perpendicular of A - so vector addition and the 90-degree
 * turn become things you can see, not just compute.
 *
 * Handle behaviour: dragging A's / B's tip changes that vector; dragging the origin keeps the
 * tips fixed so A and B (= tip - origin) restretch; dragging the sum's tip moves A so that
 * A + B reaches the cursor (B stays fixed).
 */
class FLinesDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Lines Demo | Vector Addition | Drag A, B, the centre or the sum"; }

private:
	/** Shared origin of both vectors (set to the screen centre in Enter). */
	FVector2D Origin;

	/** World-space tips of vectors A and B - these are draggable handles. */
	FVector2D PointA;
	FVector2D PointB;

	/** Which handle is currently being dragged. */
	EGrabbedVector Grabbed = EGrabbedVector::EGV_None;

	/** Click radius (px) within which a handle can be grabbed. */
	float GrabRadius = 30.0f;

	/** Radius (px) of the marker circles drawn on the points. */
	float MarkerRadius = 10.0f;

	/** Immediate-mode debug renderer used to draw the arrows and markers. */
	FDebugDraw DebugDraw;
};
