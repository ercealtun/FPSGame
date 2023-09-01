// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TP_WeaponComponent.h"
#include "FPS_Game_SimulationCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AFPS_Game_SimulationCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Super Spring Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SuperSprintAction;

	/** High Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction*  HighJumpAction;

	/** Maxi Health Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MaxiHealthAction;
public:
	AFPS_Game_SimulationCharacter();

protected:
	virtual void BeginPlay();

public:
		
	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	// Setted in BP_PickUp_Rifle event graph,
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentWeapon;
	
	/* Get rifle ref */
	UTP_WeaponComponent* GetRifle() const;


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Allows the character to begin sprinting
	void Sprint();

	// Allows the character to stop sprinting
	void StopSprinting();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	// Default sprint value
	UPROPERTY(EditAnywhere, Category="Character")
	float MaxSprintSpeed = 1050.f;
	
	// Default jump value
	UPROPERTY(EditAnywhere, Category="Character")
	float MaxJumpOnZ = 420.f;

	// Current Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character", meta=(AllowPrivateAccess))
	float CurrentHealth = 100.f;
	
	// Max health to have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character", meta=(AllowPrivateAccess))
	float MaxHealth = 100.f;

public:
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE void SetCurrentHealth(float NewHealth) { CurrentHealth =  NewHealth; }

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }


protected:
	/** TALENT DATA **/

	enum Talents
	{
		SuperSprint,
		HighJump,
		MaxiHealth
	};

	// Super Sprint amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Talents")
	float TalentSuperSprintAmount = 1600.f;
	// High Jump amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Talents")
	float TalentHighJumpAmount= 1000.f;
	// Max Health amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Talents")
	float TalentMaxiHealthAmount = 100.f;

	// The durations and timers of the character's talent
	float TalentSuperSprintDuration = 5.f;
	float TalentHighJumpDuration = 5.f;
	
	FTimerHandle TalentSuperSprintTimerHandle;
	FTimerHandle TalentHighJumpTimerHandle;

	// Determines whether the character can use the talent
	bool CanUseTalentSuperSprint();
	bool CanUseTalentHighJump();
	bool CanUseTalentMaxiHealth();

	// Talent functions
	void UseTalent(Talents Talent);
	void TriggerTalentSuperSprint();
	void TriggerTalentHighJump();
	void TriggerTalentMaxiHealth();
	void ResetTalentSuperSprint();
	void ResetTalentHighJump();

public:
	// It will trigger left white colored text in CharacterHUD to inform player whether s/he used talent or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TalentInfo = "";
	
	FORCEINLINE void ResetScreenTalentInfo() { TalentInfo = ""; }

	FTimerHandle TalentInfoTimerHandle;

public:
	// Getting damage by enemy
	UFUNCTION(BlueprintCallable)
	void DealDamage(float DamageAmount);

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerGameOverScreen();

private:
	// Number of killed enemies
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	int KillScore = 0;

public:
	// Set kill score
	FORCEINLINE void IncrementKillScore(){ KillScore++; }
	// Get kill score
	FORCEINLINE int GetKillScore() const { return KillScore; }
	
	// Character level 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int CharacterLevel = 0;
	
private:
	// Level passing amount data
	TArray<int> LevelBounds;

public:
	// Max amount of level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	int MaxLevel = 10;

	UPROPERTY(BlueprintReadOnly)
	// Character's current level
	int CurrentLevel = 0;
	
	// Current experience points
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int CurrentExperiencePoints = 0;

	// Current experience points
	UPROPERTY(EditAnywhere, Category="Character")
	int ExperiencePointsAmount = 75;
	
	// Current talent points
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int CurrentTalentPoints = 0;

public:
	// Character gains experience points (enemy class triggers this function)
	void TrigToGainXP();

private:
	// Level pass power assigning
	void AssignLevels();

	// Check if player leveled up
	void CheckIfLeveledUp();
};



