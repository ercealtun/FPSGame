// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Health.generated.h"

UCLASS()
class FPS_GAME_SIMULATION_API AHealth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthAmount = 25.f;

private:
	// Health mesh
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	// Trigger for showing info widget
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComponent;

	// If player overlaps with sphere component s/he gain health
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* InteractSphereComponent;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// FPS character Ref
	class AFPS_Game_SimulationCharacter* Character;

	// Spawner Ref
	class ASpawner* Spawner;

	// Spawn rate for next spawn
	UPROPERTY(EditAnywhere)
	float SpawnRate = 15.f;


};
