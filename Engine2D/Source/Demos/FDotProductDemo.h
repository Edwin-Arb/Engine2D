#pragma once

#include "IMathDemo.h"

/**
 * Demonstrates projection mappings and sight lines analysis using vector Dot Product math.
 */
class FDotProductDemo : public IMathDemo
{
public:
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Dot Product Demo | Vector Direction Similarity"; }
};