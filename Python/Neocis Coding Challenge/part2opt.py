'''
This is an optimized approach to the problem described in part2.py. It uses 
scipy's minimization function to minimize a loss function with variables x, y, r:
the x and y coordinates of the center of the circle and its radius respectively. The
loss is the sum of the distance squared of each toggled point from the circumference
of the ring. A starting position is determined using the algorithm from part2.py.

Created by Akram Sbaih on Oct 26, 2019 for Neocis
'''
from interface import interface
import numpy as np 
from scipy.spatial import distance
import scipy.optimize as optimize

# This function is called when the user hits the 'generate/reset' button and 
# executes the optimized version of the algorithm. It draws two circles:
# a blue one for the result of the basic algorithm, and a red one for the optimized.
def find_best_circle(gui):
	if(np.count_nonzero(gui.toggles) < 2): # reset because there are no enough points
		gui.clean_screen()
		gui.paint_balls()
		gui.paint_generate_button()
		return
	# Implementation of the basic algorithm. Check part2.py for details.
	center = (int(gui.xs[gui.toggles].mean()), int(gui.ys[gui.toggles].mean()))
	mean_distance = np.sqrt((gui.xs[gui.toggles] - center[0]) ** 2 + (gui.ys[gui.toggles] - center[1]) ** 2).mean()
	gui.draw_ring(center, int(mean_distance), 'blue', width=2)
	# This loss function is the sum of the squared distances of toggled points from the 
	# circumference of the circle centered at params[0], params[1] and radius params[2]
	def loss(params):
		x, y, r = params
		distances = np.sqrt((gui.xs[gui.toggles] - x) ** 2 + (gui.ys[gui.toggles] - y) ** 2)
		distances = (distances - r) ** 2
		return distances.sum()
	# use the scipy.optimize utility to find the optimal x, y, r with least loss
	result = optimize.minimize(loss, [center[0], center[1], mean_distance])
	if result.success: # optimization succeeded; draw a red circle of the result
		gui.draw_ring((int(result.x[0]), int(result.x[1])), int(result.x[2]), 'red', width=2)
	else:
		print("Optimal solution not found")
	# toggle all points off
	gui.toggles ^= gui.toggles
# make an instance of a GUI and set its behavior to 'generate' mood
GUI = interface(generate= find_best_circle)