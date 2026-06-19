#include "FSpringDemo.h"

#include <SFML/Window/Mouse.hpp>

#include "../Components/AActor.h"
#include "../Components/Base/UPhysicsComponent.h"
#include "../Components/Render/UCircleRenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"

void FSpringDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

	// Hide the persistent world scene so only the spring bodies are on screen.
	// Exit restores visibility once our bodies are torn down.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(false);
		}
	}

	// --- Anchor: the fixed point the spring pulls toward (static, no physics) ---
	AActor* AnchorActor = InContext.World->SpawnActor("SpringAnchorActor");
	AnchorComponent = AnchorActor->CreateComponent<UCircleRenderComponent>(AnchorRadius);
	AnchorComponent->SetWorldLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	AnchorComponent->SetColor(sf::Color::Green);

	// --- Cargo: the free mass on the end of the spring (starts stretched, above the anchor) ---
	AActor* CargoActor = InContext.World->SpawnActor("SpringCargoActor");
	CargoComponent = CargoActor->CreateComponent<UCircleRenderComponent>(CargoRadius);
	CargoComponent->SetWorldLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.2f });
	CargoComponent->SetColor(sf::Color::Yellow);

	CargoPhysicsComponent = CargoActor->CreateComponent<UPhysicsComponent>();
	CargoPhysicsComponent->SetLinearDamping(1.0f);	// no global drag; damping is modeled explicitly in Tick
	CargoPhysicsComponent->SetMass(CargoMass);

	AnchorActor->RegisterAllComponents();
	CargoActor->RegisterAllComponents();

	DemoActors.push_back(AnchorActor);
	DemoActors.push_back(CargoActor);
}

void FSpringDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// --- Mouse interaction: hold the left button to grab and drag the cargo ---
	const FVector2D MouseLocation = InContext.MousePosition;
	const bool bMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	if (bIsGrabbed)
	{
		if (!bMouseDown)
		{
			bIsGrabbed = false;	 // released - hand control back to the spring
		}
		else
		{
			// Drag kinematically: pin the cargo to the cursor and clear its motion while held.
			CargoComponent->SetWorldLocation(MouseLocation);
			CargoPhysicsComponent->SetVelocity({ 0.0f, 0.0f });
			return;
		}
	}
	else if (bMouseDown)
	{
		// Begin a grab only if the click landed on the cargo disc.
		const float ClickDistance = (MouseLocation - CargoComponent->GetComponentLocation()).Length();
		if (ClickDistance <= CargoRadius)
		{
			bIsGrabbed = true;
			return;
		}
	}

	// --- Hooke's law: restoring force F = -k * (currentLength - restLength) along the spring axis ---
	const FVector2D Offset = CargoComponent->GetComponentLocation() - AnchorComponent->GetComponentLocation();
	const float CurrentLength = Offset.Length();
	if (CurrentLength < EPSILON)
	{
		return;											   // cargo sits exactly on the anchor; spring direction is undefined this frame
	}

	const FVector2D AlongSpring = Offset / CurrentLength;  // unit vector, anchor -> cargo
	const float Stretch = CurrentLength - RestLength;	   // >0 stretched, <0 compressed
	const FVector2D SpringForce = AlongSpring * (-Stiffness * Stretch);

	// --- Damping force opposes velocity so the oscillation decays instead of ringing forever ---
	const FVector2D DampingForce = CargoPhysicsComponent->GetVelocity() * -DampingCoefficient;

	CargoPhysicsComponent->AddForce(SpringForce + DampingForce);
}

void FSpringDemo::Exit(FDemoContext& InContext)
{
	// Destroy every body this demo spawned.
	for (AActor* Actor : DemoActors)
	{
		InContext.World->DestroyActor(Actor);
	}

	DemoActors.clear();
	bIsGrabbed = false;

	// Restore the persistent world scene we hid on Enter.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(true);
		}
	}
}
