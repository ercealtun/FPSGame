// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Game_SimulationCharacter.h"
#include "FPS_Game_SimulationProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AFPS_Game_SimulationCharacter

AFPS_Game_SimulationCharacter::AFPS_Game_SimulationCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	GetCharacterMovement()->JumpZVelocity = MaxJumpOnZ;
}

void AFPS_Game_SimulationCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Assign level passing values
	AssignLevels();

}

//////////////////////////////////////////////////////////////////////////// Input

void AFPS_Game_SimulationCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPS_Game_SimulationCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPS_Game_SimulationCharacter::Look);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFPS_Game_SimulationCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFPS_Game_SimulationCharacter::StopSprinting);

		// Talents
		EnhancedInputComponent->BindAction(SuperSprintAction, ETriggerEvent::Completed, this, &AFPS_Game_SimulationCharacter::TriggerTalentHighJump);
		EnhancedInputComponent->BindAction(HighJumpAction, ETriggerEvent::Completed, this, &AFPS_Game_SimulationCharacter::TriggerTalentSuperSprint);
		EnhancedInputComponent->BindAction(MaxiHealthAction, ETriggerEvent::Completed, this, &AFPS_Game_SimulationCharacter::TriggerTalentMaxiHealth);
	}
}

void AFPS_Game_SimulationCharacter::DealDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f) return;
	
	CurrentHealth -= DamageAmount;

	if(CurrentHealth <= 0.f)
	{
		TriggerGameOverScreen();
		
		// Mesh1P->SetVisibility(false);
		// // TODO.. set visibility false for gun mesh
		// DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	}
}

void AFPS_Game_SimulationCharacter::TrigToGainXP()
{
	// Gain XP & Check if leveled up
	CurrentExperiencePoints += ExperiencePointsAmount;
	CheckIfLeveledUp();
	
}

void AFPS_Game_SimulationCharacter::AssignLevels()
{
	// A patterned leveling system
	int NeededXP = 100;
	
	for(int i=0; i<MaxLevel; ++i)
	{
		LevelBounds.Add(NeededXP);
		NeededXP *= 2;
	}
	
}

void AFPS_Game_SimulationCharacter::CheckIfLeveledUp()
{
	for(int i=CurrentLevel; i<LevelBounds.Num(); ++i)
	{
		// Level up & gain talent point
		if(CurrentExperiencePoints >= LevelBounds[i])
		{
			CurrentLevel = i+1;
			CurrentTalentPoints++;
		}
	}
}

UTP_WeaponComponent* AFPS_Game_SimulationCharacter::GetRifle() const
{
	TArray<UActorComponent*> WeaponComponents = CurrentWeapon->GetComponentsByTag(UTP_WeaponComponent::StaticClass(),FName("TP_Weapon"));
	
	for (UActorComponent* WeaponComponent : WeaponComponents)
	{
		if (WeaponComponent)
		{
			if (Cast<UTP_WeaponComponent>(WeaponComponent))
			{
				return Cast<UTP_WeaponComponent>(WeaponComponent);
			}
		}
	}

	return nullptr;
}

void AFPS_Game_SimulationCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPS_Game_SimulationCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPS_Game_SimulationCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
}

void AFPS_Game_SimulationCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AFPS_Game_SimulationCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFPS_Game_SimulationCharacter::GetHasRifle()
{
	return bHasRifle;
}

bool AFPS_Game_SimulationCharacter::CanUseTalentSuperSprint()
{
	// Ok, player can use the talent
	if(CurrentLevel >= 5 && CurrentTalentPoints >= 1)
	{
		return true;
	}
	return false;
}

bool AFPS_Game_SimulationCharacter::CanUseTalentHighJump()
{
	// Ok, player can use the talent
	if(CurrentLevel >= 5 && CurrentTalentPoints >= 1)
	{
		return true;
	}
	return false;
}

bool AFPS_Game_SimulationCharacter::CanUseTalentMaxiHealth()
{
	// Ok, player can use the talent
	if(CurrentLevel >= 5 && CurrentTalentPoints >= 1)
	{
		return true;
	}
	return false;
}

void AFPS_Game_SimulationCharacter::UseTalent(Talents Talent)
{
	switch (Talent)
	{
	case SuperSprint:
		if(CanUseTalentSuperSprint())
		{
			GetCharacterMovement()->MaxWalkSpeed = TalentSuperSprintAmount;
			TalentInfo = "SuperSprint Activated...";
			GetWorld()->GetTimerManager().SetTimer(TalentSuperSprintTimerHandle, this,
				&AFPS_Game_SimulationCharacter::ResetTalentSuperSprint, TalentSuperSprintDuration, false);
			CurrentTalentPoints--;
		}else
		{
			TalentInfo = "Can't use right now!!!";
			GetWorldTimerManager().SetTimer(TalentInfoTimerHandle,
				this, &AFPS_Game_SimulationCharacter::ResetScreenTalentInfo, TalentSuperSprintDuration, false);
		}
		break;
	case HighJump:
		if(CanUseTalentHighJump())
		{
			GetCharacterMovement()->JumpZVelocity = TalentHighJumpAmount;
			TalentInfo = "HighJump Activated...";
			GetWorld()->GetTimerManager().SetTimer(TalentHighJumpTimerHandle, this,
				&AFPS_Game_SimulationCharacter::ResetTalentHighJump, TalentHighJumpDuration, false);
			CurrentTalentPoints--;
		}else
		{
			TalentInfo = "Can't use right now!!!";
			GetWorldTimerManager().SetTimer(TalentInfoTimerHandle,
				this, &AFPS_Game_SimulationCharacter::ResetScreenTalentInfo, TalentHighJumpDuration, false);
		}
		break;
	case MaxiHealth:
		if(CanUseTalentMaxiHealth())
		{
			TalentInfo = "MaxiHealth Activated...";
			CurrentHealth = TalentMaxiHealthAmount;
			CurrentTalentPoints--;
		}else
		{
			TalentInfo = "Can't use right now!!!";
			GetWorldTimerManager().SetTimer(TalentInfoTimerHandle,
				this, &AFPS_Game_SimulationCharacter::ResetScreenTalentInfo, 5.f, false);
		}
		break;
	}
}

void AFPS_Game_SimulationCharacter::ResetTalentSuperSprint()
{
	TalentInfo = "";
	StopSprinting();
}

void AFPS_Game_SimulationCharacter::ResetTalentHighJump()
{
	TalentInfo = "";
	GetCharacterMovement()->JumpZVelocity = MaxJumpOnZ;
}


void AFPS_Game_SimulationCharacter::TriggerTalentSuperSprint()
{
	UseTalent(Talents::SuperSprint);
}

void AFPS_Game_SimulationCharacter::TriggerTalentHighJump()
{
	UseTalent(Talents::HighJump);
}

void AFPS_Game_SimulationCharacter::TriggerTalentMaxiHealth()
{
	UseTalent(Talents::MaxiHealth);
}
