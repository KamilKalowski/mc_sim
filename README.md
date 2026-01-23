# Monte Carlo Option Simulator (GBM)

High-throughput C++ Monte Carlo for European options under GBM, using xoshiro256** RNG and OpenMP parallel paths, plus a lightweight Plotly viewer for price-path fans.

![Monte Carlo fan chart](docs/downsampled_fan_chart.png)

## What it does
- Parallelizes independent paths with **OpenMP** (per-thread RNG, per-path writes).
- Uses **xoshiro256** for fast, high-quality random draws.
- Precomputes GBM invariants (`drift`, `diffuse`) per run for cache-friendly stepping.
- Computes terminal stats (mean, std dev, VaR, discounted call price).
- Optionally streams downsampled per-step prices for visualization; Plotly renders an interactive fan chart.

## Quick start
```bash
make                   # build
make run               # run simulation (writes CSVs to output/)
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python viz/plot_paths.py  # opens the fan chart
```
> macOS + Apple Clang: install libomp (e.g., `brew install libomp`) and ensure `OMP_*` flags in the Makefile point to your Homebrew prefix.

## Performance notes
- Typical high-speed preset: 20k–50k paths, 2–4 steps/day (504–1008 steps/year), logging off.
- Logging throttled via `step % N == 0` and by path subset; disable for benchmarking.
- Scale paths linearly; Monte Carlo error shrinks ~1/√N.

## Visualization
- `viz/plot_paths.py` loads a subset in chunks (`load_subset`) to keep memory low; adjust `keep_paths` and `step_stride`.
- Export plots with the Plotly toolbar (camera icon) for docs/README images.
