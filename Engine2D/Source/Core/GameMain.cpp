#include "GameMain.h"

#include "../Math/FMath.h"
#include "../Components/AActor.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Base/UPhysicsComponent.h"
#include "../Components/Collisions/UCircleCollisionComponent.h"

#include "../Demos/FDotProductDemo.h"
#include "../Demos/FCrossProductDemo.h"
#include "../Demos/FSteeringDemo.h"
#include "../Demos/FRotationInertiaDemo.h"
#include "../Demos/FFieldOfViewDemo.h"
#include "../Demos/FReflectionDemo.h"
#include "../Demos/FTransformDemo.h"
#include "../Demos/FCameraTransformDemo.h"
#include "../Demos/FGravityDemo.h"
#include "../Demos/FSpringDemo.h"
#include "../Demos/FBoidsDemo.h"
#include "../Demos/FLinesDemo.h"
#include "../Demos/FMatrixDemo.h"
#include "../Demos/FStartDemo.h"

GameMain::GameMain()
	: GlobalContext(&World)
	, FpsText(Font)
{
	// ============================================================================
	// 1. Player
	// ============================================================================
	// Spawn the player square and center it on screen.
	AActor* PlayerActor = World.SpawnActor<AActor>("PlayerSquareActor");
	MainSquareComponent = PlayerActor->CreateComponent<USquareRenderComponent>(FVector2D(SQUARE_SIZE_X, SQUARE_SIZE_Y));
	PlayerActor->SetActorLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });

	// Give the player physics (mass + damping) so it responds to forces.
	MainSquarePhysicsComponent = PlayerActor->CreateComponent<UPhysicsComponent>();
	MainSquarePhysicsComponent->SetMass(5.0f);
	MainSquarePhysicsComponent->SetLinearDamping(0.3f);

	// Add a collider and attach it to the square so it follows the visual.
	MainSquareCollisionComponent = PlayerActor->CreateComponent<UCircleCollisionComponent>(SQUARE_SIZE_X * 0.5f);

	// Zero the relative offset so the collider stays centered on the square.
	MainSquareCollisionComponent->SetRelativeLocation({ 0.0f, 0.0f });
	MainSquareCollisionComponent->AttachToComponent(MainSquareComponent);

	// ============================================================================
	// 2. Static target ring
	// ============================================================================
	// Spawn the target circle at the same center position.
	AActor* MainCir = World.SpawnActor<AActor>("MainCircleActor");
	MainCircleComponent = MainCir->CreateComponent<UCircleRenderComponent>(CIRCLE_RADIUS);
	MainCircleComponent->SetWorldLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });

	MainCircleCollisionComponent = MainCir->CreateComponent<UCircleCollisionComponent>(CIRCLE_RADIUS);

	// Attach the circle's collider to its visual component.
	MainCircleCollisionComponent->SetRelativeLocation({ 0.0f, 0.0f });
	MainCircleCollisionComponent->AttachToComponent(MainCircleComponent);

	// ============================================================================
	// 3. Direction indicator (child of the square)
	// ============================================================================
	// A small marker that shows which way the square is facing.
	AActor* IndicatorActor = World.SpawnActor<AActor>("DirectionIndicatorActor");
	DirectionIndicatorComponent = IndicatorActor->CreateComponent<UCircleRenderComponent>(SQUARE_SIZE_X * 0.05f);  // 5% of the main square size
	DirectionIndicatorComponent->SetColor(sf::Color::Yellow);

	// Parent it to the square so it rotates/moves with the square, offset to one side.
	DirectionIndicatorComponent->AttachToComponent(MainSquareComponent);
	constexpr float OffsetDistance = SQUARE_SIZE_X / 2.0f;
	DirectionIndicatorComponent->SetRelativeLocation({ OffsetDistance, 0.0f });

	// ============================================================================
	// 4. Camera
	// ============================================================================
	// Create the camera and center it on the scene.
	CameraComponent = std::make_unique<FCamera>();
	CameraComponent->TransformComponent.SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });

	// ============================================================================
	// 5. Demo registry
	// ============================================================================
	// Register one demo instance per math mode.
	DemoRegistry.emplace(EMathDemo::EMD_DotProduct, std::make_unique<FDotProductDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_CrossProduct, std::make_unique<FCrossProductDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Steering, std::make_unique<FSteeringDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_RotationInertia, std::make_unique<FRotationInertiaDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_FieldOfView, std::make_unique<FFieldOfViewDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Reflection, std::make_unique<FReflectionDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Transform, std::make_unique<FTransformDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_CameraTransform, std::make_unique<FCameraTransformDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Gravity, std::make_unique<FGravityDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Spring, std::make_unique<FSpringDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Boids, std::make_unique<FBoidsDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Lines, std::make_unique<FLinesDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_Matrix, std::make_unique<FMatrixDemo>());
	DemoRegistry.emplace(EMathDemo::EMD_None, std::make_unique<FStartDemo>());

	// Start on the "None"/start demo and cache its pointer as the active demo.
	CurrentMathMode = EMathDemo::EMD_None;
	ActiveDemo = DemoRegistry[EMathDemo::EMD_None].get();
}

