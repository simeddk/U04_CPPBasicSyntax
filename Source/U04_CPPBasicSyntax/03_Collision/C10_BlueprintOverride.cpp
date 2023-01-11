#include "C10_BlueprintOverride.h"
#include "Global.h"
#include "Chracters/CPlayer.h"

void AC10_BlueprintOverride::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AC10_BlueprintOverride::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AC10_BlueprintOverride::ActorEndOverlap);
}

void AC10_BlueprintOverride::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	SetBodyColor();
}

void AC10_BlueprintOverride::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ResetBodyColor();
}

void AC10_BlueprintOverride::ResetBodyColor_Implementation()
{
	ACPlayer* player = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player == nullptr) return;

	FLinearColor originColor = FLinearColor(0.450980f, 0.403922f, 0.360784f, 0.f);
	player->ChangeBodyColor(originColor, originColor);
}