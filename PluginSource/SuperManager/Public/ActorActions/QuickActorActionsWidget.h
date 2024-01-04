// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

UENUM(BlueprintType)
enum class E_DuplicationAxis : uint8
{
	EDA_XAxis UMETA (DisplayName = "X Axis"),
	EDA_YAxis UMETA (DisplayName = "Y Axis"),
	EDA_ZAxis UMETA (DisplayName = "Z Axis"),
	EDA_MAX UMETA (DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation")
	bool bRandomizeRotYaw = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotYaw"))
	float RotYawMin = -45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotYaw"))
	float RotYawMax = 45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation")
	bool bRandomizeRotPitch = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotPitch"))
	float RotPitchMin = -45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotPitch"))
	float RotPitchMax = 45.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation")
	bool bRandomizeRotRoll = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotRoll"))
	float RotRollMin = -45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RandomActorRotation",meta = (EditCondition = "bRandomizeRotRoll"))
	float RotRollMax = 45.f;
};

UCLASS()
class SUPERMANAGER_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
#pragma region ActorBatchSelection
	UFUNCTION(BlueprintCallable, Category="ActorBatchSelection")
	void SelectAllActorsWithSimilarName();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

#pragma endregion 

#pragma region ActorBatchDuplication

	UFUNCTION(BlueprintCallable, Category="ActorBatchDuplication")
	void DuplicateActors();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="ActorBatchDuplication")
	E_DuplicationAxis AxisForDuplication = E_DuplicationAxis::EDA_XAxis;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="ActorBatchDuplication")
	int32 NumberOfDuplicates = 5;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="ActorBatchDuplication")
	float OffsetDist = 300.f;

#pragma endregion 

#pragma region RandomizeActorTransform

	UFUNCTION(BlueprintCallable, Category="RandomizeActorTransform")
	void RandomizeActorTransform();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform")
	FRandomActorRotation RandomActorRotation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform")
	bool bRandomizeScale = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform",meta = (EditCondition = "bRandomizeScale"))
	float ScaleMin = .5f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform",meta = (EditCondition = "bRandomizeScale"))
	float ScaleMax = 2.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform")
	bool bRandomizeOffset = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform",meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMin = -50.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="RandomizeActorTransform",meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMax = 50.f;

#pragma endregion 

#pragma region ResetRotation

	UFUNCTION(BlueprintCallable, Category="ResetRotation")
	void ResetRotation(); 
#pragma endregion 
	
private:
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActionSubsystem();
};
