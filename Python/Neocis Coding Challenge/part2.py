'''
Check part2opt.py for an optimized version of this program!
This script uses 'interface' to present the user with a GUI where they can toggle
the state of the point in the grid. When the user hits 'generate/reset,' it draws the
best fit circle that matches the toggled points, or resets the window by starting over.
This code uses an extremely simple algorithm where it finds the center of mass of the points
and makes it the center of the circle, then finds the mean distance of these points from the
center to make it the radius. Check part2opt.py for better results.

Created by Akram Sbaih on Oct 26, 2019 for Neocis
'''
from interface import interface
import numpy as np 
from scipy.spatial import distance

# This function is called when the user hits the 'generate/reset' button and
# it implements the algorithm to find the best fit circle for the toggled points
def find_best_circle(gui):
	if(np.count_nonzero(gui.toggles) < 2): # we need at least two toggled points to work or otherwise reset
		gui.clean_screen()
		gui.paint_balls()
		gui.paint_generate_button()
		return
	# find the center of mass, mean of xs and mean of ys
	center = (int(gui.xs[gui.toggles].mean()), int(gui.ys[gui.toggles].mean()))
	# find the average distance of the toggled points to the found center of mass: this is the radius
	mean_distance = np.sqrt((gui.xs[gui.toggles] - center[0]) ** 2 + (gui.ys[gui.toggles] - center[1]) ** 2).mean()
	gui.draw_ring(center, int(mean_distance), 'blue', width=2)
	gui.toggles ^= gui.toggles # toggle all the points OFF (if the function is called again, it works as reset)

# create an instance of the GUI in the 'generate' mode.
GUI = interface(generate=find_best_circle)