#pragma once

#include "IMathDemo.h"

/** Default fallback system initialization demo module. */
class FStartDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override {}
	std::string GetDemoName() const override { return "No Active Math Demo | Use Left / Right Arrow Keys"; }
};