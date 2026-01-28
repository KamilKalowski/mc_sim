#pragma once

#include <vector>

struct SimulationParams {
    double initial_price;
    double volatility;
    double risk_free_rate;
    int    steps;       // time steps per path
    double delta_t;     // time per step in years
    double strike_price;
};

struct SimulationResults {
    double mean_price;
    double std_dev;
    double var_95;
    double max_price;
    double min_price;
    double option_price;     // plain MC
    double option_price_cv;  // control variate
};

struct PathResults {
    double payoff_avg;   // averaged payoff across antithetic branches
    double terminal_avg; // averaged terminal price across branches
};

struct SimulationVectors {
    std::vector<double> terminal_prices;
    std::vector<double> payoffs;
    std::vector<double> disc_payoffs;
};
