// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ship.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Interfaces/IPluginManager.h"


float CalcularMomentoInercia(float masa, float lado) {
	// Utilizamos la fórmula I = (1/6) * m * (a^2 + b^2)
	float momentoInercia = (1.0f / 6.0f) * masa * (lado * lado + lado * lado);
	return momentoInercia;
}

float CalcularDistanciaFrenadoAngular(const TArray<FEngineInfo>& Engines1, const TArray<FEngineInfo>& Engines2, FVector Intertia, FVector CenterOfMass,FVector AngularVelocity , float DeltaTime)
{

	FVector AcumulatedTorque = FVector(0,0,0);

	for (const FEngineInfo& Engine : Engines1)
	{
		FVector VectorDistancia = Engine.Position - CenterOfMass;

		FVector Fuerza = Engine.Direction * Engine.Force * DeltaTime;

		FVector Torque = FVector::CrossProduct(VectorDistancia, Fuerza);

		AcumulatedTorque += Torque;
		
	}

	for (const FEngineInfo& Engine : Engines2)
	{
		FVector VectorDistancia = Engine.Position - CenterOfMass;

		FVector Fuerza = Engine.Direction * Engine.Force * DeltaTime;

		FVector Torque = FVector::CrossProduct(VectorDistancia, Fuerza);

		AcumulatedTorque += Torque;
	}

	FVector AngularAcceleration = (AcumulatedTorque) / Intertia;

	float InitialAngularVelocity = abs(AngularVelocity.Z); // Velocidad angular inicial en radianes por segundo
	float AngularAcceleration2 = -abs(AngularAcceleration.Z); // Aceleración angular en radianes por segundo al cuadrado

	float DeltaVelocity = -abs(InitialAngularVelocity); // Cambio en la velocidad angular (frenando hasta cero)
	float BrakingTime = DeltaVelocity / AngularAcceleration2; // Tiempo de frenado

	float BreakingDistance = abs(InitialAngularVelocity) * BrakingTime + 0.5f * AngularAcceleration2 * FMath::Square(BrakingTime); // Distancia de frenado angular

	return BreakingDistance;
	
}

float CalcularDistanciaFrenadoAngularY(const TArray<FEngineInfo>& Engines1, const TArray<FEngineInfo>& Engines2, FVector Intertia, FVector CenterOfMass,FVector AngularVelocity , float DeltaTime)
{

	FVector AcumulatedTorque = FVector(0,0,0);

	for (const FEngineInfo& Engine : Engines1)
	{
		FVector VectorDistancia = Engine.Position - CenterOfMass;

		FVector Fuerza = Engine.Direction * Engine.Force * DeltaTime;

		FVector Torque = FVector::CrossProduct(VectorDistancia, Fuerza);

		AcumulatedTorque += Torque;
		
	}

	for (const FEngineInfo& Engine : Engines2)
	{
		FVector VectorDistancia = Engine.Position - CenterOfMass;

		FVector Fuerza = Engine.Direction * Engine.Force * DeltaTime;

		FVector Torque = FVector::CrossProduct(VectorDistancia, Fuerza);

		AcumulatedTorque += Torque;
	}

	FVector AngularAcceleration = (AcumulatedTorque) / Intertia;

	float InitialAngularVelocity = abs(AngularVelocity.Y); // Velocidad angular inicial en radianes por segundo
	float AngularAcceleration2 = -abs(AngularAcceleration.Y); // Aceleración angular en radianes por segundo al cuadrado

	float DeltaVelocity = -abs(InitialAngularVelocity); // Cambio en la velocidad angular (frenando hasta cero)
	float BrakingTime = DeltaVelocity / AngularAcceleration2; // Tiempo de frenado

	float BreakingDistance = abs(InitialAngularVelocity) * BrakingTime + 0.5f * AngularAcceleration2 * FMath::Square(BrakingTime); // Distancia de frenado angular


	return BreakingDistance;
	
}

