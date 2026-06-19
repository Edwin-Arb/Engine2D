#pragma once

#include "FTransform2D.h"

/**
 * Camera controller facilitating world-to-view space coordinate conversion matrix generation.
 */
class FCamera
{
public:
	/**
	 * Computes the composite View Matrix for scene rendering passes.
	 * Flips spatial positions into relative camera-space and centers the tracking focus onto the screen viewport.
	 */
	FMatrix3x3 GetViewMatrix() const;

public:
	/** Spatial node component housing the camera's location, rotation, and scaling context profiles. */
	FTransform2D TransformComponent;
};