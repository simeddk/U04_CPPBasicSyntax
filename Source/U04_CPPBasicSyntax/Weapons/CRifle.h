#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRifle.generated.h"

UCLASS()
class U04_CPPBASICSYNTAX_API ACRifle : public AActor
{
	GENERATED_BODY()
	
public:	
	ACRifle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
		FName HolsterSocket = "Holster_Rifle";

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
		FName HandSocket = "Hand_Rifle";

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USkeletalMeshComponent* Mesh;

	//생성자에서 스켈메쉬 컴포 생성 -> 에셋 세팅...
	//플레이어에게 스폰
	//소켓에 어타치

};
