'''
This file includes utility functions that plot dataframes as described.
Akram Sbaih 08-08-2019 Stanford University
'''

import matplotlib.pyplot as plt 
import pandas as pd 

# plots the raw position readings from a csv file
def position_current_plot(filenames):
	for file in filenames:
		df = pd.read_csv(file)
		[plt.plot(-df['p1'], df[f"c{i+1}"], 'o', alpha=0.3, label=f"{file}.{i+1}") for i in range(4)]
	plt.legend()
	plt.show()
    
def position_only_plot(filenames):
	for file in filenames:
		df = pd.read_csv(file)
		plt.plot(df['p2'], -df['p1'], 'o', alpha=0.3, label=file)
	plt.legend()
	plt.show()

def dist_hist_plot(filenames, var, nbins=30):
	for file in filenames:
		df = pd.read_csv(file)
		plt.hist(df[var], nbins, alpha=0.1, color=filenames[file], label=f"{file}.{var}", density=True)
	plt.xlim(-100, 200)
	plt.ylim(0, 0.13)
	plt.legend()
	plt.show()
