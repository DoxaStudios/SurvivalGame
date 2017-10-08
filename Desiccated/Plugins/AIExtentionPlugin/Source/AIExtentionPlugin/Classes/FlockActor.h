// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlockMemberData.h"
#include "FlockActor.generated.h"

/**
 * 
 */
UCLASS()
class AFlockActor : public AActor
{

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Flock")
	int32 numflockinstances;

	// constructor
	AFlockActor(const class FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	USphereComponent * pSphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	UInstancedStaticMeshComponent * pStaticMeshInstanceComponent;



	//void PostSpawnInitialize(FVector const& SpawnLocation, FRotator const& SpawnRotation, AActor* InOwner, APawn* InInstigator, bool bRemoteOwned, bool bNoFail, bool bDeferConstruction) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Flock")
		int32 numflock;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockradius = 2000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockminspeed = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockmaxspeed = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockwanderupdaterate = 2000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockminwanderdistance = 50.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockmateawarenessradius = 400.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockenemyawarenessradius = 400.0f;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float followscale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float alignscale = 0.4f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float cohesionscale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float seperationscale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float fleescale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float seperationradius = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockmaxsteeringforce = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		float flockmaterotationrate = 0.3f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringRadius = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringAlign = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringFollow = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringSeparate = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringCohesion = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringFlee = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawSteeringFleeThreat = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawFollowTarget = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock")
		bool DrawLeaderTarget = false;


	UPROPERTY(BlueprintReadOnly, Category = "Flock")
	TArray<FlockMemberData> mFlockMemberData;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Flock")
	TArray<AActor *> Threats;

	///** Add an instance to this component. Transform is given in local space of this component.  */
	UFUNCTION(BlueprintCallable, Category = "Flock")
	void AddFlockMember(const FTransform& InstanceTransform);

	///** Add an instance to this component. Transform is given in world space. */
	UFUNCTION(BlueprintCallable, Category = "Flock")
	void AddFlockMemberWorldSpace(const FTransform& WorldTransform);

	void Tick(float DeltaTime);

	FVector GetRandomWanderLocation();

	TArray<int32> GetNearbyFlockMates(int32 flockmember);

	FVector SteeringWander(FlockMemberData & flocker);

	FVector SteeringAlign(FlockMemberData & flocker, TArray<int32>& flockMates);

	FVector SteeringSeperate(FlockMemberData & flocker, TArray<int32>& flockMates);

	FVector SteeringCohesion(FlockMemberData & flocker, TArray<int32>& flockMates);

	FVector SteeringFollow(FlockMemberData & flocker, int32 flockleader);

	FVector SteeringFlee(FlockMemberData & flocker);

	FRotator FindLookAtRotation(FVector start, FVector end);
	//
};
