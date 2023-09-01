// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the spawn box component to represent the spawn volume
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	SetRootComponent(WhereToSpawn);

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	
}

FVector ASpawner::GetRandomPointInBox()
{
	FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawner::TrigSpawnerWithTimer(float SpawnTimeRate)
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawner::Spawn, SpawnTimeRate, false);
}

void ASpawner::Spawn()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("spawning"));
	if(ActorToSpawn)
	{
		UWorld* World = GetWorld();
		if(World)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			// SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			FVector SpawnLocation = GetRandomPointInBox();
			FRotator SpawnRotation = FRotator::ZeroRotator;
			
			AActor* const SpawnedAmmo = World->SpawnActor<AActor>(ActorToSpawn, SpawnLocation, SpawnRotation, SpawnParameters);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("spawned"));
		}
	}
}


