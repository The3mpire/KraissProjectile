#pragma once=
#include "GameFramework/MovementComponent.h"


// delegates for the projectile itself to subscribe to handle teardown
DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjectileStopDelegate, const FHitResult&, ImpactResult);

//data
// TODO should override the base HitResult so our subscribers can have better info (could have impact velocity, or if we have critical hits)

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

	// TODO friction work!
	UPROPERTY(EditAnywhere)
	bool bHasFriction;

	UPROPERTY(EditAnywhere, EditCondition="bHasFriction")
	float Friction;

	UPROPERTY(EditAnywhere)
	bool bCanPenetrate;

	// total number of objects (walls, people, flowers, holy hand grenades) this projectile can go through
	// inclusive!
	UPROPERTY(EditAnywhere, EditCondition="bCanPenetrate")
	int32 MaxPenetrations;
	
	// projectile spawner might be a world placed item (turret!), so don't assume pawn is always the instigator
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AController> Instigator;
};

// TODO: bounces!
USTRUCT()
struct FBouncyProjectileDefinition : FBaseProjectileDefinition
{
	GENERATED_BODY()
public:	
	// Percentage of velocity maintained after the bounce in the direction of the normal of impact (coefficient of restitution).
	// 1.0 = no velocity lost, 0.0 = no bounce
	UPROPERTY(EditAnywhere)
	float Bounciness;
	
	// Total number of bounces before triggering End of Life effect
	// -1 means no maxbounces, use lifetime to determine End of Life
	UPROPERTY(EditAnywhere)
	int32 MaxBounces;

	// Whether this bouncy projectile will do damage on each bounce 
	UPROPERTY(EditAnywhere)
	bool bDamagesOnBounce;

	UPROPERTY(EditAnywhere, EditCondition="bDamagesOnBounce")
	float DamageOnBounce;
};

// work
UCLASS()
class UKraissProjectileMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	// !! assuming the component is properly initialized & that the spawner successfully set up struct information !!
	UKraissProjectileMovementComponent();
	UKraissProjectileMovementComponent(const FVector LaunchVelocity, const FVector LaunchLocation);

	void SetInitialVelocity(const FVector Velocity) { InitialVelocity = Velocity; }
	
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

public:
	UPROPERTY(BlueprintAssignable)
	FOnProjectileStopDelegate OnProjectileStop;

protected:
	UPROPERTY()
	FBaseProjectileDefinition ProjectileDefinition;
	
	// projectile Launcher should pass this data
	FVector InitialVelocity;
	FVector InitialLocation;

	int32 TotalImpacts;
	
	float Lifespan;

	// Begin modified from UProjectileMovementComponent.cpp 
	FVector UKraissProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const;
	// End modified from UProjectileMovementComponent.cpp
};
