#pragma once

#include <algorithm>

struct CallPayoff {
    double strike;
    inline double operator()(double ST) const {
        return std::max(ST - strike, 0.0);
    }
};

struct PutPayoff {
    double strike;
    inline double operator()(double ST) const {
        return std::max(strike - ST, 0.0);
    }
};
