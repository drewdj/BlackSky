// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ShipController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Ship.h"
#include "GameFramework/PawnMovementComponent.h"


AShipController::AShipController()
{
	bShowMouseCursor = true;

	PlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneComponent"));
	PlaneComponent->SetupAttachment(RootComponent);

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(TEXT("StaticMesh'/Game/Path/To/PlaneMesh.PlaneMesh'"));*/

	/*if (PlaneMeshAsset.Succeeded())
	{
		PlaneComponent->SetStaticMesh(PlaneMeshAsset.Object);
	}*/

	// Configura la posición y escala del plano
	FVector PlaneLocation(0.0f, 0.0f, ZValue);
	PlaneComponent->SetWorldLocation(PlaneLocation);
	PlaneComponent->SetWorldScale3D(FVector(1.0f)); // Ajusta la escala según sea necesario
}

void AShipController::OnSliderValueChanged(float Value)
{
	ZValue = Value;

	if(PlaneComponent != nullptr)
	{
		FVector PlaneLocation = PlaneComponent->GetComponentLocation();
		PlaneLocation.Z = ZValue;
		PlaneComponent->SetWorldLocation(PlaneLocation);
	}

}

void AShipController::BeginPlay()
{
	Super::BeginPlay();

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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShipController::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AShipController::StartMove);
		

		EnhancedInputComponent->BindAction(LookLockAction, ETriggerEvent::Started, this, &AShipController::LookLock);
		EnhancedInputComponent->BindAction(LookLockAction, ETriggerEvent::Completed, this, &AShipController::LookLock);
		
	}
}

void AShipController::Look(const FInputActionValue& Value)
{
	if (bLookLock)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		AddYawInput(LookAxisVector.X);
		AddPitchInput(LookAxisVector.Y);
	}
	
}

void AShipController::Move(const FInputActionValue& Value)
{


	// Obtén la ubicación del ratón en la pantalla
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	// Transforma la posición del ratón en un rayo en el mundo
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	// Define la ecuación del plano: ax + by + cz + d = 0
	// En este caso, considera un plano paralelo al eje XY y a una altura de z sobre el origen.
	
   // Ajusta este valor a tu necesidad
	float a = 0, b = 0, c = 1, d = -ZValue;

	// Encuentra la intersección del rayo con el plano
	// La ecuación de un rayo es P = P0 + tD, donde P0 es el punto inicial, D la dirección y t un escalar.
	// Sustituye esta ecuación en la del plano y resuelve para t:
	float t = (-a * WorldLocation.X - b * WorldLocation.Y - c * WorldLocation.Z - d) / (a * WorldDirection.X + b * WorldDirection.Y + c * WorldDirection.Z);
    
	// Calcula el punto de intersección
	FVector Intersection = WorldLocation + t * WorldDirection;

	FVector PawnLocation = GetPawn()->GetActorLocation();
	FVector Direction = Intersection - PawnLocation;
	Direction.Normalize();


	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Intersection.ToString());

	
	UStaticMeshComponent* ShipMeshComponent = Cast<AShip>(GetPawn())->GetShipMesh();

	float ForceMagnitude = 50000.0f; // Ajusta este valor según sea necesario
	FVector Force = Direction * ForceMagnitude;

	ShipMeshComponent->AddForce(Force);


}

void AShipController::LookLock(const FInputActionValue& Value)
{
	bLookLock = !bLookLock;
	bShowMouseCursor = !bShowMouseCursor;
	SetIgnoreLookInput(bShowMouseCursor);

	// Si se está activando la mirada bloqueada, cambia a GameOnly.
	if(bLookLock) {
		SetInputMode(FInputModeGameOnly());
	}
	// Si no, vuelve a GameAndUI.
	else {
		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeData);
	}
}

void AShipController::StartMove()
{

	// Obtén la ubicación del ratón en la pantalla
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	// Transforma la posición del ratón en un rayo en el mundo
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	// Define la ecuación del plano: ax + by + cz + d = 0
	// En este caso, considera un plano paralelo al eje XY y a una altura de z sobre el origen.
	
	// Ajusta este valor a tu necesidad
	float a = 0, b = 0, c = 1, d = -ZValue;

	// Encuentra la intersección del rayo con el plano
	// La ecuación de un rayo es P = P0 + tD, donde P0 es el punto inicial, D la dirección y t un escalar.
	// Sustituye esta ecuación en la del plano y resuelve para t:
	float t = (-a * WorldLocation.X - b * WorldLocation.Y - c * WorldLocation.Z - d) / (a * WorldDirection.X + b * WorldDirection.Y + c * WorldDirection.Z);
    
	// Calcula el punto de intersección
	FVector Intersection = WorldLocation + t * WorldDirection;

	FVector PawnLocation = GetPawn()->GetActorLocation();


	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Intersection.ToString());
	
	Cast<AShip>(GetPawn())->SetDestination(Intersection);
}







