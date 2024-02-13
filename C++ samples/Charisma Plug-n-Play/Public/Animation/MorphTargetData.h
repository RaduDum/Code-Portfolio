#pragma once

#include "CoreMinimal.h"
#include "MorphTargetData.generated.h"

// Simple struct for retaining MorphTarget data
USTRUCT(BlueprintType)
struct FMorphTargetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString MorphTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Intensity;

};