void GameMain::InitGame()
{
	// The font is required for all on-screen text, so a load failure is fatal.
	if (!Font.openFromFile("Font/PFAgoraSlabPro Bold.ttf"))
	{
		throw std::runtime_error("Failed to load critical typography layout resource: Font/PFAgoraSlabPro Bold.ttf");
	}

	// Publish shared references into the context the demos read from.
	GlobalContext.Font = &Font;
	GlobalContext.MainSquare = MainSquareComponent;
	GlobalContext.MainCircle = MainCircleComponent;
	GlobalContext.DirectionIndicator = DirectionIndicatorComponent;
	GlobalContext.CameraTransform = &CameraComponent->TransformComponent;

	// Configure the on-screen FPS text.
	FpsText.setCharacterSize(20);
	FpsText.setFillColor(sf::Color::Green);
	FpsText.setPosition(FVector2D(10.0f, SCREEN_HEIGHT - 35.0f));
	FpsText.setString("FPS: 0");

	// Lifecycle ignition: register every actor's components with the world, THEN begin play.
	// Two separate passes so BeginPlay can safely query an already fully-registered world.
	World.RegisterAllActors();
	World.BeginPlay();
}

void GameMain::HandleInput()
{
	InputDirection.Set(0.0f, 0.0f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		InputDirection.X -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		InputDirection.X += 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		InputDirection.Y -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		InputDirection.Y += 1.0f;

	// Press R to instantly rotate the square to face the target circle.
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
	{
		const FVector2D SquarePos = MainSquareComponent->GetComponentLocation();
		const FVector2D CirclePos = MainCircleComponent->GetComponentLocation();

		const FVector2D RotationDirection = FVector2D::Direction(SquarePos, CirclePos);
		const float RadianAngleSquare = std::atan2(RotationDirection.Y, RotationDirection.X);
		MainSquareComponent->SetRelativeRotation(FMath::RadianToDegree(RadianAngleSquare));
	}
}

void GameMain::HandleInputSwitchMode()
{
	const bool bLeftArrowCurrentPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
	const bool bRightArrowCurrentPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

	// Edge-triggered: act only on the frame a key transitions from released to pressed.
	if (bLeftArrowCurrentPressed && !bLeftArrowPressed)
	{
		int32_t CurrentMode = static_cast<int32_t>(CurrentMathMode);
		CurrentMode--;

		if (CurrentMode < 0)
		{
			CurrentMode = static_cast<int32_t>(EMathDemo::EMD_None);
		}

		ChangeMode(static_cast<EMathDemo>(CurrentMode));
	}

	if (bRightArrowCurrentPressed && !bRightArrowPressed)
	{
		int32_t CurrentMode = static_cast<int32_t>(CurrentMathMode);
		CurrentMode++;

		if (CurrentMode >= static_cast<int32_t>(EMathDemo::EMD_None))
		{
			CurrentMode = 0;
			static_assert(static_cast<int>(EMathDemo::EMD_None) == 13);
		}

		ChangeMode(static_cast<EMathDemo>(CurrentMode));
	}

	bLeftArrowPressed = bLeftArrowCurrentPressed;
	bRightArrowPressed = bRightArrowCurrentPressed;
}

void GameMain::Movement(float InDeltaTime)
{
	if (!MainSquareComponent || !MainSquarePhysicsComponent)
	{
		return;
	}

	FVector2D SquarePos = MainSquareComponent->GetComponentLocation();
	FVector2D CirclePos = MainCircleComponent->GetComponentLocation();

	if (InputDirection.Length() > 0.0f)
	{
		// 1. Manual controls using WASD
		InputDirection = InputDirection.Normalized();
		float EngineThrust = 5000.0f;
		MainSquarePhysicsComponent->AddForce(InputDirection * EngineThrust);
	}
	else
	{
		// 2. Autopilot (Mouse or Space Bar)
		bool bHasTarget = false;
		FVector2D TargetPosition = SquarePos;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			bHasTarget = true;
			TargetPosition = CirclePos;
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			bHasTarget = true;
			TargetPosition = MousePosition;
		}

		if (bHasTarget)
		{
			FVector2D ToTarget = TargetPosition - SquarePos;
			float DistanceToTarget = ToTarget.Length();

			if (DistanceToTarget > EPSILON)
			{
				// Set the default force of the engines for movement toward the Mouse or Space Bar
				constexpr float AutopilotThrust = 20000.0f;

				FVector2D DriveDirection = ToTarget.Normalized();
				float ForceScale = 1.0f;

				// Arrival Mechanics: Gradually reduce the force when approaching closely
				if (DistanceToTarget <= SLOW_RADIUS)
				{
					ForceScale = DistanceToTarget / SLOW_RADIUS;
				}

				// Apply force toward the target
				MainSquarePhysicsComponent->AddForce(DriveDirection * (AutopilotThrust * ForceScale));
			}
		}
	}
}

