import pandas as pd
import plotly.express as px

# Keep a small subset so plotting stays responsive.
def load_subset(csv_path: str, keep_paths: int = 200, step_stride: int = 1, chunksize: int = 200_000) -> pd.DataFrame:
    keep = set(range(keep_paths))
    parts = []
    for chunk in pd.read_csv(csv_path, chunksize=chunksize, usecols=["path", "step", "price"]):
        chunk = chunk[chunk["path"].isin(keep)]
        if step_stride > 1:
            chunk = chunk[chunk["step"] % step_stride == 0]
        if not chunk.empty:
            parts.append(chunk)
    return pd.concat(parts, ignore_index=True) if parts else pd.DataFrame(columns=["path", "step", "price"])


df = load_subset("output/per_path_price_output.csv", keep_paths=200, step_stride=1, chunksize=200_000)

fig = px.line(
    df,
    x="step",
    y="price",
    color="path",
    line_group="path",
    render_mode="webgl",
)

fig.update_traces(opacity=0.4)
fig.update_layout(showlegend=False, title="Monte Carlo Paths (subset)")
fig.show()
