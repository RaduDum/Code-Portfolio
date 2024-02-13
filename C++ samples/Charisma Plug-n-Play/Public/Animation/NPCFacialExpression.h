// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MorphTargetData.h"
#include "NPCFacialExpression.generated.h"

// Facial expression UE data asset
// Composed of several FMorphTargetData(Morth Target + Intensity)
// Will trigger when receiving a AssociatedCharismaEmotion from the Playthrough
UCLASS(BlueprintType)
class CHARISMAPLUGNPLAY_API UNPCFacialExpression : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AssociatedCharismaEmotion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMorphTargetData> MorphTargets;
};
