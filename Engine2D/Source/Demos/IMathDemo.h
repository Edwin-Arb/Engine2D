#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "../Math/FMatrix3x3.h"

struct FDemoContext;

/**
 * Common interface for all interactive math demos.
 * The game owns one instance per mode and drives them through this lifecycle.
 */
class IMathDemo
{
public:
	virtual ~IMathDemo() = default;

	/** Called once when this demo becomes active (resets the scene to a known state). */
	virtual void Enter(FDemoContext& InContext);

	/** Called every frame to update the demo's logic. */
	virtual void Tick(float InDeltaTime, FDemoContext& InContext) = 0;

	/** Optional hook to handle input events while this demo is active. */
	virtual void HandleEvent(const sf::Event& InEvent, FDemoContext& InContext) {}

	/** Optional hook to draw debug overlays (rays, grids, cones) for this demo. */
	virtual void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) {}

	/** Called once when switching away from this demo (undo any demo-specific state). */
	virtual void Exit(FDemoContext& InContext) {}

	/** Returns the demo's display name, shown in the on-screen HUD. */
	virtual std::string GetDemoName() const = 0;
};