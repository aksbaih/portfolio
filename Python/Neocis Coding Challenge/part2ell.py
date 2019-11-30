'''
This is an extension to part2. It takes as input the toggle configuration
of points on the screen and produces the best-fit ellipse to match them.
Similar to the original part2, it uses interface to produce a GUI for the user,
and uses scipy's minimization utility to find the best fit ellipse.
More details about the math in the attched documentation.

Created by Akram Sbaih on Oct 26, 2019 for Neocis
'''
from interface import interface
import numpy as np 
from scipy.spatial import distance
import scipy.optimize as optimize
# this global variable is used when the optimization function fails to recover its latest point
lparams = []
# this is an arbitrary value that takes the place of zeros to make the math work; check docs
EPSILON_ZERO = 3
# This function is called when the user hits the 'generate/reset' button and 
# executes the ellipse finding algorithm.
def find_best_ellipse(gui):
	if(np.count_nonzero(gui.toggles) < 2): # reset because there are no enough points
		gui.clean_screen()
		gui.paint_balls()
		gui.paint_generate_button()
		return
	# Imolementation of the basic algorithm. Check part2.py for details.
	center = (int(gui.xs[gui.toggles].mean()), int(gui.ys[gui.toggles].mean()))
	mean_distance = np.sqrt((gui.xs[gui.toggles] - center[0]) ** 2 + (gui.ys[gui.toggles] - center[1]) ** 2).mean()
	# This loss function is the sum of the squared distances of toggled points from the 
	# ellipse of the provided x, y, a, b. It also updates the global variable 'lparams' to
	# these new params in case the optimization function fails so we have a last point.
	def loss(params):
		global lparams
		lparams = params
		x, y, a, b = params
		# center all the points around the center of the ellipse and make it the origin point
		xs, ys = gui.xs[gui.toggles] - x, gui.ys[gui.toggles] - y
		# replace zeros with EPSILON_ZERO to avoid division by zero
		xs = np.array([xs[i] if xs[i] else EPSILON_ZERO for i in range(len(xs))])
		ys = np.array([ys[i] if ys[i] else EPSILON_ZERO for i in range(len(ys))])
		# the following math is explained in the docs
		yps = xs / ys
		yps = (yps * b) ** 2 + a ** 2
		yps = np.sqrt((a * b) ** 2 / yps)
		yps = np.array([yps[i] if ys[i] > 0 else -yps[i] for i in range(len(yps))])
		xps = yps * xs / ys
		# the squared distance of each point from the nearest point on the ellipse
		distances_squared = (xs - xps) ** 2 + (ys - yps) ** 2
		# the loss is the sum of the squared distances
		return distances_squared.sum()
	# use the scipy.optimize utility to find the optimal x, y, a, b with least loss
	# uses the results found from the basic algorithm as a good starting point
	result = optimize.minimize(loss, [center[0], center[1], mean_distance, mean_distance])
	if result.success: # optimization succeeded; draw a red ellipse of the result
		gui.draw_ellipse(result.x[0], result.x[1], result.x[2], result.x[3], color='red')
	else:
		print("Optimal solution not found, will show the point where optimization stopped.")
		print(result.message)
		gui.draw_ellipse(lparams[0], lparams[1], lparams[2], lparams[3], color='red')
	# toggle all points off
	gui.toggles ^= gui.toggles
# make an instance of a GUI and set its behavior to 'generate' mood
GUI = interface(generate= find_best_ellipse)