// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Spawner.h"
#include "Components/BoxComponent.h"
#include "FPS_Game_Simulation/FPS_Game_SimulationCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnHitEnd);
	TriggerBox->SetVisibility(true);

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1500.f;
	SightConfig->PeripheralVisionAngleDegrees = 100.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	// How much time AI will forget player
	SightConfig->SetMaxAge(.1f);

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemy::OnSensed);

	CurrentVelocity = FVector::ZeroVector;
	
	DistanceSquared = BIG_NUMBER;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Setting base location in start
	BaseLocation = GetActorLocation();

	
	// Setting enemy spawner
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawner::StaticClass(), FoundSpawners);
	for (AActor* FoundSpawner : FoundSpawners)
	{
		if(FoundSpawner && FoundSpawner->ActorHasTag(TEXT("EnemySpawner")))
		{
			Spawner = Cast<ASpawner>(FoundSpawner);
		}
	}

	// Setting character
	Character = Cast<AFPS_Game_SimulationCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Getting back to base location in a smooth way
	if(!CurrentVelocity.IsZero())
	{
		NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;

		if(bBackToBaseLocation)
		{
			if((NewLocation - BaseLocation).SizeSquared2D() < DistanceSquared)
			{
				DistanceSquared = (NewLocation - BaseLocation).SizeSquared2D();
			}else
			{
				CurrentVelocity = FVector::ZeroVector;
				DistanceSquared = BIG_NUMBER;
				bBackToBaseLocation = false;

				SetNewRotation(GetActorForwardVector(), GetActorLocation());
			}
		}

		SetActorLocation(NewLocation);
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// If character in TriggerBox then attack
	if(Cast<AFPS_Game_SimulationCharacter>(OtherActor))
	{
		bCanAttackPlayer = true;
		CurrentVelocity = FVector::ZeroVector;
	}
}

void AEnemy::OnHitEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If character out TriggerBox then don't attack
	if(Cast<AFPS_Game_SimulationCharacter>(OtherActor))
	{
		bCanAttackPlayer = false;
	}
	
}

void AEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{
	for(int i=0; i<UpdatedActors.Num(); ++i)
	{
		// Get info about sensed actor
		FActorPerceptionBlueprintInfo Info;
		AIPerceptionComponent->GetActorsPerception(UpdatedActors[i], Info);

		if (Info.LastSensedStimuli[0].WasSuccessfullySensed())
		{
			if(Cast<AFPS_Game_SimulationCharacter>(UpdatedActors[i]))
			{
				// Getting the direction between enemy and player
				FVector Direction = UpdatedActors[i]->GetActorLocation() - GetActorLocation(); // Found actor location - enemy location
				Direction.Z = 0.f;
				
				if(!bCanAttackPlayer)
				{
					CurrentVelocity = Direction.GetSafeNormal() * MovementSpeed;
				}

				// Turn to the player
				SetNewRotation(UpdatedActors[i]->GetActorLocation(), GetActorLocation());
			}
		}else
		{
			// Get back to base location
			FVector Direction = BaseLocation - GetActorLocation();
			Direction.Z = 0.f;

			if(Direction.SizeSquared2D() > 1.f)
			{
				CurrentVelocity = Direction.GetSafeNormal() * MovementSpeed;
				bBackToBaseLocation = true;

				SetNewRotation(BaseLocation, GetActorLocation());
			}
		}
		
	}
}

void AEnemy::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
	FVector NewDirection = TargetPosition - CurrentPosition;
	NewDirection.Z = 0.f;

	EnemyRotation = NewDirection.Rotation();

	SetActorRotation(EnemyRotation);
}

void AEnemy::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if(Health <= 0.f)
	{
		// Spawn next enemy
		if(Spawner)
			Spawner->TrigSpawnerWithTimer(SpawnRate);

		// Make character gain XP
		if(Character)
		{
			Character->TrigToGainXP();
			Character->IncrementKillScore();
		}
		
		Destroy();
	}
}
