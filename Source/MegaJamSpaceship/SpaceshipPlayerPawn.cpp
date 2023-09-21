// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceshipPlayerPawn.h"
#include "SpaceshipPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"

// init various components in class
ASpaceshipPlayerPawn::ASpaceshipPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	BoostMultiplier = 3.f;
	SpringArmLength = 600.f;
	BoostZoomOut = 200.f;
	bFreeFly = false;
	bBoost = false;
	MaximumShipOffset = 50.f;
	
	SpringArm->TargetArmLength = SpringArmLength;
}

// Called when the game starts or when spawned
void ASpaceshipPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Actually going to keep this conversation with GitHub Copilot in here because it's interesting
	// q: what does this check do?
	// a: it checks if the HUDWidgetClass is valid, if it is, it will create the widget and add it to the viewport
	// q: what does valid mean in this context
	// a: it means that the HUDWidgetClass is not null
	// q: when is it null?
	// a: when the HUDWidgetClass is not set in the editor
	if (HUDWidgetClass)
	{
		// q: when would this be null?
		// a: when the HUDWidgetClass is not a valid class
		// but this is for the HUDWidget itself. We already checked if the HUDWidgetClass is valid
		// a: it's null when the HUDWidgetClass is not set in the editor
		// q: so why check again?
		// a: because the HUDWidgetClass could be set to a class that is not a valid class
		if (UUserWidget* HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass))
		{
			HUDWidget->AddToViewport();
		}
	}
}

void ASpaceshipPlayerPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FTransform RootTransform = Collision->GetComponentTransform();

	const FVector WorldSpaceVelocity = GetVelocity();
	const FVector LocalSpaceVelocity = RootTransform.InverseTransformVector(WorldSpaceVelocity);

	const float Sway = -LocalSpaceVelocity.GetSafeNormal().Y * DeltaTime * 100.f;
	
	const FVector CenterToShip = RootTransform.InverseTransformVector(Body->GetComponentLocation() - Collision->GetComponentLocation());
	const bool OnRight = CenterToShip.Y > 0;
	const bool MovingRight = LocalSpaceVelocity.Y > 0;
	
	const float ShipCenterDist = CenterToShip.Size();
	if (ShipCenterDist < MaximumShipOffset || (!OnRight && MovingRight) || (OnRight && !MovingRight))
	{
		// lesson learned the hard way: AddLocalOffset *already* adds the vector in the local space
		Body->AddLocalOffset(FVector(0, Sway, 0));
	}
}

// Called to bind functionality to input
void ASpaceshipPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// cast our inherited controller from Pawn to our custom SpaceshipPlayerController
	const ASpaceshipPlayerController* SpaceshipPlayerController = Cast<ASpaceshipPlayerController>(Controller);
	
	check(EnhancedInputComponent && SpaceshipPlayerController);

	// Triggered is for keys that can be held down
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerPawn::Move);
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->RotateAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerPawn::Rotate);
	// Started detects only on first press
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->FreeFlyAction, ETriggerEvent::Started, this, &ASpaceshipPlayerPawn::ToggleFreeFly);
	// Seems like you should pair Started/Completed together and you need separate actions to avoid conflicting behavior
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->StartBoostAction, ETriggerEvent::Started, this, &ASpaceshipPlayerPawn::Boost);
	EnhancedInputComponent->BindAction(SpaceshipPlayerController->StopBoostAction, ETriggerEvent::Completed, this, &ASpaceshipPlayerPawn::StopBoost);

	const ULocalPlayer* LocalPlayer = SpaceshipPlayerController->GetLocalPlayer();
	check(LocalPlayer);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(SpaceshipPlayerController->PawnMappingContext, 0);
}

void ASpaceshipPlayerPawn::Move(const FInputActionValue& ActionValue)
{
	// log current move scale
	const FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
	AddMovementInput(GetActorRotation().RotateVector(Input), MoveScale);
}

void ASpaceshipPlayerPawn::Rotate(const FInputActionValue& ActionValue)
{
	FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
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
		// question: see what removing this does, he mentioned something about the horizon changing
		// answer: Gimbal lock happens and the horizon he was referring to was in relation to the ship's alignment
		Input.Roll = 0;
		SetActorRotation(Input);	
	}
}

void ASpaceshipPlayerPawn::ToggleFreeFly()
{
	bFreeFly = !bFreeFly;
}

void ASpaceshipPlayerPawn::Boost()
{
	// start of boost, zoom out camera
	if (!bBoost)
	{
		bBoost = true;

		Movement->Acceleration *= BoostMultiplier;
		Movement->MaxSpeed *= BoostMultiplier;
		// MoveScale *= BoostMultiplier;
		SpringArm->TargetArmLength = SpringArmLength + BoostZoomOut;
	}
}

void ASpaceshipPlayerPawn::StopBoost()
{
	// end of boost, zoom in camera
	if (bBoost)
	{
		bBoost = false;

		Movement->Acceleration /= BoostMultiplier;
		Movement->MaxSpeed /= BoostMultiplier;
		// MoveScale /= BoostMultiplier;
		SpringArm->TargetArmLength = SpringArmLength;
	}
}
