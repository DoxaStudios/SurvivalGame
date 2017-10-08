// Fill out your copyright notice in the Description page of Project Settings.

#include "FlockActor.h"


void AFlockActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
/*
void AFlockActor::PostSpawnInitialize(FVector const& SpawnLocation, FRotator const& SpawnRotation, AActor* InOwner, APawn* InInstigator, bool bRemoteOwned, bool bNoFail, bool bDeferConstruction)
{
	Super::PostSpawnInitialize(SpawnLocation, SpawnRotation, InOwner, InInstigator, bRemoteOwned, bNoFail, bDeferConstruction);

}
*/

AFlockActor::AFlockActor(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//this is your regular constructor code

	pSphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, "CollisionSphere");
	if (pSphereComponent)
	{
		pSphereComponent->InitSphereRadius(flockradius);
		RootComponent = pSphereComponent;
	}
	pStaticMeshInstanceComponent = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, TEXT("InstancedMeshes"));
	if (pStaticMeshInstanceComponent)
		pStaticMeshInstanceComponent->AttachParent = RootComponent;
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}


FRotator AFlockActor::FindLookAtRotation(FVector start, FVector end)
{
	return FRotationMatrix::MakeFromX(end - start).Rotator();
}

void AFlockActor::AddFlockMember(const FTransform& InstanceTransform)
{
	pStaticMeshInstanceComponent->AddInstance(InstanceTransform);
}

void AFlockActor::AddFlockMemberWorldSpace(const FTransform& WorldTransform)
{
	pStaticMeshInstanceComponent->AddInstanceWorldSpace(WorldTransform);

	FlockMemberData flocker;
	flocker.instanceindex = numflock;
	flocker.transform = WorldTransform;
	flocker.wanderposition = GetRandomWanderLocation();
	flocker.velocity = SteeringWander(flocker);
	if (numflock == 0)
		flocker.bIsFlockLeader = true;
	mFlockMemberData.Add(flocker);
	numflock++;
}

FVector AFlockActor::GetRandomWanderLocation()
{
	
	return FMath::VRand() * (FMath::FRand() * flockradius);
}

FVector AFlockActor::SteeringWander(FlockMemberData & flocker)
{
	FVector newVec = flocker.wanderposition - flocker.transform.GetLocation();

	if (flocker.elapsedTimeSinceLastWander >= flockwanderupdaterate || newVec.Size() <= flockminwanderdistance)
	{
		flocker.wanderposition = GetRandomWanderLocation() + GetActorLocation();
		flocker.elapsedTimeSinceLastWander = 0.0f;
		newVec = flocker.wanderposition - flocker.transform.GetLocation();
	}
	return newVec;
}

FVector AFlockActor::SteeringAlign(FlockMemberData & flocker, TArray<int32>& flockMates)
{
	FVector vel = FVector(0, 0, 0);
	if (flockMates.Num() == 0) return vel;

	for (int32 i = 0; i < flockMates.Num(); i++)
	{
		vel += mFlockMemberData[flockMates[i]].velocity;
	}

	vel /= (float)flockMates.Num();

	return vel;
};

FVector AFlockActor::SteeringSeperate(FlockMemberData & flocker, TArray<int32>& flockMates)
{
	FVector force = FVector(0, 0, 0);
	if (flockMates.Num() == 0) return force;

	for (int32 i = 0; i < flockMates.Num(); i++)
	{
		FVector diff = flocker.transform.GetLocation() - mFlockMemberData[flockMates[i]].transform.GetLocation();
		float scale = diff.Size();
		diff.Normalize();
		diff = diff * (seperationradius / scale);
		force += diff;
	}
	return force;
}

FVector AFlockActor::SteeringCohesion(FlockMemberData & flocker, TArray<int32>& flockMates)
{
	FVector avgpos = FVector(0, 0, 0);
	if (flockMates.Num() == 0) return avgpos;

	for (int32 i = 0; i < flockMates.Num(); i++)
	{
		avgpos += mFlockMemberData[flockMates[i]].transform.GetLocation();
	}

	avgpos /= (float)flockMates.Num();

	return avgpos - flocker.transform.GetLocation();
}


