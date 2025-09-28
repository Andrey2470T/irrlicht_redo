#pragma once

#include "Types.h"
#include "Rect.h"

namespace utils
{
	v2f toV2f(const v2i &v)
	{
		return v2f(v.X, v.Y);
	}

	v2i toV2i(const v2f &v)
	{
		return v2i(v.X, v.Y);
	}

	v2u toV2u(const v2i &v)
    {
        return v2u(v.X, v.Y);
    }

    v2i toV2i(const v2u &v)
    {
        return v2i(v.X, v.Y);
    }

	v3f toV3f(const v3i &v)
	{
		return v3f(v.X, v.Y, v.Z);
	}

	v3i toV3i(const v3f &v)
	{
		return v3i(v.X, v.Y, v.Z);
	}

	v3u toV3u(const v3i &v)
    {
        return v3u(v.X, v.Y, v.Z);
    }

    v3i toV3i(const v3u &v)
    {
        return v3i(v.X, v.Y, v.Z);
    }

	rectf toRectf(const recti &r)
	{
		return rectf(r.ULC.X, r.ULC.Y, r.LRC.X, r.LRC.Y);
	}

	recti toRecti(const rectf &r)
	{
		return recti(r.ULC.X, r.ULC.Y, r.LRC.X, r.LRC.Y);
	}

	recti toRecti(const rectu &r)
    {
        return recti(r.ULC.X, r.ULC.Y, r.LRC.X, r.LRC.Y);
    }

    rectu toRectu(const recti &r)
    {
        return rectu(r.ULC.X, r.ULC.Y, r.LRC.X, r.LRC.Y);
    }
}
