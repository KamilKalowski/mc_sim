# Monte Carlo Option Simulator (GBM)

High-throughput C++ Monte Carlo engine for European options under GBM, using **xoshiro256** RNG, **OpenMP** parallel paths, antithetic variates, and a simple control variate; + a lightweight Plotly viewer for price-path fans.

![Monte Carlo fan chart](docs/downsampled_fan_chart.png)

## What it does
- Parallelizes paths with **OpenMP** (thread-local xoshiro256 RNGs).
- Variance reduction: **antithetic variates**; **control variate** with terminal price `S_T` (known `E[S_T]=S0·exp(rT)`).
- Precomputes GBM invariants (`drift`, `diffuse`) per run for cache-friendly stepping.
- Computes terminal stats (mean, std dev, VaR) and call price (plain MC + control variate).
- Per-step streaming is off by default for speed; optional CSV downsample for Plotly fan chart.

## Quick start
```bash
make                   # build
make run               # run simulation (writes outputs/ stats; streaming off)
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python viz/plot_paths.py  # opens the fan chart
```
> macOS + Apple Clang: install libomp (e.g., `brew install libomp`) and ensure `OMP_*` flags in the Makefile point to your Homebrew prefix.

## Performance notes
- Typical high-speed preset: 20k–50k paths, 2–4 steps/day (504–1008 steps/year), streaming off.
- Antithetic + control variate significantly reduce variance before increasing paths.
- Scale paths linearly; Monte Carlo error shrinks ~1/√N.

## Visualization
- `viz/plot_paths.py` loads a subset in chunks (`load_subset`) to keep memory low; adjust `keep_paths` and `step_stride`.
- Export plots with the Plotly toolbar (camera icon) for docs/README images.
