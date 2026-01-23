#include <stdio.h>
#include <math.h>
#include <random>
#include <vector>
#include <numeric>
#include <iostream>
#include <array>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <utils/xoshiro256ss.hpp>
#include <omp.h>

constexpr int NUM_PATHS = 1'000'000;
constexpr int STEPS_PER_DAY = 4;
constexpr int TRADING_DAYS = 252;
constexpr int TOTAL_STEPS = STEPS_PER_DAY * TRADING_DAYS;

constexpr double YEARS = 1.0;
constexpr double DELTA_T = YEARS / static_cast<double>(TOTAL_STEPS);

struct SimulationParams {
    double initial_price;
    double volatility;
    double risk_free_rate;
    int steps; // More steps -> better time resolution
    double delta_t; // Time per step in years
    double strike_price; // Strike price for option pricing
};

struct SimulationResults {
    double mean_price;
    double std_dev;
    double var_95; // 95% value at risk
    double max_price;
    double min_price;
    double option_price; // Expected value of call option
};

void write_simulated_prices_to_csv(const std::vector<double>& simulated_prices, const std::vector<double>& payoffs) {
    std::ofstream out("./output/final_price_payoffs_output.csv");
    if (!out) {
        std::cerr << "Failed to open output.csv\n";
    }
    out << "path,terminal_price,payoff\n";
    for (size_t i = 0; i < simulated_prices.size(); ++i) {
        out << i << "," << simulated_prices[i] << "," << payoffs[i] << "\n";
    }
}

SimulationResults calculate_statistics(const SimulationParams& params, std::vector<double>& simulated_prices, std::vector<double>& option_payoffs) {
    // Calculate stats
    SimulationResults results; 

    // Mean price
    results.mean_price = std::accumulate(simulated_prices.begin(), simulated_prices.end(), 0.0) / NUM_PATHS;

    // Standard deviation
    double sum_sq = std::accumulate(simulated_prices.begin(), simulated_prices.end(),
        0.0, [&](double acc, double price) {
            double diff = price - results.mean_price;
            return acc + diff * diff;
        });
    results.std_dev = std::sqrt(sum_sq / (NUM_PATHS - 1));

    // Min and max
    auto[min_it, max_it] = std::minmax_element(simulated_prices.begin(), simulated_prices.end());
    results.min_price = *min_it;
    results.max_price = *max_it;

    // 95% VaR (Value at Risk)
    std::vector<double> returns(NUM_PATHS);
    std::transform(simulated_prices.begin(), simulated_prices.end(), returns.begin(),
        [initial = params.initial_price](double price) {
            return (price - initial) / initial;
        });
    std::sort(returns.begin(), returns.end());
    results.var_95 = -returns[static_cast<size_t>(NUM_PATHS * 0.05)];

    // Option price (discounted expected payoff)
    double total_payoff = std::accumulate(option_payoffs.begin(), option_payoffs.end(), 0.0);
    double time_to_maturity = params.steps * params.delta_t;
    results.option_price = std::exp(-params.risk_free_rate * time_to_maturity) * 
                          (total_payoff / NUM_PATHS);

    return results;
}

double simulate_path(const SimulationParams& params, xoshiro256ss& rng, std::normal_distribution<>& normal_dist, std::ofstream* per_step_stream, int path) {
    double price = params.initial_price;

    // Pre-computing GBM invariants
    double drift = (params.risk_free_rate - .5 * params.volatility * params.volatility) * params.delta_t;
    double diffuse = params.volatility * std::sqrt(params.delta_t);

    for(int step = 0; step < params.steps; ++step) {
        double Z = normal_dist(rng);
        price *= std::exp(drift + diffuse * Z);
        if (per_step_stream && step % 50 == 0) {
            *per_step_stream << path << "," << step << "," << price << "\n";
        }
    }
    return price;
}

SimulationResults run_simulation(const SimulationParams& params) {
    using clock = std::chrono::high_resolution_clock;

    std::vector<double> simulated_prices(NUM_PATHS);
    std::vector<double> option_payoffs(NUM_PATHS);

    std::ofstream per_step_stream("output/per_path_price_output.csv");
    per_step_stream << "path,step,price\n";
    
    auto t0 = clock::now();

    #pragma omp parallel
    {
        xoshiro256ss rng(std::random_device{}());
        std::normal_distribution<double> normal_dist(0.0, 1.0);

        auto t0 = clock::now();
        // Run simulations
        #pragma omp for
        for(int i = 0; i < NUM_PATHS; ++i) {
            double final_price = simulate_path(params, rng, normal_dist, nullptr, i);
            simulated_prices[i] = final_price;
            option_payoffs[i] = std::max(final_price - params.strike_price, 0.0);
        }
    }

    auto t1 = clock::now();
    auto time_elapsed_sim = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    std::cout << "Time taken for a simulation: " << time_elapsed_sim << " ms\n"; 

    SimulationResults results = calculate_statistics(params, simulated_prices, option_payoffs);
    write_simulated_prices_to_csv(simulated_prices, option_payoffs);

    return results;
}

int main() {
    SimulationParams params;
    params.initial_price = 100.0;
    params.volatility = 0.20;
    params.risk_free_rate = 0.05;
    params.steps = TRADING_DAYS * STEPS_PER_DAY;
    params.delta_t = DELTA_T;
    params.strike_price = 100.0;  // At-the-money option

    SimulationResults results = run_simulation(params);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nMonte Carlo Simulation Results:\n";
    std::cout << "--------------------------------\n";
    std::cout << "Average Final Price: $" << results.mean_price << "\n";
    std::cout << "Standard Deviation: $" << results.std_dev << "\n";
    std::cout << "95% Value at Risk: " << results.var_95 * 100 << "%\n";
    std::cout << "Price Range: $" << results.min_price << " - $" << results.max_price << "\n";
    std::cout << "Call Option Price: $" << results.option_price << "\n";

    return 0;
}
