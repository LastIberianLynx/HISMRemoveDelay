// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HISM_Cubes.generated.h"


USTRUCT(BlueprintType)
struct Fcube2 {
	GENERATED_BODY()
	public:
	UPROPERTY()
	int32 instance;
	float speed;
	UPROPERTY()
	bool bMarkedForRemoval = false;
	UPROPERTY()
	FGuid UniqueID;
	UPROPERTY()
		int32 ArrayIndex;

	bool operator==(const Fcube2& Other) const
	{
		return UniqueID == Other.UniqueID;
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class HISMREMOVEDELAY_API UHISM_Cubes : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()
		UHISM_Cubes();



public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int32 AddCube(FTransform OriginTransform, FVector Destination_L);

	UFUNCTION(BlueprintCallable)
	void MoveCubes(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void AddMultipleCubes(int32 GridX, int32 GridY, float SpaceBetween, FTransform OriginTransform, FVector Destination_L);

	UFUNCTION(BlueprintCallable)
	void DestroyCube(Fcube2& cubeStruct);

	UFUNCTION(BlueprintCallable)
		void DestroyCubeDelayed(Fcube2& cubeStruct);	
	
	UFUNCTION(BlueprintCallable)
		void DestroyCubeSimple(UPARAM(ref)Fcube2& cubeStruct);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<Fcube2> cubeStruct_Arr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Destination = FVector(1000.f, 0.f, 0.f);
	UPROPERTY()
	TMap<FGuid, int32> ID_Index_Map;

	UPROPERTY()
	TMap<int32, FGuid> Index_ID_Map;

	UFUNCTION(BlueprintCallable)
	void DestroyCubeByID(FGuid ID);

	TArray<FGuid> IDs_Destroyed;
	TArray<FGuid> IDs_Created;


};
