#include "BlendModes.h"

namespace img
{

color8 doBlend(const color8 &src, const color8 &dst, BlendMode mode)
{
    CHECK_SAME_FORMAT(src, dst);
    //InfoStream << "doBlend() mode: " << (u8)mode << "\n";

	switch (mode) {
		case BM_NORMAL:
			return NormalBlend(src, dst);
		case BM_ALPHA:
			return AlphaBlend(src, dst);
		case BM_ADD:
			return AddBlend(src, dst);
		case BM_SUBTRACTION:
			return SubtractBlend(src, dst);
		case BM_MULTIPLY:
			return MultiplyBlend(src, dst);
		case BM_DIVISION:
			return DivisionBlend(src, dst);
		case BM_SCREEN:
			return ScreenBlend(src, dst);
		case BM_OVERLAY:
			return OverlayBlend(src, dst);
		case BM_HARD_LIGHT:
			return HardLightBlend(src, dst);
		case BM_SOFT_LIGHT:
			return SoftLightBlend(src, dst);
		case BM_GRAIN_EXTRACT:
			return GrainExtractBlend(src, dst);
		case BM_GRAIN_MERGE:
			return GrainMergeBlend(src, dst);
		case BM_DARKEN_ONLY:
			return DarkenOnlyBlend(src, dst);
		case BM_LIGHTEN_ONLY:
			return LightenOnlyBlend(src, dst);
		default:
            return color8(src.getFormat());
	}
}

}
