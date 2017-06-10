// Copyright (C) 2013-2017  Revolutionary Games
//! \file Common functions for test files

#pragma once

#include "AutomationTest.h"

constexpr auto THRIVE_UNITTEST = EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter;

// Helpful notes:
// AutomationTest.h has definitions of all these things. DateTimeTest.cpp is an example test
// in the engine or you can look at other tests in this folder.

// To add an error call AddError(FString::Printf(TEXT("%d"), 12)) or with line numbers
// AddError(TEXT("error"), TEXT(__FILE__), __LINE__)

// Also there are a bunch of predefined methods like TestEqual, TestFalse and TestNotNull

