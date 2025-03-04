// Fill out your copyright notice in the Description page of Project Settings.


#include "HISM_Cubes.h"

UHISM_Cubes::UHISM_Cubes() {

	PrimaryComponentTick.bCanEverTick = true;


}
void UHISM_Cubes::MoveCubes(float DeltaTime) {

	for (int32 Index = cubeStruct_Arr.Num() - 1; Index >= 0; --Index)
	{
		Fcube2& cubeStruct = cubeStruct_Arr[Index];
		if (cubeStruct.bMarkedForRemoval)
			continue;
		int32 InstanceIndex = cubeStruct.instance;
		FTransform TCur;
		GetInstanceTransform(InstanceIndex, TCur, true);

		FVector NewLocation = FMath::VInterpConstantTo(TCur.GetLocation(), Destination, DeltaTime, cubeStruct.speed);
		TCur.SetTranslation(NewLocation);
		UpdateInstanceTransform(InstanceIndex, TCur, true);

		if (TCur.GetLocation().Equals(Destination, 100.f)) {
			cubeStruct.bMarkedForRemoval = true;
			FTimerHandle TimerDestroy;
			GetWorld()->GetTimerManager().SetTimer(TimerDestroy, [this, &cubeStruct]() {
				DestroyCube(cubeStruct);
			}, 4.f, false);

			return;
		}


	}
}

void UHISM_Cubes::AddMultipleCubes(int32 GridX, int32 GridY, float SpaceBetween, FTransform OriginTransform, FVector Destination_L) {

	for (int32 x = 0; x < GridX; ++x) {
		for (int32 y = 0; y < GridY; ++y) {
			FVector LocalLocation(x * SpaceBetween, y * SpaceBetween, 0.f);
			FVector WorldLocation = OriginTransform.TransformPosition(LocalLocation);
			FTransform cubeTransform = OriginTransform; // Start with OriginTransform
			cubeTransform.SetTranslation(WorldLocation); // Set the new position
			AddCube(cubeTransform, Destination_L);
		}
	}
}

int32 UHISM_Cubes::AddCube(FTransform OriginTransform, FVector Destination_L) {

	int32 cubeInstance = -1;
	cubeInstance = AddInstanceWorldSpace(OriginTransform);
	Fcube2 cubeStruct;
	cubeStruct.instance = cubeInstance;
	cubeStruct.speed = FMath::FRandRange(25.f, 200.f);

	FGuid UniqueID = FGuid::NewGuid();
	cubeStruct.UniqueID = UniqueID;
	int32 ArrayIndex = cubeStruct_Arr.Add(cubeStruct);
	return cubeInstance;
}

void UHISM_Cubes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveCubes(DeltaTime);
	MarkRenderStateDirty();

}

void UHISM_Cubes::DestroyCube(UPARAM(ref) Fcube2& cubeStruct) {

	if (cubeStruct_Arr.Num() < 2) {
		cubeStruct_Arr.Empty();
		ClearInstances();
		return;
	}

	int32 LastcubeIndex = cubeStruct_Arr.Num() - 1;
	int32 LastInstanceIndex = GetInstanceCount() - 1;

	Fcube2* Lastcube = &cubeStruct_Arr[LastcubeIndex];
	Lastcube->instance = cubeStruct.instance;

	FTransform T;
	GetInstanceTransform(LastInstanceIndex, T, true);
	UpdateInstanceTransform(cubeStruct.instance, T, true);

	RemoveInstance(LastInstanceIndex);
	cubeStruct_Arr.RemoveSingleSwap(cubeStruct);

}