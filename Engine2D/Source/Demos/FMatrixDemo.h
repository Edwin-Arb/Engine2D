#pragma once
#include "IMathDemo.h"
#include "../Core/FDebugDraw.h"

/** Which draggable handle the mouse is currently holding. */
enum class EMatrixGrab : uint8_t
{
	EMG_None,
	EMG_BasicX,
	EMG_BasicY,
	EMG_Origin
};

class FMatrixDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	void Exit(FDemoContext& InContext) override;
	FVector2D LocalToWorld(const FVector2D& InOutVec) const;
	std::string GetDemoName() const override { return "Matrix Demo | Rotation & Translation"; }

private:
	const float UnitScale = 200.0f;

	/** Shared origin of both vectors (set to the screen centre in Enter). */
	FVector2D Origin;

	FVector2D BasisX;
	FVector2D BasisY;

	/** Click radius (px) within which a handle can be grabbed. */
	float GrabRadius = 30.0f;

	/** Radius (px) of the marker circles drawn on the points. */
	float MarkerRadius = 10.0f;

	/** Which handle is currently being dragged. */
	EMatrixGrab Grabbed = EMatrixGrab::EMG_None;

	/** Immediate-mode debug renderer used to draw the arrows and markers. */
	FDebugDraw DebugDraw;
};
