#include "C02_StaticMesh.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

AC02_StaticMesh::AC02_StaticMesh()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Meshes/SM_Cube"));

	if (meshAsset.Succeeded())
		Mesh->SetStaticMesh(meshAsset.Object);
}

void AC02_StaticMesh::BeginPlay()
{
	Super::BeginPlay();

	UObject* obj = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("MaterialInstanceConstant'/Game/Materials/MAT_StaticMesh.MAT_StaticMesh'"));
	
	UMaterialInstanceConstant* material;
	material = Cast<UMaterialInstanceConstant>(obj);
	if (material == nullptr) return;
	
	DynamicMaterial = UMaterialInstanceDynamic::Create(material, nullptr);

	Mesh->SetMaterial(0, DynamicMaterial);

	UKismetSystemLibrary::K2_SetTimer(this, "ChangeParameter", 1.f, true);

}

void AC02_StaticMesh::ChangeParameter()
{
	FLinearColor color;
	color.R =  UKismetMathLibrary::RandomFloatInRange(0, 1);
	color.G =  UKismetMathLibrary::RandomFloatInRange(0, 1);
	color.B =  UKismetMathLibrary::RandomFloatInRange(0, 1);
	color.A = 1.f;

	DynamicMaterial->SetVectorParameterValue("BaseColor", color);
	DynamicMaterial->SetScalarParameterValue("Metallic", color.R);
	DynamicMaterial->SetScalarParameterValue("Roughness", color.G);
}


