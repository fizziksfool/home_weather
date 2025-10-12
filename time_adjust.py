import pandas as pd


def shift_csv_dates(file_path: str, output_path: str, days: int = 25) -> None:
    """
    Reads a CSV file where the first column contains datetime strings,
    shifts all datetimes forward by a given number of days,
    and writes the result to a new CSV file.
    """
    # Read CSV, parse first column as datetime
    df = pd.read_csv(file_path, header=None)
    df[0] = pd.to_datetime(df[0])

    # Shift the datetime column by 'days'
    df[0] = df[0] + pd.Timedelta(days=days)  # type: ignore

    # Write adjusted CSV
    df.to_csv(output_path, index=False, header=False)


shift_csv_dates(
    r"C:\Users\erbej\Desktop\Python Projects\home_weather\data\needs_fixing.csv",
    r"C:\Users\erbej\Desktop\Python Projects\home_weather\data\fixed_dates.csv",
)
