#pragma once

#include "IMathDemo.h"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

namespace sf
{
	class Font;
}

/**
 * Simulates light/laser beam physics via algebraic ray-circle quadratic intersection sweeps.
 */
class FReflectionDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) override;
	std::string GetDemoName() const override { return "Reflection Demo | Raycast & Surface Reflection | WASD / R"; }

private:
	/** Solves quadratic equations to detect vector line segment intersection points against a 2D bounding circle. */
	bool RayCircleIntersection(const FVector2D& InRayStart, const FVector2D& InRayDir, const FVector2D& InCircleCenter, float InRadius, float& InOutT);

private:
	const float RayLength = 200.0f;

	/** UI Data Cache Variables */
	bool bHasHit = false;
	float SavedDotProduct = 0.0f;
	float SavedIncidenceAngle = 0.0f;
	FVector2D SavedIncidentRay;
	FVector2D SavedReflectionNormal;
	FVector2D SavedReflectedRay;
	const sf::Font* SavedFont = nullptr;

	/** Internal line primitive buffer processing collection. */
	std::vector<sf::Vertex> ReflectionLines;
};