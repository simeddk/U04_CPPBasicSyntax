#include "C04_TwoLights.h"
#include "Components/PointLightComponent.h"

AC04_TwoLights::AC04_TwoLights()
{
	PointLight1 = CreateDefaultSubobject<UPointLightComponent>("PointLight1");
	PointLight1->SetupAttachment(Scene);

	PointLight2 = CreateDefaultSubobject<UPointLightComponent>("PointLight2");
	PointLight2->SetupAttachment(Scene);

	PointLight1->Intensity = 1e+4f;
	PointLight1->AttenuationRadius = 200.f;
	PointLight1->LightColor = FColor(255, 128, 50);

	PointLight2->Intensity = 1e+4f;
	PointLight2->AttenuationRadius = 200.f;
	PointLight2->LightColor = FColor(255, 128, 50);
	PointLight2->SetRelativeLocation(FVector(0, 200, 0));
}

