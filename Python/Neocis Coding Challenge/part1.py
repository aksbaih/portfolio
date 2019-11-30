'''
Run this script for the first part of the project. The user clicks down on the center
of a circle and drags its radius. The program draws that circle, highlights the points
on the grid that correspond to it, and draws two other circles showing the range from
the farthest to the nearest point of those to the center. It uses the class 'interface'
to show a GUI for the user.

Created by Akram Sbaih on Oct 26, 2019 for Neocis
'''
from interface import interface
import numpy as np 
from scipy.spatial import distance

# maximum distance from the circle for a point to be considered part of it
THRESHOLD = 10

# This function takes in an instance of 'interface' and a point representing the center of
# the circle the user dragged from, and a point on this circle 'end' of the drag. It draws
# that circle and highlights the points within THRESHOLD from the circumference of that circle.
def find_best_points(gui, center, end):
	radius = distance.euclidean(center, end)
	# the radius of the input circle must be bigger than the THRESHOLD to be considered
	if(radius < THRESHOLD): return
	gui.clean_screen()
	gui.draw_ring(center, int(radius), 'blue', width=2)
	# the distance from each point on the grid and the center of the input circle:
	distances = np.sqrt((gui.xs - center[0]) ** 2 + (gui.ys - center[1]) ** 2)
	# an array of True/False where True means that this point is within THRESHOLD of the circumference:
	toggles = np.abs(distances - radius) < THRESHOLD
	if(np.count_nonzero(toggles)): # check if at least one point is being highlighted
		gui.draw_ring(center, int(distances[toggles].min()), 'red', width=1) # inner limit
		gui.draw_ring(center, int(distances[toggles].max()), 'red', width=1) # outer limit
	gui.paint_balls(toggles) # this updates the color of the points to reflect their toggle state


# create an instance of the GUI and pass a function for 'drag' to specifiy that we want the 
# drag behavior not the 'generate' behavior
GUI = interface(drag=find_best_points)
