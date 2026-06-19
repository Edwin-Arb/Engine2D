#pragma once

#include "IMathDemo.h"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

/**
 * Tests whether the target circle falls inside the square's vision cone (a field-of-view
 * check using angle and distance) and draws the cone.
 */
class FFieldOfViewDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	std::string GetDemoName() const override { return "Field Of View Demo | Vision Cone Detection"; }

private:
	/** Line vertices for the two cone edges, rebuilt each Tick and drawn in Render. */
	std::vector<sf::Vertex> FOVConeVertices;

	/** True when the target is inside the cone this frame. */
	bool bTargetDetected = false;

	/** Square pose captured in Tick so Render can draw the cone consistently. */
	float SavedSquareRot = 0.0f;
	FVector2D SavedSquarePos;

	/** Total field-of-view angle, its half (used for edge angles), and the view range. */
	const float Fov = 90.0f;
	const float HalfFov = Fov / 2.0f;
	const float DetectionDistance = 300.0f;
};