#pragma once

struct NoOpLogger {
    inline void operator()(int /*path*/, int /*step*/, double /*price*/) const {}
};