float CalculateSignedAngleBetweenVectors(const FVector& v1, const FVector& v2)
{
	// Get the normalized versions of the vectors
	FVector normalizedV1 = v1.GetSafeNormal();
	FVector normalizedV2 = v2.GetSafeNormal();

	// Calculate the dot product between the vectors
	float dotProduct = FVector::DotProduct(normalizedV1, normalizedV2);

	// Calculate the cross product between the vectors
	FVector crossProduct = FVector::CrossProduct(normalizedV1, normalizedV2);

	// Calculate the angle in radians using the atan2 function
	float angleRadians = FMath::Atan2(crossProduct.Size(), dotProduct);

	// Convert the angle from radians to degrees
	float angleDegrees = FMath::RadiansToDegrees(angleRadians);

	// Determine the sign of the angle by checking the z-component of the cross product
	float sign = FMath::Sign(crossProduct.Z);

	// Apply the sign to the angle
	angleDegrees *= sign;

	return angleDegrees;
}

// Sets default values
AShip::AShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;	

	ShipMovementComponent = CreateDefaultSubobject<UPawnMovementComponent>(TEXT("ShipMovementComponent"));

	ShipCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ShipCollisionComponent"));
	ShipCollisionComponent->SetupAttachment(RootComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	ShipCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ShipCamera"));
	ShipCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	ShipCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm



}

