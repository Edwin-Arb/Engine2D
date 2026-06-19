#pragma once

#include "IMathDemo.h"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

/**
 * Implements a 2D cone visibility algorithm assessing inclusion thresholds inside an angular limit.
 */
class FFieldOfViewDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	std::string GetDemoName() const override { return "Field Of View Demo | Vision Cone Detection"; }

private:
	std::vector<sf::Vertex> FOVConeVertices;

	bool bTargetDetected = false;
	float SavedSquareRot = 0.0f;
	FVector2D SavedSquarePos;

	const float Fov = 90.0f;
	const float HalfFov = Fov / 2.0f;
	const float DetectionDistance = 300.0f;
};