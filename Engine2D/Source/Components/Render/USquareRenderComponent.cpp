#include "USquareRenderComponent.h"
#include <algorithm>

USquareRenderComponent::USquareRenderComponent(const FVector2D& InSize)
{
	RectShape.setSize(InSize);

	// Pivot assignment configured right to the center of the bounding box bounds
	RectShape.setOrigin({ InSize.X * 0.5f, InSize.Y * 0.5f });
	RectShape.setFillColor(GRAY_COLOR);
}

void USquareRenderComponent::SetColorFromFactor(float InValue)
{
	constexpr float Offset = 1.0f;
	constexpr float Scale = 0.5f;

	// Normalize the incoming dynamic scale constraint criteria safely into a 0.0 - 1.0 alpha range
	const float TimelineResult = std::clamp((InValue + Offset) * Scale, 0.0f, 1.0f);

	const uint8_t Red = static_cast<uint8_t>(FVector2D::Lerp(255.0f, 0.0f, TimelineResult));
	const uint8_t Green = static_cast<uint8_t>(FVector2D::Lerp(0.0f, 255.0f, TimelineResult));

	RectShape.setFillColor(sf::Color(Red, Green, 100));
}

void USquareRenderComponent::SetColor(sf::Color NewColor)
{
	RectShape.setFillColor(NewColor);
}

void USquareRenderComponent::DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates)
{
	// Disregard localized SFML properties in favor of the custom engine world matrix pipeline
	RectShape.setPosition({ 0.0f, 0.0f });
	RectShape.setRotation(sf::degrees(0.0f));
	RectShape.setScale({ 1.0f, 1.0f });

	InWindow.draw(RectShape, InStates);
}