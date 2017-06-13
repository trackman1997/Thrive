// Copyright (C) 2013-2017  Revolutionary Games
//! \file Common definitions and helpers for microbe related things

#pragma once

#include "CoreMinimal.h"

//! Compound type ids
//!
//! This is an enum to prevent implicit convertions from and to integers
//! These are dynamically assigned by the UCompoundRegistry object
UENUM()
enum class ECompoundID : int32{

    Invalid = -1
};


constexpr auto COMPOUND_CLOUD_SIMULATE_SIZE = 64;


