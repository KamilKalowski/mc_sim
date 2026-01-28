#pragma once

struct GBMProcess {
    double drift;
    double diffuse;

    inline double step(double price, double Z) const {
        return price * std::exp(drift + diffuse * Z);
    }
};

inline GBMProcess make_gbm(double r, double vol, double dt) {
    GBMProcess p;
    p.drift   = (r - 0.5 * vol * vol) * dt;
    p.diffuse = vol * std::sqrt(dt);
    return p;
}
