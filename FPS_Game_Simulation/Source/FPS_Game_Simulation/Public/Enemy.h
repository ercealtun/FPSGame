// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"
class UBoxComponent;

UCLASS()
class FPS_GAME_SIMULATION_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// If player enters this box enemy will attack
	UPROPERTY(EditAnywhere, Category="Enemy")
	UBoxComponent* TriggerBox;

	// Bool for attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	bool bCanAttackPlayer = false;

	// Begin & End overlap functions
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);	
	UFUNCTION()
	void OnHitEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// To implement senses for an actor (sight, hear etc..)
	UPROPERTY(VisibleDefaultsOnly, Category="Enemy")
	class UAIPerceptionComponent* AIPerceptionComponent;

	// Is used for configure AI perception component for site detection
	UPROPERTY(VisibleDefaultsOnly, Category="Enemy")
	class UAISenseConfig_Sight* SightConfig;

	// This will inform enemy if any actors are detected by AI perception
	UFUNCTION()
	void OnSensed(const TArray<AActor*> &UpdatedActors);

	// Enemy rotation
	UPROPERTY(VisibleAnywhere, Category="Movement")
	FRotator EnemyRotation;

	// Where AI enemy patrols
	UPROPERTY(VisibleAnywhere, Category="Movement")
	FVector BaseLocation;

	// Current vectorial velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	FVector CurrentVelocity;

	// Movement speed multiplier
	UPROPERTY(VisibleAnywhere, Category="Movement")
	float MovementSpeed = 500.f;

	// Sets new rotation for enemy
	void SetNewRotation(FVector TargetPosition, FVector CurrentPosition);

	// Checks if enemy back to its base location
	bool bBackToBaseLocation;
	
	FVector NewLocation;
	
	// Calculation result for the distance between enemy and player
	float DistanceSquared;

	// Current health of enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 100.f;

	// Damage value for harming player
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageValue = 5.f;

	// Getting damage by player
	UFUNCTION(BlueprintCallable)
	void DealDamage(float DamageAmount);

private:
	// Spawner Ref
	class ASpawner* Spawner;

	// Spawn rate for next spawn
	UPROPERTY(EditAnywhere)
	float SpawnRate = 3.f;

	// Character Ref
	class AFPS_Game_SimulationCharacter* Character;
};

