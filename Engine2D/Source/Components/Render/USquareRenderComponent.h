#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "../Base/URenderComponent.h"

/** Default fill color used when a square has no explicit color set. */
constexpr sf::Color GRAY_COLOR = sf::Color(140, 140, 140);

/**
 * Render component for drawing axis-aligned rectangles.
 */
class USquareRenderComponent : public URenderComponent
{
public:
	/** Constructs a rectangle of the given size. */
	USquareRenderComponent(const FVector2D& InSize);

	/**
	 * Interpolates the fill color between red and green based on a factor.
	 * @param InValue Input factor, expected in the [-1, 1] range (clamped internally).
	 */
	void SetColorFromFactor(float InValue);

	/** Sets the rectangle's fill color directly. */
	void SetColor(sf::Color NewColor) override;

protected:
	/** Draws the rectangle using the engine-provided render states. */
	void DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates) override;

private:
	/** Underlying SFML rectangle shape. */
	sf::RectangleShape RectShape;
};