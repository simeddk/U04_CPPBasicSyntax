#include "CAnimNotifyState_Unequip.h"
#include "Global.h"
#include "Chracters/IRifle.h"
#include "Weapons/CRifle.h"

FString UCAnimNotifyState_Unequip::GetNotifyName_Implementation() const
{
	return "Unequip";
}

void UCAnimNotifyState_Unequip::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (MeshComp == nullptr) return;

	IIRifle* rifleInterface =  Cast<IIRifle>(MeshComp->GetOwner());
	if (rifleInterface == nullptr) return;

	rifleInterface->GetRifle()->Begin_Unequip();
}

void UCAnimNotifyState_Unequip::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if (MeshComp == nullptr) return;

	IIRifle* rifleInterface = Cast<IIRifle>(MeshComp->GetOwner());
	if (rifleInterface == nullptr) return;

	rifleInterface->GetRifle()->End_Unequip();
}
