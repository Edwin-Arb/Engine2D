#include "USquareRenderComponent.h"
#include <algorithm>

#include "../../Math/FMath.h"

USquareRenderComponent::USquareRenderComponent(const FVector2D& InSize)
{
	RectShape.setSize(InSize);

	// Center the origin so the rectangle is positioned by its center, not its top-left corner.
	RectShape.setOrigin({ InSize.X * 0.5f, InSize.Y * 0.5f });
	RectShape.setFillColor(GRAY_COLOR);
}

void USquareRenderComponent::SetColorFromFactor(float InValue)
{
	constexpr float Offset = 1.0f;
	constexpr float Scale = 0.5f;

	// Remap the input from [-1, 1] into a [0, 1] alpha used to blend the two colors.
	const float TimelineResult = std::clamp((InValue + Offset) * Scale, 0.0f, 1.0f);

	const uint8_t Red = static_cast<uint8_t>(FMath::Lerp(255.0f, 0.0f, TimelineResult));
	const uint8_t Green = static_cast<uint8_t>(FMath::Lerp(0.0f, 255.0f, TimelineResult));

	RectShape.setFillColor(sf::Color(Red, Green, 100));
}

void USquareRenderComponent::SetColor(sf::Color NewColor)
{
	RectShape.setFillColor(NewColor);
}

void USquareRenderComponent::DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates)
{
	// Clear SFML's own transform: positioning is driven entirely by the engine matrix in InStates.
	RectShape.setPosition({ 0.0f, 0.0f });
	RectShape.setRotation(sf::degrees(0.0f));
	RectShape.setScale({ 1.0f, 1.0f });

	InWindow.draw(RectShape, InStates);
}