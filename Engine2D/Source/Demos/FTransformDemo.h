#pragma once
#include "IMathDemo.h"

class AActor;

/**
 * Demonstrates hierarchical affine matrix cascading by chaining local spatial nodes together.
 */
class FTransformDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Transform System Demo | Local & World Space Hierarchy | R"; }

private:
	/** Tracking container preserving direct raw pointers inside memory manager registries. */
	std::vector<AActor*> DemoActors;
};
