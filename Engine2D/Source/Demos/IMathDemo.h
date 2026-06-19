#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "../Math/FMatrix3x3.h"

struct FDemoContext;

/**
 * Interface class providing the core contract for distinct mathematics environment demos.
 */
class IMathDemo
{
public:
	virtual ~IMathDemo() = default;

	/** Triggered once immediately when transitioning into this specific demonstration scope. */
	virtual void Enter(FDemoContext& InContext);

	/** Execution tick evaluated once per frame to update underlying gameplay/math logic. */
	virtual void Tick(float InDeltaTime, FDemoContext& InContext) = 0;

	/** Input events handling pipeline dispatch hook. */
	virtual void HandleEvent(const sf::Event& InEvent, FDemoContext& InContext) {}

	/** Custom rendering callback dedicated to scene overlay structures (rays, visual debug fields). */
	virtual void Render(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix) {}

	/** Triggered once immediately when swapping out from this active demonstration scope. */
	virtual void Exit(FDemoContext& InContext) {}

	/** Returns a unique name for the demo mode to be displayed in the HUD */
	virtual std::string GetDemoName() const = 0;
};