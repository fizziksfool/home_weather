import matplotlib.pyplot as plt
import pandas as pd


def append_csv(file_a: str, file_b: str) -> None:
    """
    Appends the contents of CSV file_b to CSV file_a using pandas.
    Assumes both files have identical headers and column counts.
    The function modifies file_a in place.
    """
    # Load both files
    df_a = pd.read_csv(file_a)
    df_b = pd.read_csv(file_b)

    # Check that headers match
    if not df_a.columns.equals(df_b.columns):
        raise ValueError("CSV headers do not match between file_a and file_b")

    # Concatenate and write back to file_a
    combined = pd.concat([df_a, df_b], ignore_index=True)
    combined.to_csv(file_a, index=False)


df = pd.read_csv(
    r"C:\Users\erbej\Desktop\Python Projects\home_weather\home_weather.csv"
)

df["Time"] = pd.to_datetime(df["Time"].astype(str))

df.plot(x="Time", y="Temperature")
df.plot(x="Time", y="Pressure")
df.plot(x="Time", y="Humidity")
df.plot(x="Time", y="Gas")
plt.show()
