#include "KraissProjectileMovementComponent.h"


UKraissProjectileMovementComponent::UKraissProjectileMovementComponent()
{
	// assuming the spawner passed the projectile the proper direction when launching the projectile
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
		
	}
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const
{
	// Velocity Verlet integration (http://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet)
	// The addition of p0 is done outside this method, we are just computing the delta.
	// p = p0 + v0*t + 1/2*a*t^2

	// We use ComputeVelocity() here to infer the acceleration, to make it easier to apply custom velocities.
	// p = p0 + v0*t + 1/2*((v1-v0)/t)*t^2
	// p = p0 + v0*t + 1/2*((v1-v0))*t

	const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaTime);
	const FVector Delta = (InVelocity * DeltaTime) + (NewVelocity - InVelocity) * (0.5f * DeltaTime);
	return Delta;
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	// v = v0 + a*t
	const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime);
	const FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);

	return LimitVelocity(NewVelocity);
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Acceleration(FVector::ZeroVector);

	Acceleration.Z += GetGravityZ();

	// TODO: calculate the pending force to add (or 'subtract') for option of gas fueled projectiles
	// Acceleration += PendingForce;
	
	return Acceleration;
}

// modified from UProjectileMovementComponent.cpp
FVector UKraissProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const
{
	const float CurrentMaxSpeed = ProjectileDefinition.MaxSpeed;
	if (CurrentMaxSpeed > 0.f)
	{
		NewVelocity = NewVelocity.GetClampedToMaxSize(CurrentMaxSpeed);
	}

	return ConstrainDirectionToPlane(NewVelocity);
}
