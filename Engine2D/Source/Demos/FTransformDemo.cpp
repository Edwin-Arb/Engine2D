#include "FTransformDemo.h"

#include <algorithm>

#include "../Components/AActor.h"
#include "../Core/FDemoContext.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/UWorld.h"

void FTransformDemo::Enter(FDemoContext& InContext)
{
	IMathDemo::Enter(InContext);

	if (!InContext.MainSquare)
	{
		return;
	}

	if (!DemoActors.empty())
	{
		Exit(InContext);
	}

	DemoActors.clear();

	// 1. We start the sequence from the main square of the stage
	USceneComponent* CurrentParent = InContext.MainSquare;

	int32_t Count = 10;

	for (int32_t i = 0; i < Count; ++i)
	{
		// 2. Creating a New Actor
		AActor* NewActor = InContext.World->SpawnActor<AActor>("TransformDemoActor");

		// 3. Let's create a rendering component inside it
		USquareRenderComponent* RenderComp = NewActor->CreateComponent<USquareRenderComponent>(FVector2D(SQUARE_SIZE_X * 0.8f, SQUARE_SIZE_Y * 0.8f));
		RenderComp->SetColor(sf::Color::Magenta);

		// 4. LINKING THE HIERARCHY: Get the root of the created actor and attach it to the parent
		if (USceneComponent* ActorRoot = NewActor->GetRootComponent())
		{
			ActorRoot->SetRelativeLocation(FVector2D(150.0f, 0.0f));
			ActorRoot->SetRelativeScale(FVector2D(0.7f, 0.7f));
			ActorRoot->AttachToComponent(CurrentParent);

			// The next square in the loop will use the transformation of THIS square
			CurrentParent = ActorRoot;
		}

		NewActor->RegisterAllComponents();

		// 5. We store a raw pointer to the actor so that we can easily delete it later in Exit
		DemoActors.push_back(NewActor);
	}
}

void FTransformDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	if (!InContext.MainSquare)
	{
		return;
	}

	float CurrentParentRot = InContext.MainSquare->GetRelativeRotation();

	for (AActor* Actor : DemoActors)
	{
		if (USceneComponent* Root = Actor->GetRootComponent())
		{
			float CurrentRelativeRot = Root->GetRelativeRotation();
			float NewRelativeRot = FVector2D::Lerp(CurrentRelativeRot, CurrentParentRot, 10.0f * InDeltaTime);

			Root->SetRelativeRotation(NewRelativeRot);

			CurrentParentRot = NewRelativeRot;
		}
	}
}

void FTransformDemo::Exit(FDemoContext& InContext)
{
	// Find and remove all Render components of this Actor from the render
	for (AActor* ActorToDelete : DemoActors)
	{
		// Remove the created actors from the global world vector
		InContext.World->DestroyActor(ActorToDelete);
	}

	DemoActors.clear();
}