void GameMain::Update(const sf::RenderWindow& InWindow, float InDeltaTime)
{
	// Read the mouse cursor position (screen space) and share it via the context.
	MousePosition = { static_cast<float>(sf::Mouse::getPosition(InWindow).x), static_cast<float>(sf::Mouse::getPosition(InWindow).y) };
	GlobalContext.MousePosition = MousePosition;

	// Read keyboard state into InputDirection and share it via the context.
	HandleInput();
	GlobalContext.InputDirection = InputDirection;

	// The camera-transform demo drives the square itself, so skip normal movement there.
	if (CurrentMathMode != EMathDemo::EMD_CameraTransform)
	{
		Movement(InDeltaTime);
	}

	HandleInputSwitchMode();

	// Advance every actor (and thus every component) in the world.
	World.Tick(InDeltaTime);

	// ===== Collision resolution: player square vs. target circle =================
	if (MainSquareCollisionComponent && MainCircleCollisionComponent)
	{
		FVector2D CollisionNormal;
		float PenetrationDepth = 0.0f;

		// Check if the player is intersecting with the target circle primitive
		if (MainSquareCollisionComponent->Intersects(MainCircleCollisionComponent, CollisionNormal, PenetrationDepth))
		{
			// 1. Positional Correction: Push the player out of the obstacle along the calculated normal
			FVector2D CorrectedPosition = MainSquareComponent->GetComponentLocation() + (CollisionNormal * PenetrationDepth);
			MainSquareComponent->SetWorldLocation(CorrectedPosition);

			// 2. Velocity Reflection (Elastic Impulse Response)
			if (MainSquarePhysicsComponent)
			{
				const FVector2D CurrentVelocity = MainSquarePhysicsComponent->GetVelocity();

				// Project current velocity vector onto the collision normal axis (Dot Product)
				const float ApproachSpeed = CurrentVelocity.DotProduct(CollisionNormal);

				// Only react if the actor is actively moving TOWARDS the collision surface
				if (ApproachSpeed < 0.0f)
				{
					// Coefficient of Restitution (Bounciness): 0.0f = slide, 1.0f = perfect bounce
					constexpr float Restitution = 0.8f;

					// Calculate the reflection impulse vector using the law of elastic impact
					const FVector2D ReflectionImpulse = CollisionNormal * (1.0f + Restitution) * ApproachSpeed;
					const FVector2D NewVelocity = CurrentVelocity - ReflectionImpulse;

					MainSquarePhysicsComponent->SetVelocity(NewVelocity);
				}
			}
		}
	}
	// ============================================================================

	// Keep the square inside the screen bounds (hard clamp).
	if (MainSquareComponent && MainSquarePhysicsComponent)
	{
		FVector2D Pos = MainSquareComponent->GetComponentLocation();
		bool bHitWall = false;

		constexpr float HalfW = SQUARE_SIZE_X * 0.5f;
		constexpr float HalfH = SQUARE_SIZE_Y * 0.5f;

		if (Pos.X < HalfW)
		{
			Pos.X = HalfW;
			bHitWall = true;
		}
		else if (Pos.X > SCREEN_WIDTH - HalfW)
		{
			Pos.X = SCREEN_WIDTH - HalfW;
			bHitWall = true;
		}

		if (Pos.Y < HalfH)
		{
			Pos.Y = HalfH;
			bHitWall = true;
		}
		else if (Pos.Y > SCREEN_HEIGHT - HalfH)
		{
			Pos.Y = SCREEN_HEIGHT - HalfH;
			bHitWall = true;
		}

		if (bHitWall)
		{
			MainSquareComponent->SetWorldLocation(Pos);
		}
	}

	if (ActiveDemo)
	{
		ActiveDemo->Tick(InDeltaTime, GlobalContext);
	}

	// Update the FPS readout, refreshing the displayed value every FpsUpdateInterval.
	FpsCounter += 1.0f;
	FpsUpdateTimer += InDeltaTime;

	if (FpsUpdateTimer >= FpsUpdateInterval)
	{
		CurrentFps = static_cast<int32_t>(FpsCounter / FpsUpdateInterval);
		FpsCounter = 0.0f;
		FpsUpdateTimer -= FpsUpdateInterval;

		FpsText.setString("FPS: " + std::to_string(CurrentFps));
	}
}

