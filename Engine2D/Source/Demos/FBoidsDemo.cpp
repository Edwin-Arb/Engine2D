#include "FBoidsDemo.h"

#include "../Components/AActor.h"
#include "../Components/Base/UPhysicsComponent.h"
#include "../Components/Render/UCircleRenderComponent.h"
#include "../Core/FDemoContext.h"
#include "../Core/UWorld.h"
#include "../Math/FMath.h"

void FBoidsDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

	// Hide the persistent world scene so only the boids are on screen.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(false);
		}
	}

	// Spawn the flock: each boid is an actor with a circle to draw and a physics body to move it.
	for (int32_t i = 0; i < BoidCount; ++i)
	{
		// Random spawn point, kept a little away from the screen edges.
		const float RandomX = FMath::FRandRange(BoidRadius * 2.0f, SCREEN_WIDTH - BoidRadius * 2.0f);
		const float RandomY = FMath::FRandRange(BoidRadius * 2.0f, SCREEN_HEIGHT - BoidRadius * 2.0f);

		// Random warm colour so individual boids are easy to tell apart.
		const uint8_t RandomRed = static_cast<uint8_t>(FMath::RandRange(100, 255));
		const uint8_t RandomGreen = static_cast<uint8_t>(FMath::RandRange(100, 255));

		AActor* Actor = InContext.World->SpawnActor("BoidActor");

		UCircleRenderComponent* CircleRenderComp = Actor->CreateComponent<UCircleRenderComponent>(BoidRadius);
		CircleRenderComp->SetWorldLocation({ RandomX, RandomY });
		CircleRenderComp->SetColor(sf::Color(RandomRed, RandomGreen, 0));

		UPhysicsComponent* PhysicsComp = Actor->CreateComponent<UPhysicsComponent>();
		PhysicsComp->SetLinearDamping(1.0f);  // no drag; speed is capped manually in Tick
		PhysicsComp->SetMass(1.0f);

		// Random starting heading at full speed, so the flock is alive from frame one.
		const FVector2D RandomHeading(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f));
		PhysicsComp->SetVelocity(RandomHeading.Normalized() * BoidMaxSpeed);

		Actor->RegisterAllComponents();

		BoidPhysicsComponents.push_back(PhysicsComp);
		DemoActors.push_back(Actor);
	}
}

void FBoidsDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	const int32_t BoidSum = static_cast<int32_t>(DemoActors.size());

	// Each boid steers itself from a snapshot of the others taken this frame.
	for (int32_t i = 0; i < BoidSum; ++i)
	{
		const FVector2D MyLocation = DemoActors[i]->GetActorLocation();

		// Three accumulators, filled by scanning every other boid once below.
		FVector2D PositionSum(0.0f, 0.0f);	   // COHESION  - sum of neighbour POSITIONS
		int32_t CohesionCount = 0;

		FVector2D VelocitySum(0.0f, 0.0f);	   // ALIGNMENT - sum of neighbour VELOCITIES
		int32_t AlignmentCount = 0;

		FVector2D SeparationPush(0.0f, 0.0f);  // SEPARATION - sum of "away" pushes

		for (int32_t j = 0; j < BoidSum; ++j)
		{
			if (i == j)
			{
				continue;
			}

			const FVector2D OtherLocation = DemoActors[j]->GetActorLocation();
			const float Distance = FVector2D::Distance(MyLocation, OtherLocation);

			// COHESION: remember WHERE nearby flockmates are.
			if (Distance < CohesionRadius)
			{
				PositionSum += OtherLocation;
				++CohesionCount;
			}

			// ALIGNMENT: remember WHERE nearby flockmates are heading.
			if (Distance < AlignmentRadius)
			{
				VelocitySum += BoidPhysicsComponents[j]->GetVelocity();
				++AlignmentCount;
			}

			// SEPARATION: push AWAY from anyone too close - the closer, the stronger.
			if (Distance < SeparationRadius && Distance > EPSILON)
			{
				const FVector2D Away = FVector2D::Direction(OtherLocation, MyLocation);	// unit vector pointing away
				SeparationPush += Away / Distance;										   // divide by distance: closer => bigger push
			}
		}

		FVector2D Steering(0.0f, 0.0f);

		// COHESION: the average position is a POINT (the flock's centre).
		// Turn a point into a direction with "Centre - Me".
		if (CohesionCount > 0)
		{
			const FVector2D Center = PositionSum / static_cast<float>(CohesionCount);
			const FVector2D ToCenter = FVector2D::Direction(MyLocation, Center);
			Steering += ToCenter * CohesionSpeed;
		}

		// ALIGNMENT: the average velocity is ALREADY a direction (where the group
		// heads), so there is NO "minus me" here - just steer along it.
		if (AlignmentCount > 0)
		{
			const FVector2D AverageHeading = VelocitySum / static_cast<float>(AlignmentCount);
			Steering += AverageHeading.Normalized() * AlignmentSpeed;
		}

		// SEPARATION: the summed push is already a direction away from the crowd.
		Steering += SeparationPush.Normalized() * SeparationSpeed;

		// The combined steering is a force; the physics component integrates it into motion.
		BoidPhysicsComponents[i]->AddForce(Steering);

		// --- Housekeeping (not a flocking rule) ---

		// Cap the speed so boids do not accelerate forever (there is no drag).
		const FVector2D Velocity = BoidPhysicsComponents[i]->GetVelocity();
		if (Velocity.Length() > BoidMaxSpeed)
		{
			BoidPhysicsComponents[i]->SetVelocity(Velocity.Normalized() * BoidMaxSpeed);
		}

		// Wrap around the screen edges so the flock always stays visible (toroidal world).
		FVector2D Position = DemoActors[i]->GetActorLocation();
		if (Position.X < 0.0f)
		{
			Position.X += SCREEN_WIDTH;
		}
		else if (Position.X > SCREEN_WIDTH)
		{
			Position.X -= SCREEN_WIDTH;
		}
		if (Position.Y < 0.0f)
		{
			Position.Y += SCREEN_HEIGHT;
		}
		else if (Position.Y > SCREEN_HEIGHT)
		{
			Position.Y -= SCREEN_HEIGHT;
		}
		DemoActors[i]->SetActorLocation(Position);
	}
}

void FBoidsDemo::Exit(FDemoContext& InContext)
{
	// Destroy every body this demo spawned.
	for (AActor* DemoActor : DemoActors)
	{
		InContext.World->DestroyActor(DemoActor);
	}

	DemoActors.clear();
	BoidPhysicsComponents.clear();

	// Restore the persistent world scene we hid on Enter.
	for (URenderComponent* RenderComp : InContext.World->GetRenderComponents())
	{
		if (RenderComp)
		{
			RenderComp->SetVisibility(true);
		}
	}
}
