// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Ship.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FEngineInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	FVector Direction; // Dirección del motor (vector normalizado)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	FVector Position; // Posición del motor en el espacio

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Engine")
	double Force; // Posición del motor en el espacio
};

UCLASS()
class BLACKSKY_API AShip : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShip();

	FORCEINLINE class UStaticMeshComponent* GetShipMesh() const { return ShipMesh; }
	FORCEINLINE class UPawnMovementComponent* GetShipMovementComponent() const { return ShipMovementComponent; }
	FORCEINLINE class UCapsuleComponent* GetShipCollisionComponent() const { return ShipCollisionComponent; }
	FORCEINLINE class UCameraComponent* GetShipCamera() const { return ShipCamera; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void SetDestination(FVector Destination);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> FrontEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> BackEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> RightEngines;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> LeftEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> UpEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> DownEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> FrontRightEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> BackLeftEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> FrontLeftEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> BackRightEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> FrontTopEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> BackTopEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> FrontBotEngines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<FEngineInfo> BackBotEngines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	UNiagaraSystem* EngineSystem;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShipMesh;


	UPROPERTY(VisibleAnywhere)
	UPawnMovementComponent* ShipMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* ShipCollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ShipCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	FVector initialPos = FVector(0.0f, 0.0f, 0.0f);
	FVector finalPos = FVector(1000.0f, 0.0f, 0.0f);

	double acceleration = 10000;

	double maxSpeed = 300;

	double velocity = 0;

	bool bMoveX = false;
	bool bMoveY = false;
	bool bMoveZ = false;

	bool bBreakX = false;
	bool bBreakY = false;
	bool bBreakZ = false;

	bool bRotateX = false;
	bool bRotateY = false;
	bool bRotateZ = false;
	
	bool bBreakRotationX = false;
	bool bBreakRotationY = false;
	bool bBreakRotationZ = false;


	// Variable de tiempo acumulado
	float AccumulatedTime = 0.0f;
	// Duración deseada para aplicar la fuerza (en segundos)
	float DesiredDuration = 1.0f;

	FVector VectorFrenado;
	float DistanciaFrenado = 0.0f;
	float DistanciaFrenadoReal = 0.0f;
	

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
