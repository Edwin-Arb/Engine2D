#pragma once

#include "IMathDemo.h"

/**
 * Demo logic mapping keyboard and mouse scroll interactions onto view space adjustments.
 */
class FCameraTransformDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void HandleEvent(const sf::Event& InEvent, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Camera Transform Demo | Move: WASD | Rotate: LMB/RMB | Zoom: Mouse Wheel | Reset: SPACE"; }

private:
	const float Speed = 300.0f;
	const float RotationSpeed = 120.0f;
	const float ZoomSpeed = 0.1f;
	const float MinZoom = 0.2f;
	const float MaxZoom = 3.0f;
};