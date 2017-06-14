// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "Noise.h"

#include <iostream>
#include <math.h>
#include <random>

// Code from the perlin noise prototype. This should probably be cleaned up at some point
// https://github.com/Revolutionary-Games/thrive-prototypes/blob/master/noise/cppnoise.cpp

//for normalising vectors in 2d
float norm(float a, float b){
    if (a != 0 && b!= 0){
        return pow(pow(a,2) + pow(b,2),0.5);
    }
    else{
        return 1;
    }
}

//interpolate
float lerp(float a, float b, float c){
	return (1 - c)*a + c*b;
}
// ------------------------------------ //
FPerlinNoise::FPerlinNoise(uint32_t InWidth, uint32_t InHeight) :
    Grid(InWidth, std::vector<std::array<float, 2>>(InHeight, {{0, 0}})),
    Width(InWidth), Height(InHeight)
{
    //for each point in a 100x100 Grid make a 2d normalised vector
    //pointing in a random direction
    for (int i = 0; i < Width; i++){
        for (int j = 0; j < Height; j++){
            Grid[i][j][0]  = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            Grid[i][j][1]  = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float normalise = norm(Grid[i][j][0], Grid[i][j][1]);
            Grid[i][j][0] = Grid[i][j][0] / normalise;
            Grid[i][j][1] = Grid[i][j][1] / normalise;
        }
    }

    Update();
}
// ------------------------------------ //
void FPerlinNoise::Update(){

    for (int i = 0; i < Width; i++){
        for (int j = 0; j < Height; j++){
            Grid[i][j][0]  += 0.1*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            Grid[i][j][1]  += 0.1*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float normalise = norm(Grid[i][j][0], Grid[i][j][1]);
            Grid[i][j][0] = Grid[i][j][0] / normalise;
            Grid[i][j][1] = Grid[i][j][1] / normalise;

        }
    }
}
// ------------------------------------ //
//Computes the dot product of the distance and gradient vectors.
float FPerlinNoise::DotGridGradient(int ix, int iy, float x, float y) const{
    //Precomputed (or otherwise) gradient vectors at each Grid node
	float grad_x = Grid[std::min<size_t>(Width - 1,iy)][std::min<size_t>(Width - 1, ix)][0];
	float grad_y = Grid[std::min<size_t>(Width - 1,iy)][std::min<size_t>(Width - 1, ix)][1];
	//Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;
	//Compute the dot-product
	return (dx*grad_x + dy*grad_y);
}

float FPerlinNoise::Perlin(float X, float Y) const{
	//Determine Grid cell coordinates
	int x0 = (int)floor(X);
	int x1 = (int)x0 + 1;
	int y0 = (int)floor(Y);
	int y1 = (int)y0 + 1;
	//Determine interpolation weights
	//Could also use higher order polynomial/s-curve here
	float sx = X - (float)x0;
	float sy = Y - (float)y0;
	//Interpolate between Grid point gradients
	float n0 = DotGridGradient(x0, y0, X, Y);
	float n1 = DotGridGradient(x1, y0, X, Y);
	float ix0 = lerp(n0, n1, sx);
	n0 = DotGridGradient(x0, y1, X, Y);
	n1 = DotGridGradient(x1, y1, X, Y);
	float ix1 = lerp(n0, n1, sx);
	float value = lerp(ix0, ix1, sy);
	return value;
}

float FPerlinNoise::Perlin(uint32_t X, uint32_t Y) const{
	//Determine Grid cell coordinates
	int x1 = (int)X + 1;
	int y1 = (int)Y + 1;
	//Determine interpolation weights
	//Could also use higher order polynomial/s-curve here

    // This would always be zero anyways
	// float sx = X - (float)x0;
	// float sy = Y - (float)y0;
    float sx = 0;
    float sy = 0;
    
	//Interpolate between Grid point gradients
	float n0 = DotGridGradient(X, Y, X, Y);
	float n1 = DotGridGradient(x1, Y, X, Y);
	float ix0 = lerp(n0, n1, sx);
	n0 = DotGridGradient(X, y1, X, Y);
	n1 = DotGridGradient(x1, y1, X, Y);
	float ix1 = lerp(n0, n1, sx);
	float value = lerp(ix0, ix1, sy);
	return value;
}


