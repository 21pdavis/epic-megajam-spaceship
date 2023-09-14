// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceshipPlayerPawn.h"
#include "SpaceshipPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

// init various components in class
ASpaceshipPlayerPawn::ASpaceshipPlayerPawn()
{
	// init root sphere and set root
	Collision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision"));
	// like setting parent in Unity
	SetRootComponent(Collision);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Collision);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Collision);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

	MoveScale = 1.0f;
	RotateScale = 50.f;
	bFreeFly = false;
}

// Called when the game starts or when spawned
void ASpaceshipPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ASpaceshipPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	ASpaceshipPlayerController* SpaceshipPlayerController = Cast<ASpaceshipPlayerController>(Controller);
	
	check(EnhancedInputComponent && SpaceshipPlayerController);

	// Triggered is for keys that can be held down
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerPawn::Move);
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->RotateAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerPawn::Rotate);
	// Started detects only on first press
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->FreeFlyAction, ETriggerEvent::Started, this, &ASpaceshipPlayerPawn::ToggleFreeFly);

	ULocalPlayer* LocalPlayer = SpaceshipPlayerController->GetLocalPlayer();
	check(LocalPlayer);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(SpaceshipPlayerController->PawnMappingContext, 0);
}

void ASpaceshipPlayerPawn::Move(const FInputActionValue& ActionValue)
{
	FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
	AddMovementInput(GetActorRotation().RotateVector(Input), MoveScale);
}

void ASpaceshipPlayerPawn::Rotate(const FInputActionValue& ActionValue)
{
	FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
	// input * Time.deltaTime * RotateScale in Unity
	Input *= GetWorld()->GetDeltaSeconds() * RotateScale;

	if (bFreeFly)
	{
		// this function converts to quaternions to avoid Gimbal Lock
		AddActorLocalRotation(Input);
	}
	else
	{
		Input += GetActorRotation();
		Input.Pitch = FMath::ClampAngle(Input.Pitch, -89.9f, 89.9f);
		// TODO: see what removing this does, he mentioned something about the horizon changing
		// answer: Gimbal lock happens and the horizon he was referring to was in relation to the ship's alignment
		Input.Roll = 0;
		SetActorRotation(Input);	
	}
}

void ASpaceshipPlayerPawn::ToggleFreeFly()
{
	// TODO: make this recenter the camera to be above world view
	bFreeFly = !bFreeFly;
}

