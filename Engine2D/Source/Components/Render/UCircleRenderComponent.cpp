#include "UCircleRenderComponent.h"

UCircleRenderComponent::UCircleRenderComponent(float InRadius)
{
	CircleShape.setRadius(InRadius);

	// Center the origin so the circle is positioned by its center, not its top-left corner.
	CircleShape.setOrigin({ InRadius, InRadius });
	CircleShape.setFillColor(sf::Color::Green);
}

void UCircleRenderComponent::SetColor(sf::Color NewColor)
{
	CircleShape.setFillColor(NewColor);
}

void UCircleRenderComponent::DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates)
{
	// Clear SFML's own transform: positioning is driven entirely by the engine matrix in InStates.
	CircleShape.setPosition({ 0.0f, 0.0f });
	CircleShape.setRotation(sf::degrees(0.0f));
	CircleShape.setScale({ 1.0f, 1.0f });

	InWindow.draw(CircleShape, InStates);
}