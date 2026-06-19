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

	/** Initializes the circle primitive state configuration. */
	UCircleRenderComponent(float InRadius);

	/** Overrides interface color properties. */
	void SetColor(sf::Color NewColor) override;

protected:
	/** Renders the localized circle configuration out through the custom pipeline states. */
	void DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates) override;

private:
	/** Core SFML geometric resource definition object. */
	sf::CircleShape CircleShape;
};