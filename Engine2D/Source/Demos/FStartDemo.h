#pragma once

#include "IMathDemo.h"

/** Idle "no demo selected" placeholder shown on startup; does nothing each tick. */
class FStartDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override {}
	std::string GetDemoName() const override { return "No Active Math Demo | Use Left / Right Arrow Keys"; }
};