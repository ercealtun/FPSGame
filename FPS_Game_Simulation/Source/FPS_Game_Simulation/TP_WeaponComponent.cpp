// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"

#include "Enemy.h"
#include "FPS_Game_SimulationCharacter.h"
#include "FPS_Game_SimulationProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "ProfilingDebugging/CookStats.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		// If clip got ammo in it
		if(CurrentAmmo > 0)
		{
			bIsTotalAmmoZero = false;
			// Shooting using line tracing
			FVector ShootStartLocation = GetChildComponent(1)->GetComponentLocation(); 	// Getting Shoot Point scene component location
			const FVector ForwardVector = Character->GetFirstPersonCameraComponent()->GetForwardVector();
			FVector ShootEndLocation = ShootStartLocation + ForwardVector * BulletRange;
			FCollisionQueryParams QueryParams;
			FHitResult HitResult;
			QueryParams.AddIgnoredComponent(this);
			World->LineTraceSingleByChannel(HitResult, ShootStartLocation, ShootEndLocation,ECC_Camera);

			// If found actor
			if(HitResult.GetActor())
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionEffect, HitResult.GetActor()->GetActorLocation());

				AEnemy* EnemySingleTraced = Cast<AEnemy>(HitResult.GetActor());
				// If found actor is enemy, damage
				if(EnemySingleTraced)
				{
					EnemySingleTraced->DealDamage(CurrentDamageAmount);
				}
			}

			// Pierce shot activated firing
			if(bHasUsedTalentPierceShot)
			{
				TArray<FHitResult> HitResults;
				World->LineTraceMultiByChannel(HitResults, ShootStartLocation, ShootEndLocation,ECC_Camera);
				for (FHitResult Hit : HitResults)
				{
					// If found actor
					if(Hit.GetActor())
					{
						UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionEffect, Hit.GetActor()->GetActorLocation());
						AEnemy* EnemyMultiTraced = Cast<AEnemy>(Hit.GetActor());
						// If found actor is enemy, damage
						if(EnemyMultiTraced)
						{
							EnemyMultiTraced->DealDamage(CurrentDamageAmount);
						}
					}
				}
			}
			CurrentAmmo--;
		}
		// If no ammo remains
		else
		{
			bIsTotalAmmoZero = true;
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		if(!bIsTotalAmmoZero)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
		}
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		if(!bIsTotalAmmoZero)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

void UTP_WeaponComponent::AddAmmo(int AmmoAmount)
{
	CurrentAmmo += AmmoAmount;
	bIsTotalAmmoZero = false;
}

void UTP_WeaponComponent::AttachWeapon(AFPS_Game_SimulationCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);

			// Talents
			EnhancedInputComponent->BindAction(DamageIncreaserAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::TriggerTalentDamageIncreaser);
			EnhancedInputComponent->BindAction(CapacityGrowthAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::TriggerTalentCapacityGrowth);
			EnhancedInputComponent->BindAction(PierceShotAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::TriggerTalentPierceShot);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

bool UTP_WeaponComponent::CanUseTalentDamageIncreaser()
{
	// Ok player can use the talent
	if(Character->CurrentLevel >= 5 && Character->CurrentTalentPoints >= 1)
	{
		return true;
	}
	return false;

}

bool UTP_WeaponComponent::CanUseTalentCapacityGrowth()
{
	// Ok player can use the talent
	if(Character->CurrentLevel >= 5 && Character->CurrentTalentPoints >= 1)
	{
		return true;
	}
	return false;
}

bool UTP_WeaponComponent::CanUseTalentPierceShot()
{
	// Ok player can use the talent
	if(Character->CurrentLevel >= 1 && Character->CurrentTalentPoints >= 3)
	{
		return true;
	}
	return false;

}

void UTP_WeaponComponent::UseTalent(Talents Talent)
{
	switch (Talent)
	{
	case DamageIncreaser:
		if(CanUseTalentDamageIncreaser())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("DamageIncreaser"));
			CurrentDamageAmount = TalentDamageIncreaserAmount;
			Character->TalentInfo = "DamageIncreaser Activated!";
			Character->CurrentTalentPoints -= 1;
		}else
		{
			Character->TalentInfo = "Can't use right now!!!";
		}
		GetWorld()->GetTimerManager().SetTimer(TalentDamageIncreaserTimerHandle, this,
			&UTP_WeaponComponent::ResetTalentDamageIncreaser, TalentDamageIncreaserDuration, false);
		break;
		
	case CapacityGrowth:
		if(CanUseTalentCapacityGrowth())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CapacityGrowth"));
			CurrentAmmoLimit = TalentCapacityGrowthAmount;
			Character->TalentInfo = "CapacityGrowth Activated!";
			Character->CurrentTalentPoints -= 1;
		}else
		{
			Character->TalentInfo = "Can't use right now!!!";
		}

		GetWorld()->GetTimerManager().SetTimer(TalentCapacityGrowthTimerHandle, this,
				&UTP_WeaponComponent::ResetTalentCapacityGrowth, TalentCapacityGrowthDuration, false);
		break;
	
	case PierceShot:
		if(CanUseTalentPierceShot())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("PierceShot"));
			Character->TalentInfo = "PierceShot Activated!";
			bHasUsedTalentPierceShot = true;
			Character->CurrentTalentPoints -= 3;
		}else
		{
			Character->TalentInfo = "Can't use right now!!!";
			UE_LOG(LogTemp, Error, TEXT("freeband"))
		}

		GetWorld()->GetTimerManager().SetTimer(TalentPierceShotTimerHandle, this,
				&UTP_WeaponComponent::ResetTalentPierceShot, TalentPierceShotDuration, false);
		break;
	}
}

void UTP_WeaponComponent::TriggerTalentDamageIncreaser()
{
	UseTalent(Talents::DamageIncreaser);
}

void UTP_WeaponComponent::TriggerTalentCapacityGrowth()
{
	UseTalent(Talents::CapacityGrowth);
}

void UTP_WeaponComponent::TriggerTalentPierceShot()
{
	UseTalent(Talents::PierceShot);
}

void UTP_WeaponComponent::ResetTalentDamageIncreaser()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ResetTalentDamageIncreaser"));
	
	CurrentDamageAmount = DefaultDamageAmount;
	
	Character->TalentInfo = "";
}

void UTP_WeaponComponent::ResetTalentCapacityGrowth()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ResetTalentCapacityGrowth"));
	
	CurrentAmmoLimit = DefaultAmmoCapacity;
	CurrentAmmo = DefaultAmmoCapacity;

	Character->TalentInfo = "";

}

void UTP_WeaponComponent::ResetTalentPierceShot()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ResetTalentPierceShot"));
	Character->TalentInfo = "";
	bHasUsedTalentPierceShot = false;
}
