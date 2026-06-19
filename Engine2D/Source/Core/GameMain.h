#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include "UWorld.h"
#include "../Core/FDemoContext.h"
#include "../Demos/IMathDemo.h"
#include "../Math/FCamera.h"

class UCollisionComponent;
/**
 * Mathematical calculation modes utilized during active interactive demonstrations.
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
	EMD_None
};

/**
 * Main application state controller sub-system.
 * Acts as the master orchestrator handling inputs dispatch, layout simulation updates,
 * camera viewport resolution, state machine switches, and rendering pass generation.
 */
class GameMain
{
public:
	/** Allocates base geometry and configures direct relative transformation tree constraints. */
	GameMain();

	/** Loads external resources and maps default UI context bindings. Throws if critical files are missing. */
	void InitGame();

	/** Core simulation iteration frame step evaluating inputs and active state behaviors. */
	void Update(const sf::RenderWindow& InWindow, float InDeltaTime);

	/** Master rendering command pass routing draw operations down onto the screen viewport. */
	void Render(sf::RenderWindow& InWindow);

	/** Manages structural lifetime teardowns and initialization states for newly selected modes. */
	void ChangeMode(EMathDemo InNewMode);

	/** Forwards raw OS-level hardware interactions down into active localized demo handlers. */
	void HandleEvent(const sf::Event& InEvent);

	/** Accessor returning mutable references back to the tracking viewport component transform node. */
	FTransform2D& GetCameraTransform() { return CameraComponent->TransformComponent; }

	/** Accessor tracking the active operational state profile. */
	EMathDemo GetCurrentMathMode() const { return CurrentMathMode; }

private:
	/** Polls keyboard states mapping axis thresholds onto displacement vectors. */
	void HandleInput();

	/** Tracks edge-triggered keystrokes to cycle cleanly between distinct execution profiles. */
	void HandleInputSwitchMode();

	/** Resolves algorithmic movement translation pipelines including adaptive Arrival target steering. */
	void Movement(float InDeltaTime);

private:
	UWorld World;

	/** Fast-access cache tracking persistent object primitives. */
	USquareRenderComponent* MainSquareComponent = nullptr;
	UCircleRenderComponent* MainCircleComponent = nullptr;
	UCircleRenderComponent* DirectionIndicatorComponent = nullptr;
	UPhysicsComponent* MainSquarePhysicsComponent = nullptr;

	/** Collision volume attached to the player square actor. */
	UCollisionComponent* MainSquareCollisionComponent = nullptr;

	/** Collision volume attached to the static/target circle actor. */
	UCollisionComponent* MainCircleCollisionComponent = nullptr;

	/** Viewport coordinate spaces conversion camera controller component tracking. */
	std::unique_ptr<FCamera> CameraComponent = nullptr;

	/** Edge-triggered validation flags tracking sequential mode selection shifts. */
	bool bLeftArrowPressed = false;
	bool bRightArrowPressed = false;

	/** State Machine Context properties. */
	EMathDemo CurrentMathMode = EMathDemo::EMD_None;
	FDemoContext GlobalContext;
	std::unordered_map<EMathDemo, std::unique_ptr<IMathDemo>> DemoRegistry;
	IMathDemo* ActiveDemo = nullptr;

	/** Active structural screen location tracking constraints properties. */
	FVector2D MousePosition;
	FVector2D InputDirection;

	/** Internal asset typography resources. */
	sf::Font Font;

	/** Diagnostic performance monitoring telemetry analytics tracking. */
	int32_t CurrentFps = 0;
	float FpsCounter = 0.0f;
	float FpsUpdateTimer = 0.0f;
	const float FpsUpdateInterval = 0.05f;
	sf::Text FpsText;
};