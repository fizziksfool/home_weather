import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv(r"C:\Users\erbej\Documents\BME680 Temperature Logs\backyard data.csv")

df["Time"] = pd.to_datetime(df["Time"].astype(str))

df.plot(x="Time", y="Temperature")
df.plot(x="Time", y="Pressure")
df.plot(x="Time", y="Humidity")
df.plot(x="Time", y="Gas")
plt.show()


# import csv
# from datetime import datetime

# input_file = r"C:\Users\erbej\Documents\BME680 Temperature Logs\2025-08-28 TEST.TXT"
# output_file = (
#     r"C:\Users\erbej\Documents\BME680 Temperature Logs\2025-08-28 TEST (fixed).TXT"
# )

# with (
#     open(input_file, mode="r", newline="") as infile,
#     open(output_file, mode="w", newline="") as outfile,
# ):
#     reader = csv.DictReader(infile)
#     fieldnames = ["Time"] + [
#         fn for fn in reader.fieldnames if fn not in ("Date", "Time")
#     ]
#     writer = csv.DictWriter(outfile, fieldnames=fieldnames)

#     writer.writeheader()

#     for row in reader:
#         # Combine Date and Time into a single datetime string
#         dt = datetime.strptime(row["Date"] + " " + row["Time"], "%m/%d/%Y %H:%M:%S")
#         row_out = {"Time": dt.strftime("%Y-%m-%d %H:%M:%S")}

#         # Add the rest of the columns
#         for fn in fieldnames:
#             if fn != "Time":
#                 row_out[fn] = row[fn]

#         writer.writerow(row_out)
