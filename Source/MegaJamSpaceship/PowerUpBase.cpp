// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpBase.h"

// Sets default values
APowerUpBase::APowerUpBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// creating object int the component hierarchy
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerUpMesh"));
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName); // Disable all previous collision profiles
	Mesh->SetCollisionProfileName(TEXT("OverlapAll")); // Set it to "OverlapAll"
	RootComponent = Mesh;

	// Add the OnPowerUpCollected function as a delegate to the OnComponentBeginOverlap event
	// Mesh->OnComponentBeginOverlap.AddDynamic(this, &APowerUpBase::OnPowerUpCollected);

	PowerUpDuration = 10.0f;
}

// Called when the game starts or when spawned
void APowerUpBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// rotate slowly
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += DeltaTime * 50.f;
	SetActorRotation(NewRotation);
}
