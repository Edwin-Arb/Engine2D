#pragma once

#include "IMathDemo.h"

/**
 * Demonstrates the usage of 2D Cross Product to determine relative left/right orientation.
 */
class FCrossProductDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Cross Product Demo | Left / Right Vector Orientation"; }
};