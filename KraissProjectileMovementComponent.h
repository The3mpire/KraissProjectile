#pragma once=
#include "GameFramework/MovementComponent.h"

//data
USTRUCT()
struct FBaseProjectileDefinition
{
	GENERATED_BODY()
public:
	// how long this projectile should stay around, a negative number means it should stay indefinitely
	UPROPERTY(EditAnywhere)
	float Lifetime;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	float MaxSpeed;

	UPROPERTY(EditAnywhere)
	float MinSpeed;

	UPROPERTY(EditAnywhere)
	float MaxDistance;

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
	* -1 means no maxbounces, use lifetime to determine End of Life
	*/
	UPROPERTY(EditAnywhere)
	int8 MaxBounces;
};
// work
class UKraissProjectileMovementComponent : public UMovementComponent
{
public:
	// !! assuming the component is properly initialized & that the spawner successfully set up struct information !!
	UKraissProjectileMovementComponent();

	void SetInitialVelocity(const FVector Velocity) { InitialVelocity = Velocity; }
	
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface
	
	// if networked - would need probably want a delegate for the hit result and might want to separate out the ProjectileDefinition so not sending that whole thing over the network

protected:
	UPROPERTY()
	FBaseProjectileDefinition ProjectileDefinition;
	
	// projectile Launcher should pass this data
	FVector InitialVelocity;

	// Begin modified from UProjectileMovementComponent.cpp 
	FVector UKraissProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const;
	// End modified from UProjectileMovementComponent.cpp
};
