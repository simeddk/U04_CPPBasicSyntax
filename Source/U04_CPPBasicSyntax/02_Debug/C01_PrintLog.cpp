#include "C01_PrintLog.h"
#include "Global.h"

AC01_PrintLog::AC01_PrintLog()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC01_PrintLog::BeginPlay()
{
	Super::BeginPlay();

	if (bVisible == false) return;

	CLog::Print(1);
	CLog::Print(2, 1);
	CLog::Print(3, 2, 20.f, FColor::Green);
	CLog::Print(4, 2, 20.f, FColor::Green);
	CLog::Print("Hell World");
	CLog::Print(GetActorLocation());
	CLog::Print(GetActorRotation());

	CLog::Log(10);
	CLog::Log(PI);
	CLog::Log("Hell World");
	CLog::Log(GetActorLocation());
	CLog::Log(GetActorRotation());
	CLog::Log(__FILE__);
	PrintLine();
}

void AC01_PrintLog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
	CLog::Print(RunningTime, 100);
}

