#pragma once

#include "IMathDemo.h"

/**
 * Lets the user drive the camera transform directly: pan with WASD, rotate with the
 * mouse buttons, zoom with the wheel - showing how the view matrix moves the whole scene.
 */
class FCameraTransformDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void HandleEvent(const sf::Event& InEvent, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Camera Transform Demo | Move: WASD | Rotate: LMB/RMB | Zoom: Mouse Wheel | Reset: SPACE"; }

private:
	/** Camera pan speed in world units per second. */
	const float Speed = 300.0f;

	/** Camera rotation speed in degrees per second. */
	const float RotationSpeed = 120.0f;

	/** Zoom amount applied per mouse-wheel notch. */
	const float ZoomSpeed = 0.1f;

	/** Zoom (scale) clamp range. */
	const float MinZoom = 0.2f;
	const float MaxZoom = 3.0f;
};