FVector AFlockActor::SteeringFollow(FlockMemberData & flocker, int32 flockleader)
{
	FVector newVec = FVector(0, 0, 0);

	if (flockleader <= mFlockMemberData.Num() && flockleader >= 0)
	{
		newVec = mFlockMemberData[flockleader].transform.GetLocation() - flocker.transform.GetLocation();
		newVec.Normalize();
		newVec *= flockmaxspeed;
		newVec -= flocker.velocity;

		// leader outside our awareness threshold, so lets not follow them..
		if (newVec.Size() > flockmateawarenessradius)
		{
			newVec = FVector(0, 0, 0);
		}


	}
	return newVec;
}

TArray<int32> AFlockActor::GetNearbyFlockMates(int32 flockmember)
{
	TArray<int32> mates;
	if (flockmember > mFlockMemberData.Num()) return mates;
	if (flockmember < 0) return mates;

	for (int32 i = 0; i < mFlockMemberData.Num(); i++)
	{
		if (i != flockmember)// && mFlockMemberData[i].bIsFlockLeader == false)
		{
			FVector diff = mFlockMemberData[i].transform.GetLocation() - mFlockMemberData[flockmember].transform.GetLocation();
			if (FMath::Abs(diff.Size()) < flockmateawarenessradius)
			{
				mates.Add(i);
			}
		}
	}

	return mates;
}

void AFlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!pStaticMeshInstanceComponent) return;

	for (int32 i = 0; i < mFlockMemberData.Num(); i++)
	{
		FlockMemberData &  flocker = mFlockMemberData[i];
		FVector pos = flocker.transform.GetLocation();

		if (flocker.instanceindex > pStaticMeshInstanceComponent->GetInstanceCount()) continue; // don't do anything if we havent got an instance in range..

		FVector newVelocity = FVector(0, 0, 0);
		if (flocker.bIsFlockLeader)
		{
			newVelocity += SteeringWander(flocker);
			flocker.elapsedTimeSinceLastWander += DeltaTime;
			if (DrawLeaderTarget)
			{
				DrawDebugSphere(GetWorld(), flocker.wanderposition, 20.0f, 16, FColor::Black, false, 0.1f);
				DrawDebugDirectionalArrow(GetWorld(), pos, flocker.wanderposition, 10.0f, FColor::Cyan);
			}
		}
		else
		{
			FVector followVec = FVector(0, 0, 0);
			FVector cohesionVec = FVector(0, 0, 0);
			FVector alignmentVec = FVector(0, 0, 0);
			FVector seperationVec = FVector(0, 0, 0);
			FVector fleeVec = FVector(0, 0, 0);

			if (followscale > 0.0f)
			{
				// leader following (seek)
				followVec = (SteeringFollow(flocker, 0) * followscale);
				if (DrawSteeringFollow)
					DrawDebugDirectionalArrow(GetWorld(), pos, pos + followVec, 10.0f, FColor::Cyan);
			}
			

			// other forces need nearby flockmates
			TArray<int32> mates = GetNearbyFlockMates(i);

			if (cohesionscale > 0.0f)
			{
				// cohesion - staying near nearby flockmates
				cohesionVec = (SteeringCohesion(flocker, mates) * cohesionscale);
				if (DrawSteeringCohesion)
					DrawDebugDirectionalArrow(GetWorld(), pos, pos + cohesionVec, 10.0f, FColor::Blue);
			}
			
			if (alignscale > 0.0f)
			{
				// alignment =  aligning with the heading of nearby flockmates
				alignmentVec = (SteeringAlign(flocker, mates) * alignscale);
				if (DrawSteeringAlign)
					DrawDebugDirectionalArrow(GetWorld(), pos, pos + alignmentVec, 10.0f, FColor::Green);
			}

			if (seperationscale > 0.0f)
			{
				// separation = trying to not get too close to flockmates
				seperationVec = (SteeringSeperate(flocker, mates) * seperationscale);
				if (DrawSteeringSeparate)
					DrawDebugDirectionalArrow(GetWorld(), pos, pos + seperationVec, 10.0f, FColor::Red);
			}

			if (fleescale > 0.0f)
			{
				// flee = running away from enemies!
				fleeVec = (SteeringFlee(flocker) * fleescale);
				if (DrawSteeringFlee)
					DrawDebugDirectionalArrow(GetWorld(), pos, pos + fleeVec, 10.0f, FColor::Cyan);
			}

			newVelocity += fleeVec;
			if (fleeVec.SizeSquared() <= 0.1f)
			{
				newVelocity += followVec;
				newVelocity += cohesionVec;
				newVelocity += alignmentVec;
				newVelocity += seperationVec;
			}

		}

		// truncate the new force calculated in newVelocity so we don't go crazy
		newVelocity = newVelocity.GetClampedToSize(0.0f, flockmaxsteeringforce);

		FVector targetVelocity = flocker.velocity + newVelocity;

		// rotate the flocker towards the velocity direction vector
		// get the rotation value for our desired target velocity (i.e. if we were in that direction)
		FRotator rot = FindLookAtRotation(flocker.transform.GetLocation(), flocker.transform.GetLocation() + targetVelocity);
		// lerp our current rotation towards the desired velocity vector based on rotationspeed * time
		FRotator final = FMath::RInterpTo(flocker.transform.Rotator(), rot, DeltaTime, flockmaterotationrate);

		flocker.transform.SetRotation(final.Quaternion());

		FVector forward = flocker.transform.GetUnitAxis(EAxis::X);
		forward.Normalize();
		flocker.velocity = forward * targetVelocity.Size();

		// clamp our new velocity to be within min->max speeds
		if (flocker.velocity.Size() > flockmaxspeed)
		{
			flocker.velocity = flocker.velocity.GetSafeNormal() * flockmaxspeed;
		}

		if (flocker.velocity.Size() < flockminspeed)
		{
			flocker.velocity = flocker.velocity.GetSafeNormal() * flockminspeed;
		}

		flocker.transform.SetLocation(flocker.transform.GetLocation() + flocker.velocity * DeltaTime);

		if (DrawSteeringRadius)
			DrawDebugSphere(GetWorld(), flocker.transform.GetLocation(), flockmateawarenessradius, 8, FColor::Yellow);



		pStaticMeshInstanceComponent->UpdateInstanceTransform(flocker.instanceindex, flocker.transform, true);

	}

	if (DrawSteeringFleeThreat)
	{
		for (int i = 0; i < Threats.Num(); i++)
		{
			DrawDebugSphere(GetWorld(), Threats[i]->GetActorLocation(), flockenemyawarenessradius, 8, FColor::Magenta);
		}
	}

	//UE_LOG(LogFlockComponent, Log, TEXT("Ticking Flock Component"));

	pStaticMeshInstanceComponent->UpdateBounds();

}

FVector AFlockActor::SteeringFlee(FlockMemberData & flocker)
{
	FVector newVec = FVector(0, 0, 0);

	for (int i = 0; i < Threats.Num(); i++)
	{
		if (Threats[i] != NULL && Threats[i]->IsValidLowLevel())
		{
			// calculate flee from this threat
			FVector fromEnemy = flocker.transform.GetLocation() - Threats[i]->GetActorLocation();
			float distanceToEnemy = fromEnemy.Size();
			fromEnemy.Normalize();

			// enemy inside our enemy awareness threshold, so evade them
			if (distanceToEnemy < flockenemyawarenessradius)
			{
				newVec += fromEnemy * ((flockenemyawarenessradius / distanceToEnemy) * fleescale);
			}
		}
	}
	return newVec;
}

