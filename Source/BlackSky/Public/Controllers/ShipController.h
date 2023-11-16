// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ShipController.generated.h"

/**
 * 
 */
UCLASS()
class BLACKSKY_API AShipController : public APlayerController
{
	GENERATED_BODY()

private:
	
	bool bLookLock = false;

public:
	
	AShipController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* ShipMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* BreakAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class  UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class  UInputAction* LookLockAction;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void OnSliderValueChanged(float Value);


protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlaneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	float ZValue = 0;

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void LookLock(const FInputActionValue& Value);

	void StartMove();



	virtual void SetupInputComponent() override;
	
};
