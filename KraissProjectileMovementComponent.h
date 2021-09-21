#pragma once=
#include "GameFramework/MovementComponent.h"

USTRUCT()
struct FBaseProjectileDefinition
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float Lifetime;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	bool bHasFriction;

	UPROPERTY(EditDefaultsOnly)
	float Friction;
	
	// projectile spawner might be a world placed item (turret!), so don't assume pawn is always the instigator
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AController> Instigator;
};

USTRUCT()
struct FBouncyProjectileDefinition : FBaseProjectileDefinition
{
	GENERATED_BODY()
public:
	FBouncyProjectileDefinition()
		: Bounciness(0.0f),
		MaxBounces(0)
	{}

	/**
	* Percentage of velocity maintained after the bounce in the direction of the normal of impact (coefficient of restitution).
	* 1.0 = no velocity lost, 0.0 = no bounce
	*/
	UPROPERTY(EditAnywhere)
	float Bounciness;

	/**
	* Total number of bounces before triggering End of Life effect
	*/
	UPROPERTY(EditAnywhere)
	int32 MaxBounces;
};

class KraissProjectileMovementComponent : public UMovementComponent
{
public:
	// if networked - would need probably want a delegate for the hit result and might want to separate out the ProjectileDefinition so not sending that whole thing over the network

protected:
	
	UPROPERTY()
	FBaseProjectileDefinition ProjectileDefinition;
};
