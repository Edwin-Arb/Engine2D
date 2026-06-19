#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "../Base/URenderComponent.h"

/** Default fall-back rendering color value for empty square initialization procedures. */
constexpr sf::Color GRAY_COLOR = sf::Color(140, 140, 140);

/**
 * Component optimized for rendering rectangular geometry configurations.
 */
class USquareRenderComponent : public URenderComponent
{
public:
	/** Prepares internal structural dimensions for rendering operations. */
	USquareRenderComponent(const FVector2D& InSize);

	/**
	 * Linearly interpolates the component's visual characteristics between red and green.
	 * @param InValue Unclamped scale tracking criteria factor (ideally maps between -1.0 and 1.0).
	 */
	void SetColorFromFactor(float InValue);

	/** Sets explicit fallback color configuration values onto the active primitive structure. */
	void SetColor(sf::Color NewColor) override;

protected:
	/** Draws the layout structure inside the parameters pipeline out into the target window. */
	void DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates) override;

private:
	/** Core SFML square configuration representation object. */
	sf::RectangleShape RectShape;
};