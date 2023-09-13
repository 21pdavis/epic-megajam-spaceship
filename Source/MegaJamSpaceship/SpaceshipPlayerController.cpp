// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

void ASpaceshipPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	PawnMappingContext = NewObject<UInputMappingContext>(this);

	// should I move this to blueprints?
	MoveAction = NewObject<UInputAction>(this);
	MoveAction->ValueType = EInputActionValueType::Axis3D;
	PawnMappingContext->MapKey(MoveAction, EKeys::W);

	FEnhancedActionKeyMapping& BackwardsMovementMapping = PawnMappingContext->MapKey(MoveAction, EKeys::S);
	UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(this);
	BackwardsMovementMapping.Modifiers.Add(Negate);
}
