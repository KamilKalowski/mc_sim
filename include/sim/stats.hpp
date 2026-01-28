#pragma once

#include "include/sim/types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

inline SimulationResults calculate_statistics(const SimulationParams& params,
                                              const SimulationVectors& vecs) {
    const auto& prices       = vecs.terminal_prices;
    const auto& payoffs      = vecs.payoffs;
    const auto& disc_payoffs = vecs.disc_payoffs;
    const std::size_t N      = prices.size();

    SimulationResults results{};

    results.mean_price = std::accumulate(prices.begin(), prices.end(), 0.0) / N;

    double sum_sq = std::accumulate(prices.begin(), prices.end(), 0.0,
        [&](double acc, double p) {
            double d = p - results.mean_price;
            return acc + d * d;
        });
    results.std_dev = std::sqrt(sum_sq / (N - 1));

    auto [min_it, max_it] = std::minmax_element(prices.begin(), prices.end());
    results.min_price = *min_it;
    results.max_price = *max_it;

    std::vector<double> returns(N);
    std::transform(prices.begin(), prices.end(), returns.begin(),
        [initial = params.initial_price](double p) { return (p - initial) / initial; });
    std::nth_element(returns.begin(), returns.begin() + static_cast<std::size_t>(0.05 * N), returns.end());
    results.var_95 = -returns[static_cast<std::size_t>(0.05 * N)];

    double time_to_maturity = params.steps * params.delta_t;
    double discount         = std::exp(-params.risk_free_rate * time_to_maturity);
    double total_payoff     = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    results.option_price    = discount * (total_payoff / N);

    // Control variate with Y = S_T, E[Y] known analytically
    double mean_X = std::accumulate(disc_payoffs.begin(), disc_payoffs.end(), 0.0) / N;
    double mean_Y = results.mean_price;

    double cov_XY = 0.0;
    double var_Y  = 0.0;
    for (std::size_t i = 0; i < N; ++i) {
        double dy = prices[i] - mean_Y;
        cov_XY += (disc_payoffs[i] - mean_X) * dy;
        var_Y  += dy * dy;
    }
    cov_XY /= N;
    var_Y  /= N;

    double beta = (var_Y > 0.0) ? cov_XY / var_Y : 0.0;
    double expected_ST = params.initial_price * std::exp(params.risk_free_rate * time_to_maturity);
    results.option_price_cv = mean_X - beta * (mean_Y - expected_ST);

    return results;
}
