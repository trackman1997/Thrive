// Copyright (C) 2013-2017  Revolutionary Games
//! \file Common definitions and helpers for microbe related things

#pragma once

#include "CoreMinimal.h"

//! Compound type ids
//!
//! This is an enum to prevent implicit convertions from and to integers
//! These are dynamically assigned by the UCompoundRegistry object
UENUM(BlueprintType)
enum class ECompoundID : uint8{

    Invalid = 0
};

UENUM(BlueprintType)
enum class EBioProcessID : uint8 {

	Invalid = 0
};

constexpr auto COMPOUND_CLOUD_SIMULATE_SIZE = 64;
constexpr auto COMPOUND_CLOUD_NOISE_SCALE = 5;


