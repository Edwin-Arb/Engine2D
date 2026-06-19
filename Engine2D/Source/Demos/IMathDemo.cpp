#include "IMathDemo.h"

#include "../Components/Render/UCircleRenderComponent.h"
#include "../Components/Render/USquareRenderComponent.h"
#include "../Core/FDemoContext.h"

void IMathDemo::Enter(FDemoContext& InContext)
{
	// Default Enter: recenter and reset the shared square/circle so every demo
	// starts from the same clean state. Demos can override to add their own setup.
	if (InContext.MainSquare)
	{
		InContext.MainSquare->SetRelativeLocation(FVector2D(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f));
		InContext.MainSquare->SetRelativeRotation(0.0f);
		InContext.MainSquare->SetRelativeScale(FVector2D(1.0f, 1.0f));

		InContext.MainCircle->SetRelativeLocation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
		InContext.MainCircle->SetRelativeRotation(0.0f);
		InContext.MainCircle->SetRelativeScale(FVector2D(1.0f, 1.0f));

		InContext.MainSquare->SetColor(GRAY_COLOR);
	}
}