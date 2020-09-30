import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from csv import reader

N = 0
with open("sir_data.csv", "r") as read_obj:
	csv_reader = reader(read_obj)
	header = next(csv_reader)
	first_line = next(csv_reader)
	first_line = [int(x) for x in first_line]
	#print(first_line)
	N = sum(first_line)

sir_data_df = pd.read_csv("sir_data.csv")

fig, ax = plt.subplots(figsize=(15,5))
ax.plot(sir_data_df["t"], sir_data_df["S"], label="S")
ax.plot(sir_data_df["t"], sir_data_df["I"], label="I")
ax.plot(sir_data_df["t"], sir_data_df["R"], label="R")
plt.legend()
plt.savefig("figs/plot_social_sir_{0}.pdf".format(N))
plt.show()
