'''
This script uses the other classes to perform all kinds of tests and visualizations as 
coded for.
Akram Sbaih 08-06-2019 Stanford University
'''
# from scanner_chaning_speed import scan 
from plotter import position_current_plot, dist_hist_plot
# from coord import *
# from arm import arm

'''
for speed in [8]:
	for iteration in range(0, 15):
		scan(joint=1, direction='r', n_steps=800//speed, filename=f"scans/auto/rough_0.5f/{speed}.{iteration}.csv", topo=False, reposition=True, speed=speed, iterations=1)
		print(f"done: {speed} . {iteration}")
'''

# dist_hist_plot({f"scans/auto/rough_0.5f/{2 ** s}.{i}.csv":['blue', 'red', 'green'][s-1] for i in range(1,15) for s in [1]}, var='c1', nbins=50)
[position_current_plot([f"scans/auto/rough_0.5f/2.{i}.csv"]) for i in range(15)]
