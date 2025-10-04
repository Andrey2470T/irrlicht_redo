#pragma once

#include "Types.h"
#include "Rect.h"

namespace utils
{
    v2f toV2f(const v2i &v);

    v2i toV2i(const v2f &v);

    v2u toV2u(const v2i &v);

    v2i toV2i(const v2u &v);

    v3f toV3f(const v3i &v);

    v3i toV3i(const v3f &v);

    v3u toV3u(const v3i &v);

    v3i toV3i(const v3u &v);

    rectf toRectf(const recti &r);

    recti toRecti(const rectf &r);

    recti toRecti(const rectu &r);

    rectu toRectu(const recti &r);

}
