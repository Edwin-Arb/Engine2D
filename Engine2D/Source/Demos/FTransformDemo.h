#pragma once
#include "IMathDemo.h"

class AActor;

/**
 * Builds a chain of attached actors to show how transforms compose down a hierarchy:
 * each child inherits its parent's position, rotation, and scale.
 */
class FTransformDemo : public IMathDemo
{
public:
	void Enter(FDemoContext& InContext) override;
	void Tick(float InDeltaTime, FDemoContext& InContext) override;
	void Exit(FDemoContext& InContext) override;
	std::string GetDemoName() const override { return "Transform System Demo | Local & World Space Hierarchy | R"; }

private:
	/** Non-owning pointers to the actors spawned by this demo, so Exit can destroy them. */
	std::vector<AActor*> DemoActors;
};
