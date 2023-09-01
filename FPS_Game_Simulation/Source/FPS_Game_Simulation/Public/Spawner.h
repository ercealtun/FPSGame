// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class FPS_GAME_SIMULATION_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Returns the WhereToSpawn subobject
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	// Find a random point within the BoxComponent
	UFUNCTION(BlueprintPure, Category="Spawning")
	FVector GetRandomPointInBox();

	// Trigger to work spawner using timers
	void TrigSpawnerWithTimer(float SpawnRate);

protected:
	// Actor to be spawned
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AActor> ActorToSpawn;
	
	FTimerHandle SpawnTimerHandle;

		
private:
	// Box Component to specify where ammos should be spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning", meta=(AllowPrivateAccess="true"))
	UBoxComponent* WhereToSpawn;

	// Certain spawn time
	float SpawnRate = 3.f;
	
	// Handle spawning a new ammo
	void Spawn();
	

};
