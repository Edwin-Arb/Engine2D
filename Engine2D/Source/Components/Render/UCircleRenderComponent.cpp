#include "UCircleRenderComponent.h"

UCircleRenderComponent::UCircleRenderComponent(float InRadius)
{
	CircleShape.setRadius(InRadius);

	// Anchor the origin point right to the center of the geometric volume
	CircleShape.setOrigin({ InRadius, InRadius });
	CircleShape.setFillColor(sf::Color::Green);
}

void UCircleRenderComponent::SetColor(sf::Color NewColor)
{
	CircleShape.setFillColor(NewColor);
}

void UCircleRenderComponent::DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates)
{
	// Reset the structural identity transformations inside SFML to prevent matrix overlapping conflicts
	CircleShape.setPosition({ 0.0f, 0.0f });
	CircleShape.setRotation(sf::degrees(0.0f));
	CircleShape.setScale({ 1.0f, 1.0f });

	InWindow.draw(CircleShape, InStates);
}