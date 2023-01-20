#include "CRifle.h"
#include "Global.h"
#include "Chracters/IRifle.h"
#include "Chracters/CPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMeshActor.h"

ACRifle::ACRifle()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> meshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/AR4/SK_AR4.SK_AR4'"));
	Mesh->SetSkeletalMesh(meshAsset.Object);

	ConstructorHelpers::FObjectFinder<UAnimMontage> grabMontageAsset(TEXT("AnimMontage'/Game/Character/Montages/Rifle_Grab_Montage.Rifle_Grab_Montage'"));
	if (grabMontageAsset.Succeeded())
		GrabMontage = grabMontageAsset.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> ungrabMontageAsset(TEXT("AnimMontage'/Game/Character/Montages/Rifle_Ungrab_Montage.Rifle_Ungrab_Montage'"));
	if (ungrabMontageAsset.Succeeded())
		UngrabMontage = ungrabMontageAsset.Object;

	ConstructorHelpers::FClassFinder<UCameraShake> cameraShakeAsset(TEXT("Blueprint'/Game/Weapons/BP_CameraShake.BP_CameraShake_C'"));
	if (cameraShakeAsset.Succeeded())
		CameraShakeClass = cameraShakeAsset.Class;
}

ACRifle* ACRifle::Spawn(UWorld* InWorld, ACharacter* InOwnerCharacter)
{
	FActorSpawnParameters spawnParam;
	spawnParam.Owner = InOwnerCharacter;
	return InWorld->SpawnActor<ACRifle>(spawnParam);
}

void ACRifle::Equip()
{
	if (bEquipped == true) return;
	if (bEquipping == true) return;

	bEquipping = true;

	OwnerCharacter->PlayAnimMontage(GrabMontage, 1.75f);
}

void ACRifle::Begin_Equip()
{
	bEquipped = true;
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HandSocket);
}

void ACRifle::End_Equip()
{
	bEquipping = false;
}

void ACRifle::Unequip()
{
	if (bEquipped == false) return;
	if (bEquipping == true) return;

	bEquipping = true;

	OwnerCharacter->PlayAnimMontage(UngrabMontage, 1.75f);
}

void ACRifle::Begin_Unequip()
{
	bEquipped = false;
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);
}

void ACRifle::End_Unequip()
{
	bEquipping = false;
}

void ACRifle::Begin_Aiming()
{
	bAiming = true;
}

void ACRifle::End_Aiming()
{
	bAiming = false;
}

void ACRifle::Begin_Fire()
{
	if (bEquipped == false) return;
	if (bEquipping == true) return;
	if (bAiming == false) return;
	if (bFiring == true) return;

	bFiring = true;

	Firing();
}

void ACRifle::End_Fire()
{
	bFiring = false;
}

void ACRifle::Firing()
{
	//Todo
	//- > Bullet Class : StaticMeshComp, ProjectileMovementComp
	// -> Effect(Sound, Particle, Decal)

	//Camera Shake
	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	if (!!player)
	{
		APlayerController* controller = player->GetController<APlayerController>();

		if (!!CameraShakeClass)
			controller->PlayerCameraManager->PlayCameraShake(CameraShakeClass);
	}


	IIRifle* rifleInterface = Cast<IIRifle>(OwnerCharacter);
	if (rifleInterface == nullptr) return;

	FVector start, end, direction;
	rifleInterface->GetAimInfo(start, end, direction);

	FHitResult hitResult;
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this);
	collisionQueryParams.AddIgnoredActor(OwnerCharacter);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, collisionQueryParams))
	{
		AStaticMeshActor* staticMeshActor = Cast<AStaticMeshActor>(hitResult.GetActor());
		if (!!staticMeshActor)
		{
			UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(staticMeshActor->GetRootComponent());
			if (!!staticMeshComp)
			{
				//Add Impulse - Target
				if (staticMeshComp->BodyInstance.bSimulatePhysics)
				{
					direction = staticMeshActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
					direction.Normalize();

					staticMeshComp->AddImpulseAtLocation(direction * 3000.f, OwnerCharacter->GetActorLocation());
					return;

				}//if (bSimulatePhysics)
			}// if (!!staticMeshComp)
		}// if (!!staticMeshActor)
	}//if (LineTrace)
}

void ACRifle::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true) , HolsterSocket);
}

void ACRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//조준 중일때 -> 표적판 마우스를 대면 -> 위젯.Dot 빨갛게...
	if (bAiming == false) return;

	IIRifle* rifleInterface = Cast<IIRifle>(OwnerCharacter);
	if (rifleInterface == nullptr) return;

	FVector start, end, direction;
	rifleInterface->GetAimInfo(start, end, direction);
	
	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1.f, 0, 3.f);

	FHitResult hitResult;
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this);
	collisionQueryParams.AddIgnoredActor(OwnerCharacter);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_PhysicsBody, collisionQueryParams))
	{
		AStaticMeshActor* staticMeshActor = Cast<AStaticMeshActor>(hitResult.GetActor());
		if (!!staticMeshActor)
		{
			UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(staticMeshActor->GetRootComponent());
			if (!!staticMeshComp)
			{
				if (staticMeshComp->BodyInstance.bSimulatePhysics)
				{
					rifleInterface->OnTarget();
					return;

				}//if (bSimulatePhysics)
			}// if (!!staticMeshComp)
		}// if (!!staticMeshActor)
	}//if (LineTrace)

	rifleInterface->OffTarget();

}

