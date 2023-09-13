// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceshipPlayerPawn.h"
#include "SpaceshipPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

// init various components in class
ASpaceshipPlayerPawn::ASpaceshipPlayerPawn()
{
	// init root sphere and set root
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	// like setting parent in Unity
	SetRootComponent(Sphere);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Sphere);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Sphere);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

	MoveScale = 1.0f;
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
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerPawn::Move);

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

