#include "FCamera.h"

FMatrix3x3 FCamera::GetViewMatrix() const
{
	// 1. Invert the camera's world matrix to transform world coordinates into relative camera local space
	const FMatrix3x3 InverseCameraMatrix = TransformComponent.ToMatrix().Inverse();

	// 2. Shift the layout coordinate origin from top-left (0,0) over onto the exact midpoint center of the screen
	const FMatrix3x3 MoveToScreenCenter = FMatrix3x3::CreateTranslation({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });

	// 3. Premultiply transformations: The translation offset must cascade from the left side matrix block
	return MoveToScreenCenter * InverseCameraMatrix;
}