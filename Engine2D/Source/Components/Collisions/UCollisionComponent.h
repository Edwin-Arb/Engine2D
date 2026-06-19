#pragma once
#include "../Base/USceneComponent.h"

/** Supported collision filtering channels mimicking Unreal Engine's architecture. */
enum class ECollisionChannel : int8_t
{
	ECC_WorldStatic,
	ECC_WorldDynamic,
	ECC_Pawn,
	ECC_Visibility
};

/**
 * Base abstract class for all spatial collision shapes and volumes.
 * Must be attached to an AActor containing a valid root transform node.
 */
class UCollisionComponent : public USceneComponent
{
public:
	// UCollisionComponent() = default;
	~UCollisionComponent() override = default;

	/**
	 * Pure virtual method to evaluate intersection against another collision shape.
	 * @param InOther           The other collision component to test against.
	 * @param OutNormal         The push-out direction vector pointing from InOther towards this component.
	 * @param OutPenetration    The depth of intersection overlap in pixels.
	 * @return                  True if shapes are overlapping, false otherwise.
	 */
	virtual bool Intersects(UCollisionComponent* InOther, FVector2D& OutNormal, float& OutPenetration) = 0;

	/** Setters and Getters for collision filtering channels. */
	void SetCollisionChannel(ECollisionChannel NewChannel) { CollisionChannel = NewChannel; }
	ECollisionChannel GetCollisionChannel() const { return CollisionChannel; }

protected:
	/** Defines how this component filters out tracking sweeps and queries. */
	ECollisionChannel CollisionChannel = ECollisionChannel::ECC_Pawn;
};
