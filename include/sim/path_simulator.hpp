#pragma once

#include "include/sim/types.hpp"

template <class Process, class Payoff, class Logger, class RNG, class NormalDist>
inline PathResults simulate_path_antithetic(const SimulationParams& params,
                                            const Process& process,
                                            const Payoff& payoff,
                                            const Logger& logger,
                                            RNG& rng,
                                            NormalDist& normal_dist) {
    double price_pos = params.initial_price;
    double price_neg = params.initial_price;

    for (int step = 0; step < params.steps; ++step) {
        double Z = normal_dist(rng);
        price_pos = process.step(price_pos, Z);
        price_neg = process.step(price_neg, -Z);
        logger(0, step, price_pos); // path id not tracked here; logger can ignore
    }

    double payoff_pos = payoff(price_pos);
    double payoff_neg = payoff(price_neg);

    return {0.5 * (payoff_pos + payoff_neg), 0.5 * (price_pos + price_neg)};
}
