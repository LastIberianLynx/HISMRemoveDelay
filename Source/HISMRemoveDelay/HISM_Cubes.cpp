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
			FGuid ID = cubeStruct.UniqueID;
			GetWorld()->GetTimerManager().SetTimer(TimerDestroy, [this, ID]() {

				DestroyCubeByID(ID);

				//DestroyCube(cubeStruct);
				}, 4.f, false);
			cubeStruct_Arr[Index].speed = 9999;
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
	OriginTransform.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
	cubeInstance = AddInstanceWorldSpace(OriginTransform);
	Fcube2 cubeStruct;
	cubeStruct.instance = cubeInstance;
	//cubeStruct.speed = FMath::Max(cubeStruct_Arr.Num()+1, 50);
	cubeStruct.speed = FMath::FRandRange(25.f, 100.f);

	FGuid UniqueID = FGuid::NewGuid();
	cubeStruct.UniqueID = UniqueID;
	cubeStruct.ArrayIndex = cubeStruct_Arr.Num();
	ID_Index_Map.Add(UniqueID, cubeStruct.ArrayIndex);
	//Index_ID_Map.Add(cubeStruct.ArrayIndex, UniqueID);
	IDs_Created.Add(UniqueID);
	int32 ArrayIndex = cubeStruct_Arr.Add(cubeStruct);
	return cubeInstance;
}

void UHISM_Cubes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveCubes(DeltaTime);
	MarkRenderStateDirty();

}

void UHISM_Cubes::DestroyCubeByID(FGuid ID) {

	if (cubeStruct_Arr.Num() < 2) {
		cubeStruct_Arr.Empty();
		ID_Index_Map.Empty();
		ClearInstances();
		return;
	}

	if (IDs_Destroyed.Contains(ID)) {
		int32 error = 3;
		return;
	}

	if (!IDs_Created.Contains(ID)) {
		int32 error = 3;
		return;
	}

	IDs_Destroyed.Add(ID);

	int32 RemoveIndex = *ID_Index_Map.Find(ID);
	Fcube2& cubeStruct = cubeStruct_Arr[RemoveIndex];

	int32 LastcubeIndex = cubeStruct_Arr.Num() - 1;
	int32 LastInstanceIndex = GetInstanceCount() - 1;

	if (RemoveIndex == LastcubeIndex) {

		RemoveInstance(LastInstanceIndex);
		cubeStruct_Arr.RemoveAt(cubeStruct.instance);
		ID_Index_Map.Remove(cubeStruct.UniqueID);
		return;
	}

	Fcube2* Lastcube = &cubeStruct_Arr[LastcubeIndex];
	int32 CubeInstanceToRemove = cubeStruct.instance;

	// Instead of reassigning the reference, update the array directly
	ID_Index_Map.Remove(cubeStruct.UniqueID);
	ID_Index_Map.Add(Lastcube->UniqueID, cubeStruct.ArrayIndex);

	cubeStruct_Arr[LastcubeIndex].ArrayIndex = cubeStruct.ArrayIndex;
	cubeStruct_Arr[LastcubeIndex].instance = cubeStruct.instance;
	cubeStruct_Arr[cubeStruct.ArrayIndex] = cubeStruct_Arr[LastcubeIndex];


	FTransform T;
	GetInstanceTransform(LastInstanceIndex, T, true);
	UpdateInstanceTransform(CubeInstanceToRemove, T, true);

	RemoveInstance(LastInstanceIndex);
	cubeStruct_Arr.RemoveAt(LastcubeIndex);

}


void UHISM_Cubes::DestroyCube( Fcube2& cubeStruct) {

	if (cubeStruct_Arr.Num() < 2) {
		cubeStruct_Arr.Empty();
		ClearInstances();
		return;
	}

	int32 LastcubeIndex = cubeStruct_Arr.Num() - 1;
	int32 LastInstanceIndex = GetInstanceCount() - 1;

	Fcube2* Lastcube = &cubeStruct_Arr[LastcubeIndex];
	int32 CubeInstanceToRemove = cubeStruct.instance;

	// Instead of reassigning the reference, update the array directly
	cubeStruct_Arr[LastcubeIndex].ArrayIndex = cubeStruct.ArrayIndex;
	cubeStruct_Arr[LastcubeIndex].instance = cubeStruct.instance;
	cubeStruct_Arr[cubeStruct.ArrayIndex] = cubeStruct_Arr[LastcubeIndex];


	FTransform T;
	GetInstanceTransform(LastInstanceIndex, T, true);
	UpdateInstanceTransform(CubeInstanceToRemove, T, true);

	RemoveInstance(LastInstanceIndex);
	cubeStruct_Arr.RemoveAt(LastcubeIndex);

}


void UHISM_Cubes::DestroyCubeSimple(UPARAM(ref) Fcube2& cubeStruct) {

	//this one screwed

	//if (cubeStruct_Arr.Num() < 2) {
	//	cubeStruct_Arr.Empty();
	//	ClearInstances();
	//	return;
	//}



	//int32 LastcubeIndex = cubeStruct_Arr.Num() - 1;
	//int32 LastInstanceIndex = GetInstanceCount() - 1;

	//Fcube2* Lastcube = &cubeStruct_Arr[LastcubeIndex];
	//Lastcube->instance = cubeStruct.instance;

	////FTransform T;
	////GetInstanceTransform(LastInstanceIndex, T, true);
	////UpdateInstanceTransform(cubeStruct.instance, T, true);

	//RemoveInstance(cubeStruct.instance);
	//cubeStruct_Arr.RemoveSingleSwap(cubeStruct);

}


void UHISM_Cubes::DestroyCubeDelayed( Fcube2& cubeStruct) {
		FTimerHandle TimerDestroy;
	GetWorld()->GetTimerManager().SetTimer(TimerDestroy, [this, &cubeStruct]() {
		if (!cubeStruct.UniqueID.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("FGuid is all zero!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FGuid is valid."));
		}

		DestroyCubeByID(cubeStruct.UniqueID);

		//DestroyCube(cubeStruct);
	}, 4.f, false);
	
}