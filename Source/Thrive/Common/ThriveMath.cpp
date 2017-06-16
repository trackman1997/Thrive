// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveMath.h"

float UThriveMath::sigmoid(float x) {
	return 1 / (1 + FMath::Exp(-x));
}
