#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include "../Base/URenderComponent.h"

/**
 * Component optimized for rendering standard 2D circular shapes via SFML wrapper bindings.
 */
class UCircleRenderComponent : public URenderComponent
{
public:
	UCircleRenderComponent() = delete;

	/** Constructs a circle with the given radius. */
	UCircleRenderComponent(float InRadius);

	/** Sets the circle's fill color. */
	void SetColor(sf::Color NewColor) override;

protected:
	/** Draws the circle using the engine-provided render states. */
	void DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates) override;

private:
	/** Underlying SFML circle shape. */
	sf::CircleShape CircleShape;
};