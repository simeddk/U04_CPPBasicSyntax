#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDoor.generated.h"

UCLASS()
class U04_CPPBASICSYNTAX_API ACDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACDoor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


private:
	UFUNCTION()
		void ActorBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);
	
	UFUNCTION()
		void ActorEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
		void Interact();

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* Door;

	UPROPERTY(VisibleDefaultsOnly)
		class UBoxComponent* Box;

private:
	UPROPERTY(EditAnywhere)
		bool bHiddenCollision;

	UPROPERTY(EditAnywhere)
		float MaxDegree = 90.f;

	UPROPERTY(EditAnywhere)
		float Speed = 100.f;

private:
	class ACPlayer* Player;

	float Direction; //-1, +1
	float DirectionMaxDegree; //-90, +90
	float YawPerTick;
	
	bool bClosed = true; //Open or Close
	bool bOpening; //Tick
	bool bClosing; //Tick

};
