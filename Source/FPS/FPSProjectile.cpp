// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BulletHoleMaterialFound(TEXT("Material'/Game/FPS_TR/Blueprints/Ammunition/BulletDecal/BulletHoleDecal.BulletHoleDecal'"));
	if (BulletHoleMaterialFound.Object) {
		BulletHoleMaterial = BulletHoleMaterialFound.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BloodSplatterMaterialFound(TEXT("Material'/Game/FPS_TR/Blueprints/Ammunition/BulletDecal/BloodSplatterDecal.BloodSplatterDecal'"));
	if (BloodSplatterMaterialFound.Object) {
		BloodSplatterMaterial = BloodSplatterMaterialFound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> HitMarkerSoundFound(TEXT("SoundWave'/Game/FPS_TR/Audio/BloodSplatterSound.BloodSplatterSound'"));
	if (HitMarkerSoundFound.Object) {
		HitMarkerSound = HitMarkerSoundFound.Object;
	}
}

void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ADecalActor* Decal = GetWorld()->SpawnActor<ADecalActor>(Hit.Location, FRotator());
	if (Decal) {
		ACharacter* CharacterCast = Cast<ACharacter>(Hit.Actor);
		if (CharacterCast == nullptr) {
			Decal->SetDecalMaterial(BulletHoleMaterial);
		}
		else {
			UGameplayStatics::PlaySoundAtLocation(this, HitMarkerSound, GetActorLocation());
			Decal->SetDecalMaterial(BloodSplatterMaterial);
		}
		Decal->SetLifeSpan(5.0f);
		Decal->GetDecal()->DecalSize = FVector(4.0f, 8.0f, 8.0f);
	}

	Destroy();
}