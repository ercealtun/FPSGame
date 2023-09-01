// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Spawner.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "FPS_Game_Simulation/FPS_Game_SimulationCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAmmo::AAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ammo Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	BoxComponent->SetupAttachment(Mesh);

	InteractSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	InteractSphereComponent->SetupAttachment(Mesh);
	InteractSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnSphereBeginOverlap);

}

// Called when the game starts or when spawned
void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	// Setting ammo spawner
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawner::StaticClass(), FoundSpawners);
	for (AActor* FoundSpawner : FoundSpawners)
	{
		if(FoundSpawner && FoundSpawner->ActorHasTag(TEXT("AmmoSpawner")))
		{
			Spawner = Cast<ASpawner>(FoundSpawner);
		}
	}
}

// Called every frame
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmo::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	Character = Cast<AFPS_Game_SimulationCharacter>(OtherActor);

	// If character is valid and got the rifle
	if(Character && Character->bHasRifle)
	{
		Weapon = Character->GetRifle();
		// If weapon is valid
		if(Weapon)
		{
			// If the gun needs ammo
			if(Weapon->CurrentAmmoLimit > Weapon->CurrentAmmo)
			{
				//  If you collect an ammo item and the amount of collectible bullets exceeds the
				//  current maximum amount of ammo, the character must collect needed amount of bullets from the ground.
				//  The remaining amount of bullets will stay on the ground.
				if(Weapon->CurrentAmmoLimit - Weapon->CurrentAmmo >= AmmoAmount)
				{
					Weapon->AddAmmo(AmmoAmount);
					// Once you collect an ammo item, there must be a certain amount of time interval before spawning next ammo item
					if(Spawner)
						Spawner->TrigSpawnerWithTimer(SpawnRate);
					Destroy();
				}else
				{
					// Taking the needed bullets
					AmmoAmount -= Weapon->CurrentAmmoLimit - Weapon->CurrentAmmo;
					// Fulling the ammo
					Weapon->CurrentAmmo += Weapon->CurrentAmmoLimit - Weapon->CurrentAmmo;

					TriggerAmmoAmount();
				}
			}else
			{
				// Dont take the ammo, you dont got the right!!!
			}
		}
	}
}

