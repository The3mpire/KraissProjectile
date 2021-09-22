#pragma once

#include "GameFramework/MovementComponent.h"

#include "KraissProjectileMovementComponent.generated.h"

// delegates for the projectile itself to subscribe to handle teardown
DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjectileStopDelegate, const FHitResult&, ImpactResult);
DECLARE_MULTICAST_DELEGATE(FOnEndOfLifeDelegate);

//data
// TODO should override the base HitResult so our subscribers can have better info (could have impact velocity, or if we have critical hits)
UENUM()
enum class EDamageType : uint8
{
	Point,
	Radial,
};

USTRUCT()
struct FBaseProjectileDefinition
{
	GENERATED_BODY()
public:
	FBaseProjectileDefinition()
		: Lifetime(0),
		InitialSpeed(0),
		MaxSpeed(0),
		MinSpeed(0),
		MaxDistance(MAX_FLT),
		Damage(0),
		DamageType(EDamageType::Point),
		DamageRadius(0),
		bCanPenetrate(false),
		MaxPenetrations(0),
		bCanBounce(false),
		Bounciness(0.0f),
		MaxBounces(0),
		DamageOnBounce(0)
	{}
	// drop rate & friction should be design editable
	
	// how long this projectile should stay around, a negative number means it should stay indefinitely
	UPROPERTY(EditAnywhere)
	float Lifetime;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	// if no max speed, leave 0
	UPROPERTY(EditAnywhere)
	float MaxSpeed;

	// if no min speed, leave 0
	UPROPERTY(EditAnywhere)
	float MinSpeed;

	UPROPERTY(EditAnywhere)
	float MaxDistance;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	EDamageType DamageType;

	UPROPERTY(EditAnywhere)
	float DamageRadius;
	
	UPROPERTY(EditAnywhere)
	bool bCanPenetrate;

	// total number of objects (walls, people, flowers, holy hand grenades) this projectile can go through
	// inclusive!
	UPROPERTY(EditAnywhere, meta = (EditCondition="bCanPenetrate"))
	int32 MaxPenetrations;

	// TODO: bounces!
	// should likely be seperated out to a "BounceableProjectile" inheriting from BaseProjectile, since this struct is a bit big

	// whether this projectile can bounce
	UPROPERTY(EditAnywhere, Category="Bounceable", meta = (EditCondition="!bCanPenetrate"))
	bool bCanBounce;

	// Percentage of velocity maintained after the bounce in the direction of the normal of impact (coefficient of restitution).
	// 1.0 = no velocity lost, 0.0 = no bounce
	UPROPERTY(EditAnywhere, Category="Bounceable", meta = (EditCondition="bCanBounce"))
	float Bounciness;
	
	// Total number of bounces before triggering End of Life effect
	// -1 means no maxbounces, use lifetime to determine End of Life
	UPROPERTY(EditAnywhere, Category="Bounceable", meta = (EditCondition="bCanBounce"))
	int32 MaxBounces;

	// Whether this bouncy projectile will do damage on each bounce
	// 0 means no damage on bounce
	UPROPERTY(EditAnywhere, Category="Bounceable", meta = (EditCondition="bDamagesOnBounce"))
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

	UPROPERTY(BlueprintAssignable)
	FOnEndOfLifeDelegate OnEndOfLife;

protected:
	void HandleEndOfLife(FHitResult& HitResult);

	void HandleDamage(FHitResult &HitResult);

	TArray<AActor> GetExplosionRadiusItems(FVector StartLocation);
	
	// Begin modified from UProjectileMovementComponent.cpp 
	FVector UKraissProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;
	FVector UKraissProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const;
	// End modified from UProjectileMovementComponent.cpp

protected:
	UPROPERTY()
	FBaseProjectileDefinition ProjectileDefinition;
	
	// projectile Launcher should pass this data
	FVector InitialVelocity;
	FVector InitialLocation;

	int32 TotalImpacts;
	
	float Lifespan;
};
