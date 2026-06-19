#pragma once

#include "../Math/FVector2D.h"
#include "../Components/AActor.h"

struct FTransform2D;
class UWorld;
class USquareRenderComponent;
class UCircleRenderComponent;
class URenderComponent;

namespace sf
{
	class Font;
}

/**
 * Global data context passed down to individual math demonstration modes.
 * Contains shared registries, global scene component hooks, and viewport input states.
 */
struct FDemoContext
{
public:
	FDemoContext(UWorld* InWorld);

public:
	USquareRenderComponent* MainSquare = nullptr;
	UCircleRenderComponent* MainCircle = nullptr;
	UCircleRenderComponent* DirectionIndicator = nullptr;
	FTransform2D* CameraTransform = nullptr;

	FVector2D MousePosition;
	FVector2D InputDirection;

	UWorld* World = nullptr;

	const sf::Font* Font = nullptr;
};