#include "CPlayer.h"
#include "Global.h"
#include "CAnimInstance.h"
#include "Weapons/CRifle.h"
#include "Widgets/CUserWidget_Aim.h"
#include "Widgets/CUserWidget_AutoFire.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Blueprint/UserWidget.h"

ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	//Create Scene Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	//Set SkeletalMesh(Mannequin)
	ConstructorHelpers::FObjectFinder<USkeletalMesh> meshAsset(TEXT("SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (meshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(meshAsset.Object);

	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));

	ConstructorHelpers::FClassFinder<UCAnimInstance> animClass(TEXT("AnimBlueprint'/Game/Player/ABP_CPlayer.ABP_CPlayer_C'"));
	if (animClass.Succeeded())
		GetMesh()->SetAnimInstanceClass(animClass.Class);

	//Component Settings
	SpringArm->SetRelativeLocation(FVector(0, 0, 60));
	SpringArm->TargetArmLength = 200.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0, 60, 0);

	//Movement Settings
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
	//Get Aim Widget ClassRef
	ConstructorHelpers::FClassFinder<UCUserWidget_Aim> aimWidgetClass(TEXT("WidgetBlueprint'/Game/Widgets/WB_Aim.WB_Aim_C'"));
	if (aimWidgetClass.Succeeded())
		AimWidgetClass = aimWidgetClass.Class;

	//Get AutoFire Widget ClassRef
	ConstructorHelpers::FClassFinder<UCUserWidget_AutoFire> autoFireWidgetClass(TEXT("WidgetBlueprint'/Game/Widgets/WB_AutoFire.WB_AutoFire_C'"));
	if (autoFireWidgetClass.Succeeded())
		AutoFireWidgetClass = autoFireWidgetClass.Class;
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	UObject* obj;
	
	//Body Material Asset
	obj = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("MaterialInstanceConstant'/Game/Character/Materials/M_UE4Man_Body_Inst.M_UE4Man_Body_Inst'"));
	UMaterialInstanceConstant* bodyMaterialAsset = Cast<UMaterialInstanceConstant>(obj);
	if (bodyMaterialAsset == nullptr) return;

	//Logo Material Asset
	obj = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("MaterialInstanceConstant'/Game/Character/Materials/M_UE4Man_ChestLogo_Inst.M_UE4Man_ChestLogo_Inst'"));
	UMaterialInstanceConstant* logoMaterialAsset = Cast<UMaterialInstanceConstant>(obj);
	if (logoMaterialAsset == nullptr) return;

	//Create Dynamic Material
	BodyMaterialDynamic = UMaterialInstanceDynamic::Create(bodyMaterialAsset, nullptr);
	LogoMaterialDynamic = UMaterialInstanceDynamic::Create(logoMaterialAsset, nullptr);

	//Set Material to SkelMeshComp
	GetMesh()->SetMaterial(0, BodyMaterialDynamic);
	GetMesh()->SetMaterial(1, LogoMaterialDynamic);

	//Spawn Rifle
	Rifle = ACRifle::Spawn(GetWorld(), this);

	//Create Aim Widget
	AimWidget = CreateWidget<UCUserWidget_Aim, APlayerController>(GetController<APlayerController>(), AimWidgetClass);
	AimWidget->AddToViewport();
	AimWidget->SetVisibility(ESlateVisibility::Hidden);

	//Create AutoFire Widget
	AutoFireWidget = CreateWidget<UCUserWidget_AutoFire, APlayerController>(GetController<APlayerController>(), AutoFireWidgetClass);
	AutoFireWidget->AddToViewport();

	OnRifle();
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);

	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);

	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &ACPlayer::OnRun);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &ACPlayer::OffRun);

	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ACPlayer::OnInteract);
	
	PlayerInputComponent->BindAction("Rifle", EInputEvent::IE_Pressed, this, &ACPlayer::OnRifle);

	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ACPlayer::OnAim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ACPlayer::OffAim);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACPlayer::OnFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACPlayer::OffFire);

	PlayerInputComponent->BindAction("AutoFire", EInputEvent::IE_Pressed, this, &ACPlayer::OnAutoFire);
}

void ACPlayer::OnMoveForward(float InAxis)
{
	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotator).GetForwardVector().GetSafeNormal2D();

	AddMovementInput(direction, InAxis);
}

void ACPlayer::OnMoveRight(float InAxis)
{
	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotator).GetRightVector().GetSafeNormal2D();

	AddMovementInput(direction, InAxis);
}

void ACPlayer::OnHorizontalLook(float InAxis)
{
	AddControllerYawInput(InAxis);
}

void ACPlayer::OnVerticalLook(float InAxis)
{
	AddControllerPitchInput(InAxis);
}

void ACPlayer::OnRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ACPlayer::OffRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void ACPlayer::OnInteract()
{
	if (OnInteractEvent.IsBound())
		OnInteractEvent.Broadcast();
}

void ACPlayer::OnRifle()
{
	if (Rifle->IsEquipped())
	{
		OffAim();

		Rifle->Unequip();
		return;
	}

	Rifle->Equip();
}

void ACPlayer::OnAim()
{
	if (Rifle->IsEquipped() == false) return;
	if (Rifle->IsEquipping() == true) return;

	SpringArm->TargetArmLength = 100.f;
	SpringArm->SocketOffset = FVector(0, 30, 10);

	ZoomIn();

	Rifle->Begin_Aiming();

	AimWidget->SetVisibility(ESlateVisibility::Visible);
	
}

void ACPlayer::OffAim()
{
	if (Rifle->IsEquipped() == false) return;
	if (Rifle->IsEquipping() == true) return;

	SpringArm->TargetArmLength = 200.f;
	SpringArm->SocketOffset = FVector(0, 60, 0);

	ZoomOut();

	Rifle->End_Aiming();

	AimWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACPlayer::OnFire()
{
	Rifle->Begin_Fire();
}

void ACPlayer::OffFire()
{
	Rifle->End_Fire();
}

void ACPlayer::OnAutoFire()
{
	if (Rifle->IsFiring() == true) return;

	Rifle->ToggleAutoFire();

	Rifle->IsAutoFire() ? AutoFireWidget->OnAutoFire() : AutoFireWidget->OffAutoFire();
}


void ACPlayer::GetAimInfo(FVector& OutAimStart, FVector& OutAimEnd, FVector& OutAimDirection)
{
	OutAimDirection = Camera->GetForwardVector();

	FTransform transform = Camera->GetComponentToWorld();
	FVector cameraLocation = transform.GetLocation();
	OutAimStart = cameraLocation + OutAimDirection * 100.f;

	FVector recoilCone = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(OutAimDirection, 0.2f);
	OutAimEnd = cameraLocation + recoilCone * 3000;
}

void ACPlayer::OnTarget()
{
	AimWidget->OnTarget();
}

void ACPlayer::OffTarget()
{
	AimWidget->OffTarget();
}

void ACPlayer::ChangeBodyColor(FLinearColor InBodyColor, FLinearColor InLogoColor)
{
	BodyMaterialDynamic->SetVectorParameterValue("BodyColor", InBodyColor);
	LogoMaterialDynamic->SetVectorParameterValue("BodyColor", InLogoColor);
}
