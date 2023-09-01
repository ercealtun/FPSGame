// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

UCLASS()
class FPS_GAME_SIMULATION_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Ammo amount 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int AmmoAmount = 10;

	// Trigger for changes in ammo amount
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerAmmoAmount();

private:
	// Ammo mesh
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	// Trigger for showing info widget
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComponent;

	// If player overlaps with sphere component s/he can gain ammo
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* InteractSphereComponent;
	
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// FPS character Ref
	class AFPS_Game_SimulationCharacter* Character;
	
	// Weapon Ref
	class UTP_WeaponComponent* Weapon;
	
	// Spawner Ref
	class ASpawner* Spawner;
	
	// Spawn rate for next spawn
	UPROPERTY(EditAnywhere)
	float SpawnRate = 9.f;

};
