// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpBase.generated.h"

UCLASS()
class MEGAJAMSPACESHIP_API APowerUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUpBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	// UFUNCTION(BlueprintCallable)
	// virtual void OnPowerUpCollected(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category="Powerup")
	float PowerUpDuration;

	// Timer handle to manage power up duration
	FTimerHandle PowerUpTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
