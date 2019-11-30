'''
This file includes the implementation of the class 'interface.' It uses
pygame to view the GUI of the program and provides methods for input and output.

Created by Akram Sbaih on Oct 26, 2019 for Neocis
'''
import pygame
import numpy as np 

RANDOM_SCATTER = False 	# Enabling this option scatters the points instead of grid

GRID_SIZE = (20, 20)	# number of columns, number of rows
MARGIN = 20				# window margins and spacing between points
BUTTON_HEIGHT = 40		# for the 'Generate/Reset' button
RADIUS = 4				# for the points in the grid
NUM_BALLS = GRID_SIZE[0] * GRID_SIZE[1]
WINDOW_SIZE = ((GRID_SIZE[0]+1) * MARGIN, (GRID_SIZE[1]+2) * MARGIN + BUTTON_HEIGHT)

colors = {'black': (0, 0, 0), 'blue': (0, 0, 255), 'white': (255, 255, 255), 'red': (255, 0, 0), 'gray': (200, 200, 200)}
BALL_COLOR = colors['black']
TOGGLED_COLOR = colors['blue']
BG_COLOR = colors['white']
BUTTON_COLOR = colors['gray']

# An instance of 'interface' views the GUI of the program
class interface:
	# At initialization, this function paints all the objects on the window and
	# calls the event listeners. Only one of the two optional variables should be 
	# passed and it controls the behavior of the window. The behavior is undefined otherwise.
	# The values of these variables are functions that will be called in specific events.
	def __init__(self, generate=None, drag=None):
		# Initialize the screen
		pygame.init()
		pygame.font.init()
		# font is not specified so that the default system font is used
		self.font = pygame.font.SysFont('__', 30)
		self.screen = pygame.display.set_mode(WINDOW_SIZE)
		self.screen.fill(BG_COLOR)

		# Paint the initial grid
		# self.xs, self.ys, self.toggles: the x and y coordinates and the toggle state
		# of each point, respectively. The index goes to the right and down for each point.
		if RANDOM_SCATTER: 
			self.xs = np.random.randint(MARGIN, WINDOW_SIZE[0]-MARGIN, NUM_BALLS)
			self.ys = np.random.randint(MARGIN, MARGIN * GRID_SIZE[1], NUM_BALLS)
		else:
			self.xs = np.tile(MARGIN * np.arange(1, GRID_SIZE[0]+1), GRID_SIZE[1])
			self.ys = np.repeat(MARGIN * np.arange(1, GRID_SIZE[1]+1), GRID_SIZE[0])
		self.toggles= np.full(NUM_BALLS, False) # all points are untoggled initially
		self.paint_balls()

		# prepare for the event listeners and paint the 'generate' button if necessary 
		self.drag = drag
		self.generate = generate
		if(generate): self.paint_generate_button()

		# Start listening to events
		self.event_loop()

	# This function is called once and runs throughout the program listening for events
	# and taking suitable actions for them depending on what functions were passed to __init__
	def event_loop(self):
		done = False # turns to True to end the program
		while not done:
			for event in pygame.event.get():
				if event.type == pygame.QUIT: # close the program by breaking the while loop
					done = True
					break
				if event.type == pygame.MOUSEBUTTONDOWN:
					if self.drag: # this is the beginning of a drag; store the starting position
						self.drag_begin = event.pos
					elif self.generate: # this is a click to toggle a point or on the 'generate/reset'
						if self.generate_button.collidepoint(event.pos): # the 'generate/reset' is hit
							self.generate(self) # generate the best-fit circle
						else:
							self.check_toggle(event.pos) # it might be a point toggle; check which one if any
				elif event.type == pygame.MOUSEBUTTONUP and self.drag:
					self.drag(self, self.drag_begin, event.pos) # the end of drag; report starting and ending positions

	# This function views the 'generate/reset' button on the window
	def paint_generate_button(self):
		self.generate_button = pygame.Rect(MARGIN, (GRID_SIZE[1]+1) * MARGIN, (GRID_SIZE[0]-1) * MARGIN, BUTTON_HEIGHT)
		pygame.draw.rect(self.screen, BUTTON_COLOR, self.generate_button)
		self.screen.blit(self.font.render('Generate/Reset', False, colors['blue']), (WINDOW_SIZE[0]//3, WINDOW_SIZE[1]-MARGIN//2-BUTTON_HEIGHT))
		pygame.display.update()

	# This function is called when the user clicks down somewhere on the window in the 'generate' mode
	# It finds if this click is inside any of the points and toggles its state.
	def check_toggle(self, point):
		# I use the manhatten distance because it's less expensive and does the job wanted
		manhatten_distance = np.abs(self.xs - point[0]) + np.abs(self.ys - point[1])
		# XOR toggles the state of any point that the click is inside
		self.toggles ^= manhatten_distance < RADIUS
		# update the GUI to show the new state of the button. This overwrites the old graph
		self.paint_balls()

	# This function overpaints the points on the window with a provided toggle configuration
	# or with the one saved in the object otherwise (self.toggles)
	def paint_balls(self, toggles=[]):
		if len(toggles) == 0: toggles = self.toggles # the default state
		for i in range(NUM_BALLS):
			pygame.draw.circle(self.screen, TOGGLED_COLOR if toggles[i] else BALL_COLOR, (self.xs[i], self.ys[i]), RADIUS)
		pygame.display.update()

	# This fucntion wipse up everything from the screen for a fresh start
	def clean_screen(self):
		self.screen.fill(BG_COLOR)

	# This function draws an empty circle with provided properities. Usually used as the output
	# of the program. The param 'color' is a string from the 'colors' defined at the top of this document
	def draw_ring(self, center, radius, color, width=1):
		pygame.draw.circle(self.screen, colors[color], center, radius, width)
		pygame.display.update()

	def draw_ellipse(self, x, y, a, b, color='blue'):
		pygame.draw.ellipse(self.screen, colors[color], pygame.Rect(x-a, y-b, 2*a, 2*b), 1)
		pygame.display.update()

