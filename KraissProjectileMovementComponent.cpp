#include "KraissProjectileMovementComponent.h"

#include <NvParameterized.h>


UKraissProjectileMovementComponent::UKraissProjectileMovementComponent()
{
	InitialVelocity = FVector::ZeroVector;
	InitialLocation = GetOwner()->GetActorLocation();
}

UKraissProjectileMovementComponent::UKraissProjectileMovementComponent(const FVector LaunchVelocity, const FVector LaunchLocation)
{
	InitialVelocity = LaunchVelocity;
	InitialLocation = LaunchLocation;
	Lifespan = 0.0f;
	TotalImpacts = 0;
	
	// validating data? never heard of her ¯\_(ツ)_/¯
}

void UKraissProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick Tick, FActorComponentTickFunction* ThisTickFunction)
{
	UMovementComponent::TickComponent(DeltaTime, Tick, ThisTickFunction);

	FHitResult Hit;
	// not simulating physics since assuming we're not networked
	// move our projectile

	const FVector MoveDelta = ComputeMoveDelta(InitialVelocity, DeltaTime);
	
	MoveUpdatedComponent(MoveDelta, MoveDelta.Rotation(), true, &Hit);

	// Handle hit result after movement
	if (Hit.bBlockingHit)
	{
		UHealthComponent* HealthComponent;
		// NOTE: making an assumption that something like this exists for damageable items
		if (IHealthComponentInterface* HealthInterface = Cast<IHealthComponentInterface>(Hit.Item))
		{
			HealthComponent = UHealthComponentHealthInterface->GetHealthComponent();
		}
		
		// for now making the assumption that a projectile cannot be bouncy & penetrate
		// theoretically there could be bouncy materials and penetratable materials :(
		if (ProjectileDefinition.bCanPenetrate)
		{
			TotalImpacts++;
			if (TotalImpacts > ProjectileDefinition.MaxPenetrations)
			{
				OnProjectileStop.Broadcast(Hit);
				if (ProjectileDefinition.Lifetime > 0) // check if we stay around until the projectile itself decides (throwing knife)
				{
					HandleEndOfLife();
				}
			}
			// apply damage
			
		} 
		else if (ProjectileDefinition.bCanBounce) // we bouncin?
		{
			if (ProjectileDefinition.DamageOnBounce > 0)
			{
				HealthComponent->ApplyDamage(ProjectileDefinition.DamageOnBounce);
			}

			// calculate new trajectory
		}
	}

	Lifespan += DeltaTime;
	
	// check if we have gone too far or been alive too long!
	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	if (Lifespan > ProjectileDefinition.Lifetime || FVector::DistSquared(CurrentLocation, InitialLocation) > ProjectileDefinition.MaxDistance)
	{
		OnProjectileStop.Broadcast(Hit);	
	}
}

void UKraissProjectileMovementComponent::HandleEndOfLife()
{
	if (ProjectileDefinition.DamageType == EDamageType::Radial)
	{
		// TODO sphere sweep for damage to everything we hit
	}

	// tear down
	OnEndOfLife.Broadcast();
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const
{
	const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaTime);
	const FVector Delta = (InVelocity * DeltaTime) + (NewVelocity - InVelocity) * (0.5f * DeltaTime);
	return Delta;
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime);
	const FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);

	return LimitVelocity(NewVelocity);
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Acceleration(FVector::ZeroVector);

	Acceleration.Z += GetGravityZ();

	// TODO: check if we are at max speed to see if we should add (or 'subtract') for option of gas fueled projectiles
	// Acceleration += PendingForce;
	
	return Acceleration;
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const
{
	if (ProjectileDefinition.MaxSpeed != 0.0f || ProjectileDefinition.MinSpeed != 0.0f)
	{
		NewVelocity = NewVelocity.GetClampedToSize(ProjectileDefinition.MinSpeed, ProjectileDefinition.MaxSpeed);
	}
	
	return ConstrainDirectionToPlane(NewVelocity);
}
