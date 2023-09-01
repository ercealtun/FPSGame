// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AFPS_Game_SimulationCharacter;
class UInputAction;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_GAME_SIMULATION_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFPS_Game_SimulationProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* FireAction;
	
	/** Damage Increaser Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* DamageIncreaserAction;

	/** Capacity Growth Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* CapacityGrowthAction;

	/** Pierce Shot Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* PierceShotAction;

public:
	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AFPS_Game_SimulationCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintImplementableEvent, Category="Weapon")
	void TriggerMuzzleFlashAnimation();

	// Add ammo 
	void AddAmmo(int AmmoAmount);

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AFPS_Game_SimulationCharacter* Character;

private:
	// Explosion effect when projectile hit something
	UPROPERTY(EditAnywhere, Category="Particle")
	UParticleSystem* ExplosionEffect;

public:
	/* WEAPON DATA */

	// Default ammo amount of weapon can take
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	int DefaultAmmoCapacity = 50;
	// Current total ammo amount of weapon can take
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	int CurrentAmmoLimit = 50;
	// The total amount of ammo being carried for the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	int CurrentAmmo = 47;
	// Bullet distance amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	int BulletRange = 2000.f;
	// Determines whether total ammo is zero or not
	bool bIsTotalAmmoZero = false;
	// Default damage value for every fire event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	float DefaultDamageAmount = 20.f;
	// Current damage value for every fire event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	float CurrentDamageAmount = 20.f;

protected:

	/* TALENT DATA*/

	enum  Talents
	{
		DamageIncreaser,
		CapacityGrowth,
		PierceShot,
	};

	// Damage Increaser amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Talents")
	float TalentDamageIncreaserAmount = 30.f;
	// Capacity Growth amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Talents")
	int TalentCapacityGrowthAmount = 70;

	// Determines if the character has used their talent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Talents")
	bool bHasUsedTalentPierceShot = false;

	// The durations and timers of the character's talent
	float TalentDamageIncreaserDuration = 6.f;
	float TalentCapacityGrowthDuration = 10.f;
	float TalentPierceShotDuration = 5.5f;
	
	FTimerHandle TalentDamageIncreaserTimerHandle;
	FTimerHandle TalentCapacityGrowthTimerHandle;
	FTimerHandle TalentPierceShotTimerHandle;

	// Determines whether the character can use the talent
	bool CanUseTalentDamageIncreaser();
	bool CanUseTalentCapacityGrowth();
	bool CanUseTalentPierceShot();

	// Talent functions
	void UseTalent(Talents Talent);
	void TriggerTalentDamageIncreaser();
	void TriggerTalentCapacityGrowth();
	void TriggerTalentPierceShot();
	void ResetTalentDamageIncreaser();
	void ResetTalentCapacityGrowth();
	void ResetTalentPierceShot();
	
	
};
