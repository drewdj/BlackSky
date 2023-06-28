// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ShipController.generated.h"

class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;


UCLASS()
class BLACKSKY_API AShipController : public APlayerController
{
	GENERATED_BODY()

public:
	AShipController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* ShipMappingContext;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

protected:

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	/** Input handlers for SetDestination action. */
	void OnMoveStarted(const FInputActionValue& Value);
	void OnMoveTriggered(const FInputActionValue& Value);
	void OnMoveReleased(const FInputActionValue& Value);


private:
	FVector CachedDestination;

	float FollowTime;	
};
