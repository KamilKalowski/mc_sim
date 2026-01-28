#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <omp.h>

#include "include/payoffs/vanilla.hpp"
#include "include/processes/gbm.hpp"
#include "include/sim/logger.hpp"
#include "include/sim/path_simulator.hpp"
#include "include/sim/stats.hpp"
#include "include/sim/types.hpp"
#include "utils/xoshiro256ss.hpp"

constexpr int NUM_PATHS     = 1'000'000;
constexpr int STEPS_PER_DAY = 4;
constexpr int TRADING_DAYS  = 252;
constexpr int TOTAL_STEPS   = STEPS_PER_DAY * TRADING_DAYS;

constexpr double YEARS   = 1.0;
constexpr double DELTA_T = YEARS / static_cast<double>(TOTAL_STEPS);

void write_simulated_prices_to_csv(const std::vector<double>& simulated_prices,
                                   const std::vector<double>& payoffs) {
    std::ofstream out("./output/final_price_payoffs_output.csv");
    if (!out) {
        std::cerr << "Failed to open output.csv\n";
        return;
    }
    out << "path,terminal_price,payoff\n";
    for (std::size_t i = 0; i < simulated_prices.size(); ++i) {
        out << i << "," << simulated_prices[i] << "," << payoffs[i] << "\n";
    }
}

SimulationResults run_simulation(const SimulationParams& params) {
    using clock = std::chrono::high_resolution_clock;

    SimulationVectors vecs{
        std::vector<double>(NUM_PATHS),
        std::vector<double>(NUM_PATHS),
        std::vector<double>(NUM_PATHS)
    };

    const double time_to_maturity = params.steps * params.delta_t;
    const double discount         = std::exp(-params.risk_free_rate * time_to_maturity);

    GBMProcess process = make_gbm(params.risk_free_rate, params.volatility, params.delta_t);
    CallPayoff payoff{params.strike_price};
    NoOpLogger logger;

    auto t0 = clock::now();

    #pragma omp parallel
    {
        xoshiro256ss rng(std::random_device{}() + omp_get_thread_num());
        std::normal_distribution<double> normal_dist(0.0, 1.0);

        #pragma omp for
        for (int i = 0; i < NUM_PATHS; ++i) {
            auto path_result = simulate_path_antithetic(params, process, payoff, logger, rng, normal_dist);
            vecs.terminal_prices[i] = path_result.terminal_avg;
            vecs.payoffs[i]         = path_result.payoff_avg;
            vecs.disc_payoffs[i]    = discount * path_result.payoff_avg;
        }
    }

    auto t1 = clock::now();
    auto time_elapsed_sim = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "Time taken for simulation: " << time_elapsed_sim << " ms\n";

    SimulationResults results = calculate_statistics(params, vecs);
    write_simulated_prices_to_csv(vecs.terminal_prices, vecs.payoffs);
    return results;
}

int main() {
    SimulationParams params;
    params.initial_price  = 100.0;
    params.volatility     = 0.20;
    params.risk_free_rate = 0.05;
    params.steps          = TOTAL_STEPS;
    params.delta_t        = DELTA_T;
    params.strike_price   = 100.0;  // ATM

    SimulationResults results = run_simulation(params);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nMonte Carlo Simulation Results:\n";
    std::cout << "--------------------------------\n";
    std::cout << "Average Final Price: $" << results.mean_price << "\n";
    std::cout << "Standard Deviation: $" << results.std_dev << "\n";
    std::cout << "95% Value at Risk: " << results.var_95 * 100 << "%\n";
    std::cout << "Price Range: $" << results.min_price << " - $" << results.max_price << "\n";
    std::cout << "Call Option Price (plain MC): $" << results.option_price << "\n";
    std::cout << "Call Option Price (control variate): $" << results.option_price_cv << "\n";
    return 0;
}
