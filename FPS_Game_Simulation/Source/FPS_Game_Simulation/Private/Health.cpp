// Fill out your copyright notice in the Description page of Project Settings.


#include "Health.h"

#include "Spawner.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "FPS_Game_Simulation/FPS_Game_SimulationCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHealth::AHealth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Health Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	BoxComponent->SetupAttachment(Mesh);

	InteractSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	InteractSphereComponent->SetupAttachment(Mesh);
	InteractSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealth::OnSphereBeginOverlap);

	
}

// Called when the game starts or when spawned
void AHealth::BeginPlay()
{
	Super::BeginPlay();

	// Setting health spawner
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawner::StaticClass(), FoundSpawners);
	for (AActor* FoundSpawner : FoundSpawners)
	{
		if(FoundSpawner && FoundSpawner->ActorHasTag(TEXT("HealthSpawner")))
		{
			Spawner = Cast<ASpawner>(FoundSpawner);
		}
	}
	
}

// Called every frame
void AHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealth::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AFPS_Game_SimulationCharacter>(OtherActor);
	
	// If character is valid & health is under 100
	if(Character && Character->GetCurrentHealth() <= 100)
	{
		// Checking if character needs the health or not
		float CharacterHealth = Character->GetCurrentHealth();
		if(CharacterHealth + HealthAmount > 100)
		{
			Character->SetCurrentHealth(Character->GetMaxHealth());
		}else
		{
			Character->SetCurrentHealth(CharacterHealth+HealthAmount);
		}

		// Spawn again
		if (Spawner)
			Spawner->TrigSpawnerWithTimer(SpawnRate);
		
		Destroy();
	}
}

