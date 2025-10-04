#include "ResizeFilters.h"

namespace img
{

f32 NearestFilter(f32 x)
{
    if (x >= -0.5f && x <= 0.5f)
		return 1.0f;

	return 0.0f;
}

f32 BilinearFilter(f32 x)
{
	if (x < 0.0f)
		x = -x;

	if (x < 1.0f)
		return 1.0f - x;

	return 0.0f;
}

f32 BicubicFilter(f32 x, f32 a)
{
	if (x < 0.0f)
		x = -x;

	if (x < 1.0f)
		return (((a + 2.0f) * x - (a + 3.0f)) * x * x + 1.0f);
	else if (x < 2.0f)
		return (((a * x - 5.0f * a) * x + 8.0f) * x - 4.0f * a);
	
	return 0.0f;
}

f32 getFilterRadius(RESAMPLE_FILTER type)
{
	f32 radius = 0.0f;
	
	switch (type) {
		case RF_NEAREST:
			radius = 0.5f;
			break;
		case RF_BILINEAR:
			radius = 1.0f;
			break;
		case RF_BICUBIC:
			radius = 2.0f;
			break;
	};
	
	return radius;
}

std::vector<f32> Kernel(f32 center, u32 border, f32 rk, RESAMPLE_FILTER type)
{
	f32 radius = getFilterRadius(type);

    u32 min = std::max<s32>(0, (s32)(std::floor(center - radius)));
    u32 max = std::min<s32>((s32)(std::ceil(center + radius)), border);
	
	std::vector<f32> weights(max - min);
	for (u32 x = min; x < max; x++) {
		switch (type) {
			case RF_NEAREST:
                weights[x-min] = NearestFilter((x - center + 0.5f) * rk) * rk;
				break;
			case RF_BILINEAR:
                weights[x-min] = BilinearFilter((x - center + 0.5f) * rk) * rk;
				break;
			case RF_BICUBIC:
                weights[x-min] = BicubicFilter((x - center + 0.5f) * rk) * rk;
				break;
			default:
                weights[x-min] = 0.0f;
				break;
		};
	}
	
	return weights;
}

}
