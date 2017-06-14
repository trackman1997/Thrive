// Copyright (C) 2013-2017  Revolutionary Games
//! \file This file contains noise generation functions for procedural generation and other
//! things that need noise

#pragma once

#include <inttypes.h>
#include <vector>
#include <array>


class FPerlinNoise{
public:

    FPerlinNoise(uint32_t InWidth, uint32_t InHeight);

    //! for each point in the grid move the vector a little
    void Update();

    //! \returns Noise at point in range [-1, 1]
    float Perlin(uint32_t X, uint32_t Y) const;

    //! ditto
    float Perlin(float X, float Y) const;

protected:

    float DotGridGradient(int IX, int IY, float X, float Y) const;
    
private:

    std::vector<std::vector<std::array<float, 2>>> Grid;

    const uint32_t Width;
    const uint32_t Height;
};

