// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackSky/Public/Controllers/ShipController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "BlackSky/Public/Characters/ShipCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AShipController::AShipController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AShipController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ShipMappingContext, 0);
	}
}

void AShipController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AShipController::OnMoveStarted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShipController::OnMoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AShipController::OnMoveReleased);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AShipController::OnMoveReleased);
	}
	
}

void AShipController::OnMoveStarted(const FInputActionValue& Value)
{
	StopMovement();
	UE_LOG(LogTemp, Warning, TEXT("Move Started"));
}

// Triggered every frame when the input is held down
void AShipController::OnMoveTriggered(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Move Triggered"));
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer*
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn != nullptr)
	{
		FVector WordlDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WordlDirection, 1.0, false);
	}
	
}

void AShipController::OnMoveReleased(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Move Released"));
	//If it was a short press
	if(FollowTime <= ShortPressThreshold)
	{
		//We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UE_LOG(LogTemp, Warning, TEXT("Move to %s"), *CachedDestination.ToString());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
	
}
