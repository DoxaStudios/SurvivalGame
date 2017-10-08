#pragma once

#include "Engine.h"
#include "FlockMemberData.generated.h"

USTRUCT()
struct FlockMemberData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 instanceindex;

	UPROPERTY()
		FVector velocity;

	UPROPERTY()
		FVector wanderposition;

	UPROPERTY()
		FTransform transform;

	UPROPERTY()
		float elapsedTimeSinceLastWander;

	UPROPERTY()
		bool bIsFlockLeader;

	FlockMemberData()
	{
		instanceindex = 0;
		velocity = FVector(0, 0, 0);
		elapsedTimeSinceLastWander = 0.0f;
		wanderposition = FVector(0, 0, 0);
		bIsFlockLeader = false;
	};
};