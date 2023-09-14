// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

static void MapKey(UInputMappingContext* InputMappingContext, UInputAction* InputAction, FKey Key,
	bool bNegate = false, bool bSwizzle = false,
	EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ)
{
	FEnhancedActionKeyMapping& Mapping = InputMappingContext->MapKey(InputAction, Key);
	UObject* Outer = InputMappingContext->GetOuter();
	
	if (bNegate)
	{
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(Outer);
		Mapping.Modifiers.Add(Negate);
	}

	if (bSwizzle)
	{
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
		Swizzle->Order = SwizzleOrder;
		Mapping.Modifiers.Add(Swizzle);
	}
}

void ASpaceshipPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	PawnMappingContext = NewObject<UInputMappingContext>(this);

	// should I move this to blueprints?
	MoveAction = NewObject<UInputAction>(this);
	MoveAction->ValueType = EInputActionValueType::Axis3D;

	MapKey(PawnMappingContext, MoveAction, EKeys::W);
	MapKey(PawnMappingContext, MoveAction, EKeys::S, true);
	MapKey(PawnMappingContext, MoveAction, EKeys::D, false, true);
	MapKey(PawnMappingContext, MoveAction, EKeys::A, true, true);
	MapKey(PawnMappingContext, MoveAction, EKeys::SpaceBar, false, true, EInputAxisSwizzle::ZYX);
	MapKey(PawnMappingContext, MoveAction, EKeys::LeftControl, true, true, EInputAxisSwizzle::ZYX);

	RotateAction = NewObject<UInputAction>(this);
	RotateAction->ValueType = EInputActionValueType::Axis3D;
	MapKey(PawnMappingContext, RotateAction, EKeys::MouseY);
	MapKey(PawnMappingContext, RotateAction, EKeys::MouseX, false, true);
	MapKey(PawnMappingContext, RotateAction, EKeys::E, false, true, EInputAxisSwizzle::ZYX);
	MapKey(PawnMappingContext, RotateAction, EKeys::Q, true, true, EInputAxisSwizzle::ZYX);

	FreeFlyAction = NewObject<UInputAction>(this);
	MapKey(PawnMappingContext, FreeFlyAction, EKeys::F);
}
