#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include "UWorld.h"
#include "../Core/FDemoContext.h"
#include "../Demos/IMathDemo.h"
#include "../Math/FCamera.h"

class UCollisionComponent;

/**
 * Identifies which interactive math demo is currently active.
 */
enum class EMathDemo : int8_t
{
	EMD_DotProduct,
	EMD_CrossProduct,
	EMD_Steering,
	EMD_RotationInertia,
	EMD_FieldOfView,
	EMD_Reflection,
	EMD_Transform,
	EMD_CameraTransform,
	EMD_Gravity,
	EMD_Spring,
	EMD_Boids,
	EMD_None
};

/**
 * Top-level application controller.
 * Owns the world and the demo registry, and drives input handling, updates,
 * camera, demo switching, and rendering each frame.
 */
class GameMain
{
public:
	/** Spawns the base actors and wires up their transform hierarchy. */
	GameMain();

	/** Loads resources and sets up default state. Throws if a critical file is missing. */
	void InitGame();

	/** Per-frame update: processes input and advances the active demo. */
	void Update(const sf::RenderWindow& InWindow, float InDeltaTime);

	/** Draws the world and the on-screen demo UI. */
	void Render(sf::RenderWindow& InWindow);

	/** Switches to a new demo, tearing down the old one and initializing the new. */
	void ChangeMode(EMathDemo InNewMode);

	/** Forwards an OS input event to the active demo. */
	void HandleEvent(const sf::Event& InEvent);

	/** Returns the camera's transform (mutable). */
	FTransform2D& GetCameraTransform() { return CameraComponent->TransformComponent; }

	/** Returns the currently active demo mode. */
	EMathDemo GetCurrentMathMode() const { return CurrentMathMode; }

private:
	/** Reads keyboard state and builds the movement input direction. */
	void HandleInput();

	/** Cycles between demos on edge-triggered arrow key presses. */
	void HandleInputSwitchMode();

	/** Moves the main square, including the steering "arrival" behavior. */
	void Movement(float InDeltaTime);

private:
	/** The scene: owns all actors and the render list. */
	UWorld World;

	/** Cached non-owning pointers to the persistent demo actors' components. */
	USquareRenderComponent* MainSquareComponent = nullptr;
	UCircleRenderComponent* MainCircleComponent = nullptr;
	UCircleRenderComponent* DirectionIndicatorComponent = nullptr;
	UPhysicsComponent* MainSquarePhysicsComponent = nullptr;

	/** Collision volume attached to the player square actor. */
	UCollisionComponent* MainSquareCollisionComponent = nullptr;

	/** Collision volume attached to the static/target circle actor. */
	UCollisionComponent* MainCircleCollisionComponent = nullptr;

	/** Camera controller converting world space into screen/view space. */
	std::unique_ptr<FCamera> CameraComponent = nullptr;

	/** Edge-trigger flags so arrow keys switch demos once per press, not per frame. */
	bool bLeftArrowPressed = false;
	bool bRightArrowPressed = false;

	/** Active demo mode and the registry of available demos. */
	EMathDemo CurrentMathMode = EMathDemo::EMD_None;
	FDemoContext GlobalContext;
	std::unordered_map<EMathDemo, std::unique_ptr<IMathDemo>> DemoRegistry;
	IMathDemo* ActiveDemo = nullptr;

	/** Cursor position and current movement input direction. */
	FVector2D MousePosition;
	FVector2D InputDirection;

	/** Font used for on-screen text. */
	sf::Font Font;

	/** FPS counter state for the on-screen performance readout. */
	int32_t CurrentFps = 0;
	float FpsCounter = 0.0f;
	float FpsUpdateTimer = 0.0f;
	const float FpsUpdateInterval = 0.05f;
	sf::Text FpsText;
};