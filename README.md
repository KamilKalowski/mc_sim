# Monte Carlo Option Simulator (GBM)

A fast C++ Monte Carlo engine for European options (GBM) plus a lightweight Python Plotly visualizer for the simulated price paths.

![Monte Carlo fan chart](docs/downsampled_fan_chart.png)

## What it does
- Simulates **10k** GBM price paths at fine time resolution (configurable).
- Computes terminal stats (mean, std dev, VaR, call price).
- Streams downsampled per-step prices for visualization.
- Plots interactive “fan chart” of paths using Plotly.

## Quick start
```bash
make          # build
make run      # run simulation (writes CSVs to output/)
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt  # or pip install pandas plotly
python viz/plot_paths.py         # opens interactive fan chart
