#include "FGravityDemo.h"

#include "../Components/AActor.h"
#include "../Components/Base/UPhysicsComponent.h"
#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"

void FGravityDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

	// Hide the persistent world scene so only the gravity bodies are on screen.
	// Exit restores visibility once our bodies are torn down.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(false);
		}
	}

	// --- Sun: static central body, no physics, sits at screen center ---
	AActor* ActorSun = InContext.World->SpawnActor("SunActor");
	Sun = ActorSun->CreateComponent<UCircleRenderComponent>(SunRadius);
	ActorSun->SetActorLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	Sun->SetColor(sf::Color::Yellow);

	// --- Earth: orbits the Sun ---
	AActor* ActorEarth = InContext.World->SpawnActor("EarthActor");
	Earth = ActorEarth->CreateComponent<UCircleRenderComponent>(EarthRadius);
	ActorEarth->SetActorLocation({ Sun->GetComponentLocation().X - 370.0f, Sun->GetComponentLocation().Y - 50.0f });
	Earth->SetColor(sf::Color::Cyan);

	EarthPhysicsComponent = ActorEarth->CreateComponent<UPhysicsComponent>();
	EarthPhysicsComponent->SetLinearDamping(1.0f);	// 1.0 = no drag; drag would bleed orbital energy
	EarthPhysicsComponent->SetMass(EarthMass);

	// Circular-orbit velocity around the Sun: speed = sqrt(G*M/r), aimed perpendicular to the Sun-ward direction.
	const FVector2D EarthToSun = Sun->GetComponentLocation() - Earth->GetComponentLocation();
	const float SunDistance = EarthToSun.Length();
	const FVector2D EarthOrbitDirection = FVector2D(-EarthToSun.Y, EarthToSun.X).Normalized();	// 90-degree rotation
	const float EarthOrbitSpeed = std::sqrt(G * SunMass / SunDistance);
	EarthPhysicsComponent->SetVelocity(EarthOrbitDirection * EarthOrbitSpeed);

	// --- Moon: orbits the Earth ---
	AActor* ActorMoon = InContext.World->SpawnActor("MoonActor");
	Moon = ActorMoon->CreateComponent<UCircleRenderComponent>(MoonRadius);
	ActorMoon->SetActorLocation({ Earth->GetComponentLocation().X - 50.0f, Earth->GetComponentLocation().Y });
	Moon->SetColor(GRAY_COLOR);

	MoonPhysicsComponent = ActorMoon->CreateComponent<UPhysicsComponent>();
	MoonPhysicsComponent->SetLinearDamping(1.0f);
	MoonPhysicsComponent->SetMass(MoonMass);

	// The Moon orbits a MOVING Earth, so its world velocity is Earth's velocity
	// plus the local orbital velocity around the Earth.
	const FVector2D MoonToEarth = Earth->GetComponentLocation() - Moon->GetComponentLocation();
	const float EarthDistance = MoonToEarth.Length();
	const FVector2D MoonOrbitDirection = FVector2D(-MoonToEarth.Y, MoonToEarth.X).Normalized();
	const float MoonOrbitSpeed = std::sqrt(G * EarthMass / EarthDistance);
	MoonPhysicsComponent->SetVelocity(EarthPhysicsComponent->GetVelocity() + MoonOrbitDirection * MoonOrbitSpeed);

	ActorSun->RegisterAllComponents();
	ActorEarth->RegisterAllComponents();
	ActorMoon->RegisterAllComponents();

	DemoActors.push_back(ActorSun);
	DemoActors.push_back(ActorEarth);
	DemoActors.push_back(ActorMoon);
}

void FGravityDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// Earth is pulled only by the Sun.
	EarthPhysicsComponent->AddForce(									 //
		ComputeGravitationalForce(Sun->GetComponentLocation(), SunMass,	 //
								  Earth->GetComponentLocation(), EarthMass));

	// Superposition: the Moon feels BOTH the Earth and the Sun. The net force is the vector sum.
	const FVector2D PullFromEarth = ComputeGravitationalForce(Earth->GetComponentLocation(), EarthMass,	 //
															  Moon->GetComponentLocation(), MoonMass);

	const FVector2D PullFromSun = ComputeGravitationalForce(Sun->GetComponentLocation(), SunMass,		 //
															Moon->GetComponentLocation(), MoonMass);

	MoonPhysicsComponent->AddForce(PullFromEarth + PullFromSun);
}

FVector2D FGravityDemo::ComputeGravitationalForce(const FVector2D& SourceLocation, float SourceMass,  //
												  const FVector2D& TargetLocation, float TargetMass) const
{
	// Vector pointing from the target body toward the source (the pull direction).
	const FVector2D Offset = SourceLocation - TargetLocation;
	const float Distance = Offset.Length();

	// F = G * M * m / r^2 along the unit direction. Since dir = Offset / Distance,
	// multiplying by Offset and dividing by Distance^3 combines both in one step.
	return Offset * (G * SourceMass * TargetMass / (Distance * Distance * Distance));
}

void FGravityDemo::Exit(FDemoContext& InContext)
{
	for (AActor* ActorToDelete : DemoActors)
	{
		InContext.World->DestroyActor(ActorToDelete);
	}

	DemoActors.clear();

	// Restore the persistent world scene we hid on Enter.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(true);
		}
	}
}
