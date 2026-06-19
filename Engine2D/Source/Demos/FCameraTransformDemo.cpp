#include "FCameraTransformDemo.h"

#include "../Core/FDemoContext.h"
#include "../Math/FCamera.h"

void FCameraTransformDemo::Tick(float InDeltaTime, FDemoContext& InContext)
{
	FTransform2D* CameraTransformComp = InContext.CameraTransform;

	// Reset spatial transforms instantly back to baseline default parameters on Space-bar input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		CameraTransformComp->SetPosition({ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f });
		CameraTransformComp->SetRotation(0.0f);
		CameraTransformComp->SetScale({ 1.0f, 1.0f });
		return;
	}

	FVector2D CamInput(0.0f, 0.0f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		CamInput.X -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		CamInput.X += 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		CamInput.Y -= 1.0f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		CamInput.Y += 1.0f;

	if (CamInput.Length() > EPSILON)
	{
		// Normalize inputs to maintain fixed constant speed vectors over oblique axes
		FVector2D NewPos = CameraTransformComp->GetRelativePosition() + CamInput.Normalized() * Speed * InDeltaTime;
		CameraTransformComp->SetPosition(NewPos);
	}

	float CurrentRotation = CameraTransformComp->GetRelativeRotation();
	bool bRotationChanged = false;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		CurrentRotation -= RotationSpeed * InDeltaTime;
		bRotationChanged = true;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		CurrentRotation += RotationSpeed * InDeltaTime;
		bRotationChanged = true;
	}

	if (bRotationChanged)
	{
		CameraTransformComp->SetRotation(CurrentRotation);
	}
}

void FCameraTransformDemo::HandleEvent(const sf::Event& InEvent, FDemoContext& InContext)
{
	if (const auto* MouseWheel = InEvent.getIf<sf::Event::MouseWheelScrolled>())
	{
		if (MouseWheel->wheel == sf::Mouse::Wheel::Vertical)
		{
			const float ZoomDelta = MouseWheel->delta > 0 ? -ZoomSpeed : ZoomSpeed;
			FVector2D CamScale = InContext.CameraTransform->GetRelativeScale();

			CamScale.X += ZoomDelta;
			CamScale.Y += ZoomDelta;

			// Enforce bounding limits onto scale dimensions
			CamScale.X = std::max(MinZoom, std::min(MaxZoom, CamScale.X));
			CamScale.Y = std::max(MinZoom, std::min(MaxZoom, CamScale.Y));

			InContext.CameraTransform->SetScale(CamScale);
		}
	}
}