import matplotlib.pyplot as plt
import pandas as pd


def plot_csv_timeseries(file_path: str, gap_threshold="5m2s") -> None:
    # Load CSV
    df = pd.read_csv(file_path)

    # Try to parse Time as datetime
    try:
        df["Time"] = pd.to_datetime(df["Time"])
    except Exception:
        pass  # leave as-is if not datetime

    # Calculate time differences
    time_diffs = df["Time"].diff()
    gap_mask = time_diffs > pd.Timedelta(gap_threshold)

    # Convert °C to °F
    df["Temperature"] = round((df["Temperature"] * 9 / 5) + 32, 2)

    # Assign a "segment id" that increments every time there's a gap
    df["segment"] = gap_mask.cumsum()

    variables = ["Temperature", "Pressure", "Humidity", "Gas"]
    units = ["°F", "mbar", "%RH", "Ω"]

    # Create subplots (4 rows, 1 column)
    fig, axes = plt.subplots(len(variables), 1, figsize=(10, 8), sharex=True, dpi=120)

    for ax, col, unit in zip(axes, variables, units):
        for _, seg in df.groupby("segment"):
            ax.plot(seg["Time"], seg[col], label=col, color="tab:blue")
        ax.set_ylabel(f"{col} ({unit})")
        ax.grid(True)

    axes[-1].set_xlabel("Time")
    fig.suptitle("Environmental Data Over Time", fontsize=14)
    plt.tight_layout(rect=(0, 0, 1, 0.96))
    plt.show()


def append_csv(file_a: str, file_b: str | None = None) -> None:
    """
    Appends the contents of CSV file_b to CSV file_a using pandas.
    Assumes both files have identical headers and column counts.
    The function modifies file_a in place.
    """
    if not file_b:
        print("No data to append to home_weather.csv")
        return

    # Load both files
    df_a = pd.read_csv(file_a)
    df_b = pd.read_csv(file_b)

    # Check that headers match
    if not df_a.columns.equals(df_b.columns):
        raise ValueError("CSV headers do not match between file_a and file_b")

    # Concatenate and write back to file_a
    combined = pd.concat([df_a, df_b], ignore_index=True)
    combined.to_csv(file_a, index=False)


append_csv(
    ".\\data\\home_weather.csv",
    # ".\\data\\TEST.CSV",
)

plot_csv_timeseries(".\\data\\home_weather.csv")
