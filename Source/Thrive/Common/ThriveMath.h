// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "ThriveMath.generated.h"


UCLASS()
class THRIVE_API UThriveMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Logistic function. The return value is in the range [0.0, 1.0]. It has an S shape */
	UFUNCTION(BlueprintPure, Category = "Math")
	static float sigmoid(float x);
};
