// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Engine.h"


// Log gategories
// Log colours are defined by the severities which are: Log,Warning,Error,Fatal

//General Log
DECLARE_LOG_CATEGORY_EXTERN(ThriveLog, Log, All);


//DECLARE_LOG_CATEGORY_EXTERN(CellLog, Log, All);
 
//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(ThriveCriticalErrors, Log, All);

// Quick access to printing text with no extra formatting
#define LOG_LOG(x) UE_LOG(ThriveLog, Log, TEXT(x));
#define LOG_WARNING(x) UE_LOG(ThriveLog, Warning, TEXT(x));
#define LOG_ERROR(x) UE_LOG(ThriveLog, Error, TEXT(x));
#define LOG_FATAL(x) UE_LOG(ThriveCriticalErrors, Fatal, TEXT(x));

// Shows up on screen
#define DISPLAY_MESSAGE(text) if (GEngine) \
 GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue, text);

