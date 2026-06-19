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
	/** Main controllable square actor's render component. */
	USquareRenderComponent* MainSquare = nullptr;

	/** Target/marker circle's render component. */
	UCircleRenderComponent* MainCircle = nullptr;

	/** Small circle used to visualize a direction or heading. */
	UCircleRenderComponent* DirectionIndicator = nullptr;

	/** Shared camera transform applied to the scene. */
	FTransform2D* CameraTransform = nullptr;

	/** Cursor position in world space, updated each frame. */
	FVector2D MousePosition;

	/** Current input direction derived from keyboard/mouse. */
	FVector2D InputDirection;

	/** World that owns the demo's actors (non-owning). */
	UWorld* World = nullptr;

	/** Font used to draw on-screen demo text (non-owning). */
	const sf::Font* Font = nullptr;
};