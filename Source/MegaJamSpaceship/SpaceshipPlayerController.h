// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceshipPlayerController.generated.h"

/**
 *	Set up following this tutorial: https://www.youtube.com/watch?v=4wWIjlkSj2w
 */
UCLASS()
class MEGAJAMSPACESHIP_API ASpaceshipPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

	UPROPERTY()
	class UInputMappingContext* PawnMappingContext;

	UPROPERTY()
	class UInputAction* MoveAction;

	UPROPERTY()
	class UInputAction* RotateAction;

	UPROPERTY()
	class UInputAction* FreeFlyAction;

	UPROPERTY()
	class UInputAction* StartBoostAction;

	UPROPERTY()
	class UInputAction* StopBoostAction;
};