void GameMain::Render(sf::RenderWindow& InWindow)
{
	const FMatrix3x3 ViewMatrix = CameraComponent->GetViewMatrix();

	// 1. Draw every registered render component through the camera's view matrix.
	for (URenderComponent* RenderComp : World.GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->Draw(InWindow, ViewMatrix);
		}
	}

	// 2. Let the active demo draw its own overlays (rays, grids, cones) and its name.
	if (ActiveDemo)
	{
		ActiveDemo->Render(InWindow, ViewMatrix);

		sf::Text DemoNameText(Font, ActiveDemo->GetDemoName(), 20u);
		DemoNameText.setFillColor(sf::Color::White);
		DemoNameText.setPosition({ 10.0f, 10.0f });

		InWindow.draw(DemoNameText);
	}

	InWindow.draw(FpsText);
}

void GameMain::ChangeMode(EMathDemo InNewMode)
{
	// Already in this mode - nothing to switch.
	if (CurrentMathMode == InNewMode)
	{
		return;
	}

	// Reset the camera to its default pose so each demo starts from a clean view.
	CameraComponent->TransformComponent.SetPosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	CameraComponent->TransformComponent.SetRotation(0.0f);
	CameraComponent->TransformComponent.SetScale({ 1.0f, 1.0f });

	// Let the outgoing demo clean up before the new one takes over.
	if (ActiveDemo)
	{
		ActiveDemo->Exit(GlobalContext);
	}

	CurrentMathMode = InNewMode;

	auto It = DemoRegistry.find(CurrentMathMode);
	if (It != DemoRegistry.end())
	{
		ActiveDemo = It->second.get();
		ActiveDemo->Enter(GlobalContext);
	}
	else
	{
		ActiveDemo = nullptr;
	}
}

void GameMain::HandleEvent(const sf::Event& InEvent)
{
	if (ActiveDemo)
	{
		ActiveDemo->HandleEvent(InEvent, GlobalContext);
	}
}