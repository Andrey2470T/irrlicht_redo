#pragma once

#include "BasicIncludes.h"

namespace img
{

enum RESAMPLE_FILTER
{
	RF_NEAREST,
	RF_BILINEAR,
	RF_BICUBIC
};

f32 NearestFilter(f32 x);
f32 BilinearFilter(f32 x);
f32 BicubicFilter(f32 x, f32 a=0.0f);
f32 getFilterRadius(RESAMPLE_FILTER type);
std::vector<f32> Kernel(f32 center, u32 border, f32 rk, RESAMPLE_FILTER type);

}