void AShip::SetDestination(FVector destination)
{
	finalPos = destination;

	bMoveX = false;
	bMoveY = false;
	bMoveZ = false;
	bBreakX = false;
	bBreakY = false;
	bBreakZ = false;

	bRotateX = false;
	bRotateY = true;
	bRotateZ = false;
	
	bBreakRotationX = false;
	bBreakRotationY = false;
	bBreakRotationZ = false;
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every framex
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	FVector Position = GetActorLocation();  // Obtener la posición actual de la nave
	FVector DistanceToObjective = finalPos - Position;  // Calcular la distancia al objetivo
	FVector DesiredDirection = DistanceToObjective.GetSafeNormal();
	
	FVector Velocity = ShipMesh->GetPhysicsLinearVelocity();
	FVector ForwardVector = ShipMesh->GetForwardVector();
	FVector RightVector = ShipMesh->GetRightVector();
	FVector UpVector = ShipMesh->GetUpVector();

	float LocalVelocityX = FVector::DotProduct(Velocity, ForwardVector);
	float LocalVelocityY = FVector::DotProduct(Velocity, RightVector);
	float LocalVelocityZ = FVector::DotProduct(Velocity, UpVector);

	float LocalDistanceX = FVector::DotProduct(DistanceToObjective, ForwardVector);
	float LocalDistanceY = FVector::DotProduct(DistanceToObjective, RightVector);
	float LocalDistanceZ = FVector::DotProduct(DistanceToObjective, UpVector);

	FVector TargetLocal = FVector(LocalDistanceX, LocalDistanceY, LocalDistanceZ);

	FVector AngularVelocity = ShipMesh->GetPhysicsAngularVelocityInRadians();

	// Obtener la rotación actual del cubo
	FRotator CubeRotation = GetActorRotation();

	// Convertir el FRotator en un FQuat (cuaternion)
	FQuat CubeQuaternion = CubeRotation.Quaternion();

	// Obtener el cuaternion inverso (conjugado) para transformar del sistema global al sistema local
	FQuat InverseQuaternion = CubeQuaternion.Inverse();

	// Transformar la velocidad angular global en sus componentes locales
	FVector LocalAngularVelocity = InverseQuaternion.RotateVector(AngularVelocity);

	AngularVelocity = LocalAngularVelocity;
	
	FVector Acceleration = acceleration * ShipMesh->GetForwardVector() * DeltaTime;  // Calcular la aceleración actual de la nave	

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Velocidad angular: %.2f, %.2f, %.2f grados/s"), AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Velocidad angular local: %.2f, %.2f, %.2f grados/s"), LocalAngularVelocity.X, LocalAngularVelocity.Y, LocalAngularVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Objetivo: %.2f, %.2f, %.2f unidades"), finalPos.X, finalPos.Y, finalPos.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Posición: %.2f, %.2f, %.2f unidades"), Position.X, Position.Y, Position.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Velocidad: %.2f, %.2f, %.2f unidades/s"), LocalVelocityX, LocalVelocityY, LocalVelocityZ));

	//print bmove and bbreak
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: MoveX: %s, BreakX: %s"), bMoveX ? TEXT("true") : TEXT("false"), bBreakX ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: MoveY: %s, BreakY: %s"), bMoveY ? TEXT("true") : TEXT("false"), bBreakY ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: MoveZ: %s, BreakZ: %s"), bMoveZ ? TEXT("true") : TEXT("false"), bBreakZ ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: RotateX: %s, BreakRotateX: %s"), bRotateX ? TEXT("true") : TEXT("false"), bBreakRotationX ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: RotateY: %s, BreakRotateY: %s"), bRotateY ? TEXT("true") : TEXT("false"), bBreakRotationY ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, FString::Printf(TEXT("ESTADO: RotateZ: %s, BreakRotateZ: %s"), bRotateZ ? TEXT("true") : TEXT("false"), bBreakRotationZ ? TEXT("true") : TEXT("false")));

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Objetivo global: %.2f, %.2f, %.2f unidades"), finalPos.X, finalPos.Y, finalPos.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Objetivo Local: %.2f, %.2f, %.2f unidades"), LocalDistanceX, LocalDistanceY, LocalDistanceZ));

	FVector TargetVector = finalPos - ShipMesh->GetCenterOfMass();

	// Obtener la normal del plano horizontal local del cubo (vector "Up" del cubo)
	FVector CubeNormal = ShipMesh->GetUpVector();





	//Proyectar el vector objetivo en el plano vertical local del cubo
	FVector TargetVerticalVector = TargetVector - TargetVector.ProjectOnToNormal(ShipMesh->GetRightVector());
	
	// Proyectar el vector objetivo en el plano horizontal local del cubo
	TargetVector = TargetVector - TargetVector.ProjectOnToNormal(CubeNormal);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Purple, FString::Printf(TEXT("Target horizontal: %.2f, %.2f, %.2f unidades"), TargetVector.X, TargetVector.Y, TargetVector.Z));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Purple, FString::Printf(TEXT("Target Vertical: %.2f, %.2f, %.2f unidades"), TargetVerticalVector.X, TargetVerticalVector.Y, TargetVerticalVector.Z));

	FVector Forward = ShipMesh->GetForwardVector();

	TargetVector.Normalize();
	TargetVerticalVector.Normalize();
	Forward.Normalize();

	float AngleRadians = FMath::Acos(FVector::DotProduct(TargetVector, Forward));

	float Angle2Radians = CalculateSignedAngleBetweenVectors(TargetVector, Forward);

	float AngleVerticalRadians = FMath::Acos(FVector::DotProduct(TargetVerticalVector, Forward));

	FVector sign = FVector::CrossProduct(TargetVerticalVector, Forward);

	AngleVerticalRadians *= FMath::Sign(sign.Y);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Angulo: %.2f grados"), FMath::RadiansToDegrees(AngleRadians)));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Angulo2: %.2f grados"), Angle2Radians));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("AnguloVertical: %.2f grados"), FMath::RadiansToDegrees(AngleVerticalRadians)));

	float TotalBreakingForceX = 0.0f;
	float TotalBreakingForceY = 0.0f;
	float TotalBreakingForceZ = 0.0f;

	if (bRotateY)
	{
		if (AngleVerticalRadians > 0 )
		{

			float BreakingDistance = CalcularDistanciaFrenadoAngularY(FrontTopEngines, BackBotEngines, ShipMesh->GetInertiaTensor(), ShipMesh->GetCenterOfMass(), AngularVelocity, DeltaTime);
			
			for (const FEngineInfo& Engine : FrontBotEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackTopEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			if(abs(BreakingDistance) > abs(AngleVerticalRadians))
			{
				bRotateY = false;
				bBreakRotationY = true;
			}
			
			
		}
		else if(AngleVerticalRadians < 0 )
		{

			float BreakingDistance = CalcularDistanciaFrenadoAngularY(FrontBotEngines, BackTopEngines, ShipMesh->GetInertiaTensor(), ShipMesh->GetCenterOfMass(), AngularVelocity, DeltaTime);
			
			for (const FEngineInfo& Engine : FrontTopEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackBotEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			if(abs(BreakingDistance) > abs(AngleVerticalRadians))
			{
				bRotateY = false;
				bBreakRotationY = true;
			}
		}
		
	}
	
	if (bRotateX)
	{

		if(Angle2Radians > 0)
		{
			float BreakingDistance = CalcularDistanciaFrenadoAngular(FrontLeftEngines, BackRightEngines, ShipMesh->GetInertiaTensor(), ShipMesh->GetCenterOfMass(), AngularVelocity, DeltaTime);

			for (const FEngineInfo& Engine : FrontRightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackLeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			if(abs(BreakingDistance) > abs(AngleRadians))
			{
				DistanciaFrenado = BreakingDistance;
				VectorFrenado = ShipMesh->GetForwardVector();
				bRotateX = false;
				bBreakRotationX = true;
			}
		}
		else
		{
			float BreakingDistance = CalcularDistanciaFrenadoAngular(FrontRightEngines, BackLeftEngines, ShipMesh->GetInertiaTensor(), ShipMesh->GetCenterOfMass(), AngularVelocity, DeltaTime);

			for (const FEngineInfo& Engine : FrontLeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackRightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			if(abs(BreakingDistance) > abs(AngleRadians))
			{
				DistanciaFrenado = BreakingDistance;
				VectorFrenado = ShipMesh->GetForwardVector();
				bRotateX = false;
				bBreakRotationX = true;
			}
		}
		
	}

	if(bBreakRotationY)
	{
		if (AngularVelocity.Y < 0)
		{
			for (const FEngineInfo& Engine : FrontTopEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackBotEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

		}
		else if (AngularVelocity.Y > 0)
		{
			for (const FEngineInfo& Engine : FrontBotEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			for (const FEngineInfo& Engine : BackTopEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}

			
		}
	}

	if (bBreakRotationX)
	{
		if (AngularVelocity.Z < 0)
		{
			for (const FEngineInfo& Engine : FrontLeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

			}
		
			for (const FEngineInfo& Engine : BackRightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

			}
		}
		else if (AngularVelocity.Z > 0)
		{
			for (const FEngineInfo& Engine : FrontRightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

			}
		
			for (const FEngineInfo& Engine : BackLeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

			}
			
		}else
		{
			bRotateY = true;
		}

		float AngleRadians2 = FMath::Acos(FVector::DotProduct(VectorFrenado, ShipMesh->GetForwardVector()));

		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("FRENADO REAL: %.2f grados"),  FMath::RadiansToDegrees(AngleRadians2)));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("FRENADO: %.2f grados"), FMath::RadiansToDegrees(DistanciaFrenado)));

		
	}
	
	if (bMoveX)
	{
		if (LocalDistanceX > 0)
		{
			for (const FEngineInfo& Engine : FrontEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceX += Engine.Force;

				/*UNiagaraComponent* MotorComp = UNiagaraFunctionLibrary::SpawnSystemAttached(EngineSystem, ShipMesh, FName("Engine"), Engine.Position, FRotator(90,0,0), EAttachLocation::KeepRelativeOffset, true);

				MotorComp->SetNiagaraVariableFloat(FString("Power"), 3);*/


			}		

			float StoppingDistanceX = (LocalVelocityX * LocalVelocityX) / (2.0f * TotalBreakingForceX * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoX: %.2f, unidades"), StoppingDistanceX));

			if (StoppingDistanceX > LocalDistanceX)
			{
				bMoveX = false;
				bBreakX = true;
			}
		}
		else
		{
			for (const FEngineInfo& Engine : BackEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceX += Engine.Force;
			}
			
			float StoppingDistanceX = (LocalVelocityX * LocalVelocityX) / (2.0f * TotalBreakingForceX * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoX: %.2f, unidades"), StoppingDistanceX));

			if (abs(StoppingDistanceX) > abs(LocalDistanceX))
			{
				bMoveX = false;
				bBreakX = true;
			}
			
		}
		
	}

	if(bMoveY)
	{
		if (LocalDistanceY > 0)
		{
			for (const FEngineInfo& Engine : RightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceY += Engine.Force;
			}

			float StoppingDistanceY = (LocalVelocityY * LocalVelocityY) / (2.0f * TotalBreakingForceY * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoY: %.2f, unidades"), StoppingDistanceY));

			if (StoppingDistanceY > LocalDistanceY)
			{
				bMoveY = false;
				bBreakY = true;
			}
		}
		else
		{
			for (const FEngineInfo& Engine : LeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceY += Engine.Force;
			}

			float StoppingDistanceY = (LocalVelocityY * LocalVelocityY) / (2.0f * TotalBreakingForceY * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoY: %.2f, unidades"), StoppingDistanceY));

			if (abs(StoppingDistanceY) > abs(LocalDistanceY))
			{
				bMoveY = false;
				bBreakY = true;
			}
		}
		
	}

	if (bMoveZ)
	{
		if (LocalDistanceZ > 0)
		{
			for (const FEngineInfo& Engine : UpEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceZ += Engine.Force;
			}

			float StoppingDistanceZ = (LocalVelocityZ * LocalVelocityZ) / (2.0f * TotalBreakingForceZ * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoZ: %.2f, unidades"), StoppingDistanceZ));

			if (StoppingDistanceZ > LocalDistanceZ)
			{
				bMoveZ = false;
				bBreakZ = true;
			}
		}
		else
		{
			for (const FEngineInfo& Engine : DownEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);

				TotalBreakingForceZ += Engine.Force;
			}

			float StoppingDistanceZ = (LocalVelocityZ * LocalVelocityZ) / (2.0f * TotalBreakingForceZ * DeltaTime);

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Distancia de frenadoZ: %.2f, unidades"), StoppingDistanceZ));

			if (abs(StoppingDistanceZ) > abs(LocalDistanceZ))
			{
				bMoveZ = false;
				bBreakZ = true;
			}
		}
		
	}

	if (bBreakX)
	{
		if (LocalVelocityX > 0)
		{
			for (const FEngineInfo& Engine : BackEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}
		}
		else if(LocalVelocityX < 0)
		{
			for (const FEngineInfo& Engine : FrontEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}
		}

	}

	if (bBreakY)
	{
		if (LocalVelocityY > 0)
		{
			for (const FEngineInfo& Engine : LeftEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}	
		}
		else if (LocalVelocityY < 0)
		{
			for (const FEngineInfo& Engine : RightEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}
		}
		
	}

	if (bBreakZ)
	{
		if (LocalVelocityZ > 0)
		{
			for (const FEngineInfo& Engine : DownEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}	
		}
		else if (LocalVelocityZ < 0)
		{
			for (const FEngineInfo& Engine : UpEngines)
			{
				ShipMesh->AddForceAtLocationLocal(Engine.Force * Engine.Direction * DeltaTime, Engine.Position);
			}
		}
		
	}

	/*FVector ClampedVelocity = FVector(
	FMath::Clamp(Velocity.X, -maxSpeed, maxSpeed),
	FMath::Clamp(Velocity.Y, -maxSpeed, maxSpeed),
	FMath::Clamp(Velocity.Z, -maxSpeed, maxSpeed)
	);

	ShipMesh->SetPhysicsLinearVelocity(ClampedVelocity);*/
	
}



// Called to bind functionality to input
void AShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

