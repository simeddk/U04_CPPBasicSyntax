#include "CAnimInstance.h"
#include "Global.h"
#include "Chracters/IRifle.h"
#include "Weapons/CRifle.h"
#include "GameFramework/Character.h"

void UCAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter == nullptr) return;

	Speed = OwnerCharacter->GetVelocity().Size2D();
	Direction = CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetControlRotation());
	Pitch = OwnerCharacter->GetBaseAimRotation().Pitch;

	IIRifle* rifleInterface = Cast<IIRifle>(OwnerCharacter);
	if (!!rifleInterface)
	{
		bEquipped = rifleInterface->GetRifle()->IsEquipped();
		bAiming = rifleInterface->GetRifle()->IsAiming();
	}
}