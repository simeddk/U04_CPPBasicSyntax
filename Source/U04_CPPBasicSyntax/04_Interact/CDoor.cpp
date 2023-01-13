#include "CDoor.h"
#include "Global.h"
#include "Chracters/CPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"

ACDoor::ACDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	//Create Components
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	DoorFrame->SetupAttachment(Scene);

	Door = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	Door->SetupAttachment(DoorFrame);

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(Scene);

	//Component Settings
	ConstructorHelpers::FObjectFinder<UStaticMesh> doorFrameAsset(TEXT("StaticMesh'/Game/DoorMesh/Props/SM_DoorFrame.SM_DoorFrame'"));
	if (doorFrameAsset.Succeeded())
		DoorFrame->SetStaticMesh(doorFrameAsset.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> doorAsset(TEXT("StaticMesh'/Game/DoorMesh/Props/SM_Door.SM_Door'"));
	if (doorAsset.Succeeded())
		Door->SetStaticMesh(doorAsset.Object);

	Door->SetRelativeLocation(FVector(0, 45, 0));

	Box->SetRelativeLocation(FVector(0, 0, 100));
	Box->SetBoxExtent(FVector(100, 50, 100));
	Box->SetCollisionProfileName("Trigger");
}

void ACDoor::BeginPlay()
{
	Super::BeginPlay();

	Box->SetHiddenInGame(bHiddenCollision);

	OnActorBeginOverlap.AddDynamic(this, &ACDoor::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACDoor::ActorEndOverlap);

	ACPlayer* player = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!!player)
		player->OnInteractEvent.AddDynamic(this, &ACDoor::Interact);

}

void ACDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOpening)
	{
		YawPerTick = Direction * Speed * DeltaTime;

		if (FMath::IsNearlyEqual(Door->GetRelativeRotation().Yaw, DirectionMaxDegree, 1.f))
		{
			bOpening = false;
			Door->SetRelativeRotation(FRotator(0, DirectionMaxDegree, 0));
		}
		
		Door->AddRelativeRotation(FRotator(0, YawPerTick, 0));
	}

	if (bClosing)
	{
		YawPerTick = -Direction * Speed * DeltaTime;

		if (FMath::IsNearlyZero(Door->GetRelativeRotation().Yaw, 1.f))
		{
			bClosing = false;
			Door->SetRelativeRotation(FRotator::ZeroRotator);
		}

		Door->AddRelativeRotation(FRotator(0, YawPerTick, 0));
	}
}

void ACDoor::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	Player = Cast<ACPlayer>(OtherActor);
}

void ACDoor::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	Player = nullptr;
}

void ACDoor::Interact()
{
	if (Player == nullptr) return;

	UCameraComponent* camera = Cast<UCameraComponent>(Player->GetComponentByClass(UCameraComponent::StaticClass()));
	if (camera == nullptr) return;

	float dot = GetActorForwardVector() | camera->GetForwardVector();
	Direction = FMath::Sign(dot);
	DirectionMaxDegree = Direction* MaxDegree;

	if (bClosed)
	{
		bClosed = false;
		bOpening = true;
		bClosing = false;
	}
	else
	{
		bClosed = true;
		bOpening = false;
		bClosing = true;
	}
}

