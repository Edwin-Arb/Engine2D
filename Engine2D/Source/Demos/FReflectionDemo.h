#pragma once

#include "IMathDemo.h"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

namespace sf
{
	class Font;
}

/**
 * Casts a ray from the square, reflects it off the target circle, and visualizes the
 * incident ray, surface normal, and reflected ray (R = I - 2*(I.N)*N).
 */
class FReflectionDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	std::string GetDemoName() const override { return "Reflection Demo | Raycast & Surface Reflection | WASD / R"; }

private:
	/** Returns the nearest forward intersection of a ray with a circle; outputs the hit distance in InOutT. */
	bool RayCircleIntersection(const FVector2D& InRayStart, const FVector2D& InRayDir, const FVector2D& InCircleCenter, float InRadius, float& InOutT);

private:
	/** Length of the cast ray, in world units. */
	const float RayLength = 200.0f;

	/** Cached results of the latest Tick, displayed as on-screen HUD text in Render. */
	bool bHasHit = false;
	float SavedDotProduct = 0.0f;
	float SavedIncidenceAngle = 0.0f;
	FVector2D SavedIncidentRay;
	FVector2D SavedReflectionNormal;
	FVector2D SavedReflectedRay;
	const sf::Font* SavedFont = nullptr;

	/** Line vertices (incident, reflected, normal) built in Tick and drawn in Render. */
	std::vector<sf::Vertex> ReflectionLines;
};