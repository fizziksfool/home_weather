import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv(
    r"C:\Users\erbej\Desktop\Python Projects\home_weather\home_weather.csv"
)

df["Time"] = pd.to_datetime(df["Time"].astype(str))

df.plot(x="Time", y="Temperature")
df.plot(x="Time", y="Pressure")
df.plot(x="Time", y="Humidity")
df.plot(x="Time", y="Gas")
plt.show()
