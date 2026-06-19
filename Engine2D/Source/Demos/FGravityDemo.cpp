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
	SunComponent = ActorSun->CreateComponent<UCircleRenderComponent>(SunRadius);
	ActorSun->SetActorLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	SunComponent->SetColor(sf::Color::Yellow);

	// --- Earth: orbits the Sun ---
	AActor* ActorEarth = InContext.World->SpawnActor("EarthActor");
	EarthComponent = ActorEarth->CreateComponent<UCircleRenderComponent>(EarthRadius);
	ActorEarth->SetActorLocation({ SunComponent->GetComponentLocation().X - 150.0f, SunComponent->GetComponentLocation().Y - 50.0f });
	EarthComponent->SetColor(sf::Color::Cyan);

	EarthPhysicsComponent = ActorEarth->CreateComponent<UPhysicsComponent>();
	EarthPhysicsComponent->SetLinearDamping(1.0f);	// 1.0 = no drag; drag would bleed orbital energy
	EarthPhysicsComponent->SetMass(EarthMass);

	// Circular-orbit velocity around the Sun: speed = sqrt(G*M/r), aimed perpendicular to the Sun-ward direction.
	const FVector2D EarthToSun = SunComponent->GetComponentLocation() - EarthComponent->GetComponentLocation();
	const float EarthSunDistance = EarthToSun.Length();
	const FVector2D EarthOrbitDirection = FVector2D(-EarthToSun.Y, EarthToSun.X).Normalized();	// 90-degree rotation
	const float EarthOrbitSpeed = std::sqrt(G * SunMass / EarthSunDistance);
	EarthPhysicsComponent->SetVelocity(EarthOrbitDirection * EarthOrbitSpeed);

	// --- Moon: orbits the Earth ---
	AActor* ActorMoon = InContext.World->SpawnActor("MoonActor");
	MoonComponent = ActorMoon->CreateComponent<UCircleRenderComponent>(MoonRadius);
	ActorMoon->SetActorLocation({ EarthComponent->GetComponentLocation().X - 12.0f, EarthComponent->GetComponentLocation().Y });
	MoonComponent->SetColor(GRAY_COLOR);

	MoonPhysicsComponent = ActorMoon->CreateComponent<UPhysicsComponent>();
	MoonPhysicsComponent->SetLinearDamping(1.0f);
	MoonPhysicsComponent->SetMass(MoonMass);

	// --- Jupiter: orbits the Sun on a wider radius than the Earth ---
	AActor* ActorJupiter = InContext.World->SpawnActor("JupiterActor");
	JupiterComponent = ActorJupiter->CreateComponent<UCircleRenderComponent>(JupiterRadius);
	ActorJupiter->SetActorLocation({ SunComponent->GetComponentLocation().X + 350.0f, SunComponent->GetComponentLocation().Y });
	JupiterComponent->SetColor(GRAY_COLOR);

	JupiterPhysicsComponent = ActorJupiter->CreateComponent<UPhysicsComponent>();
	JupiterPhysicsComponent->SetLinearDamping(1.0f);
	JupiterPhysicsComponent->SetMass(JupiterMass);

	// Same circular-orbit setup as the Earth: without this Jupiter would fall straight into the Sun.
	const FVector2D JupiterToSun = SunComponent->GetComponentLocation() - JupiterComponent->GetComponentLocation();
	const float JupiterSunDistance = JupiterToSun.Length();
	const FVector2D JupiterOrbitDirection = FVector2D(-JupiterToSun.Y, JupiterToSun.X).Normalized();
	const float JupiterOrbitSpeed = std::sqrt(G * (SunMass + EarthMass) / JupiterSunDistance);
	JupiterPhysicsComponent->SetVelocity(JupiterOrbitDirection * JupiterOrbitSpeed);

	// The Moon orbits a MOVING Earth, so its world velocity is Earth's velocity
	// plus the local orbital velocity around the Earth.
	const FVector2D MoonToEarth = EarthComponent->GetComponentLocation() - MoonComponent->GetComponentLocation();
	const float MoonEarthDistance = MoonToEarth.Length();
	const FVector2D MoonOrbitDirection = FVector2D(-MoonToEarth.Y, MoonToEarth.X).Normalized();
	const float MoonOrbitSpeed = std::sqrt(G * EarthMass / MoonEarthDistance);
	MoonPhysicsComponent->SetVelocity(EarthPhysicsComponent->GetVelocity() + MoonOrbitDirection * MoonOrbitSpeed);

	ActorSun->RegisterAllComponents();
	ActorEarth->RegisterAllComponents();
	ActorMoon->RegisterAllComponents();
	ActorJupiter->RegisterAllComponents();

	DemoActors.push_back(ActorSun);
	DemoActors.push_back(ActorEarth);
	DemoActors.push_back(ActorMoon);
	DemoActors.push_back(ActorJupiter);
}

void FGravityDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	// Each moving body's net force is the vector sum of the pull from every other body
	// (superposition). The static Sun is a force source only - it never moves itself.
	// UPhysicsComponent integrates the accumulated forces into motion on its own tick.

	// --- Net force on the Earth: pulled by the Sun and by Jupiter ---
	const FVector2D SunPullEarth = ComputeGravitationalForce(SunComponent->GetComponentLocation(), SunMass,				 //
															 EarthComponent->GetComponentLocation(), EarthMass);

	const FVector2D JupiterPullEarth = ComputeGravitationalForce(JupiterComponent->GetComponentLocation(), JupiterMass,	 //
																 EarthComponent->GetComponentLocation(), EarthMass);

	EarthPhysicsComponent->AddForce(SunPullEarth + JupiterPullEarth);

	// --- Net force on the Moon: superposition of the Earth, the Sun and Jupiter ---
	const FVector2D EarthPullMoon = ComputeGravitationalForce(EarthComponent->GetComponentLocation(), EarthMass,		//
															  MoonComponent->GetComponentLocation(), MoonMass);

	const FVector2D SunPullMoon = ComputeGravitationalForce(SunComponent->GetComponentLocation(), SunMass,				//
															MoonComponent->GetComponentLocation(), MoonMass);

	const FVector2D JupiterPullMoon = ComputeGravitationalForce(JupiterComponent->GetComponentLocation(), JupiterMass,	//
																MoonComponent->GetComponentLocation(), MoonMass);

	MoonPhysicsComponent->AddForce(EarthPullMoon + SunPullMoon + JupiterPullMoon);

	// --- Net force on Jupiter: pulled by the Sun and by the Earth ---
	const FVector2D SunPullJupiter = ComputeGravitationalForce(SunComponent->GetComponentLocation(), SunMass,		 //
															   JupiterComponent->GetComponentLocation(), JupiterMass);

	const FVector2D EarthPullJupiter = ComputeGravitationalForce(EarthComponent->GetComponentLocation(), EarthMass,	 //
																 JupiterComponent->GetComponentLocation(), JupiterMass);

	JupiterPhysicsComponent->AddForce(SunPullJupiter + EarthPullJupiter);
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
	// Destroy every body this demo spawned (Sun, Earth, Moon, Jupiter).
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