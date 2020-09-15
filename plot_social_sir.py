import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

sir_data_df = pd.read_csv("sir_data.csv")

fig, ax = plt.subplots(figsize=(15,5))
ax.plot(sir_data_df["t"], sir_data_df["S"])
ax.plot(sir_data_df["t"], sir_data_df["I"])
ax.plot(sir_data_df["t"], sir_data_df["R"])
plt.legend()
plt.show()
