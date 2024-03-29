'''
Akram Sbaih, 08-06-2019, Stanford University
This file contains the class template for a robotic arm object,
use it to control and read from the arm as described below
'''
from CurrentReader import *
import os, ctypes, struct, datetime, sys


class arm:
	def __init__(self):
		self.start()
		# self.enable_free_motion()

	'''
	arm has the following attributes: dynamixel, reader
	'''
	def import_sdk(self, dynamixel_sdk_path):
		os.sys.path.append(dynamixel_sdk_path)  # append sdk to PATH
		os.sys.path.append('.')			 	# append '.' to PATH
		current_working_directory = os.getcwd()
		os.chdir(dynamixel_sdk_path) 			# change working dir
		import dynamixel_functions as dynamixel # imports Dynamixel SDK library
		self.dynamixel = dynamixel 				# saves the sdk into the object
		os.chdir(current_working_directory)		# return the working directory back here

	def configure_reader(self, config):
		self.reader = DynamixelReader(device_name=config['name'], baud_rate=config['baud'], m1id=config['ids'][0], m2id=config['ids'][1], m3id=config['ids'][2], m4id=config['ids'][3], proto_ver=config['protocol'], read_addr=config['reader_addr'], read_len=config['reader_len'])

	def set_current(self, motor, value):
		self.reader.Set_Value(motor, self.goal['add_cur'], self.goal['len_cur'], value)

	def set_pos(self, motor, value):
		self.reader.Set_Value(motor, self.goal['add_pos'], self.goal['len_pos'], value)

	def set_pos_slow(self, motor, origin, target):
		for tick in range(int(origin), int(target), 1 if target > origin else -1):
			self.reader.Set_Value(motor, self.goal['add_pos'], self.goal['len_pos'], tick)
		self.reader.Set_Value(motor, self.goal['add_pos'], self.goal['len_pos'], int(target))

	def set_all_pos_slow(self, m1, m2, m4):
		original_state = self.read_state()
		nsteps = 2
		m1s = (m1 - original_state['p1']) / nsteps
		m2s = (m2 - original_state['p2']) / nsteps
		m4s = (m4 - original_state['p4']) / nsteps		
		for step in range(int(nsteps + 1)):
			self.reader.Set_Value(self.reader.m1id, self.goal['add_pos'], self.goal['len_pos'], int(original_state['p1'] + step * m1s))
			self.reader.Set_Value(self.reader.m2id, self.goal['add_pos'], self.goal['len_pos'], int(original_state['p2'] + step * m2s))
			self.reader.Set_Value(self.reader.m4id, self.goal['add_pos'], self.goal['len_pos'], int(original_state['p4'] + step * m4s))


	# returns a dictionary representing the state of all the currents and positions and time of the arm
	def read_state(self):
		[timestamp, dxl1_current, dxl2_current, dxl3_current, dxl4_current] = self.reader.Read_Sync_Once()
		position1 = self.reader.Read_Value(self.reader.m1id, self.present['add_pos'], self.present['len_pos'])
		position2 = self.reader.Read_Value(self.reader.m2id, self.present['add_pos'], self.present['len_pos'])
		position4 = self.reader.Read_Value(self.reader.m4id, self.present['add_pos'], self.present['len_pos'])
		state = {'time': timestamp, 'c1': dxl1_current, 'c2': dxl2_current, 'c3': dxl3_current, 'c4': dxl4_current, 'p1': position1, 'p2': position2, 'p4': position4}
		return state

	# returns the id of the motor responsible for the joint and direction. Kills if it's inappropriate
	def what_motor(self, joint, direction):
		if joint == 1:
			if direction == 'l' or direction == 'r':
				return self.reader.m1id
			elif direction == 'u' or direction == 'd':
				return self.reader.m2id
		elif joint == 2:
			if direction == 'l' or direction == 'r':
				return self.reader.m3id
			elif direction == 'u' or direction == 'd':
				return self.reader.m4id
		print("Inappropriate motion direction. EXIT")
		sys.exit(0)

	def set_pro_params(self, present={'add_pos': 132, 'len_pos': 4, 'add_cur': 126, 'len_cur': 4}, goal={'add_pos': 116, 'len_pos': 4, 'add_cur': 102, 'len_cur': 4}):
		self.present = present
		self.goal = goal

	# moves the specified motor in the specified direction by the dpecified distance
	def move_motor(self, motor, direction, distance):
		# print(f"{motor} => {distance}, {direction}")
		initial_pos = self.init_state[{self.reader.m1id: 'p1',  self.reader.m2id: 'p2'}[motor]]
		if (direction == 'l' or direction == 'u'):
			self.set_pos(motor, initial_pos - distance)
		else:
			self.set_pos(motor, initial_pos + distance)

	# initializer for the arm addresses, default to the arm used in the lab
	def start(self, dynamixel_sdk_path= '../DynamixelSDK-master/python/dynamixel_functions_py', config= {'name': "/dev/tty.usbserial-FT2GXCCT".encode('utf-8'), 'baud': 115200, 'ids':[200, 201, 202, 203], 'protocol': 2, 'reader_addr': 126, 'reader_len': 2}):
		self.import_sdk(dynamixel_sdk_path)
		self.configure_reader(config)
		self.set_pro_params()
		self.init_state = self.read_state() # starting point 

	def enable_free_motion(self):
		# Enable free motion
		self.reader.Set_Value(self.reader.m1id, 64, 1, 0)
		self.reader.Set_Value(self.reader.m2id, 64, 1, 0)
		self.reader.Set_Value(self.reader.m4id, 64, 1, 0)

	def set_velocity(velocity):
		self.reader.Set_Value(self.reader.m1id, 104, 4, velocity)
		self.reader.Set_Value(self.reader.m2id, 104, 4, velocity)
		self.reader.Set_Value(self.reader.m3id, 104, 4, velocity)
		self.reader.Set_Value(self.reader.m4id, 104, 4, velocity)


	# reposition the arm
	def reposition(self):
		state = self.read_state()
		mid_state = 1950
		self.set_pos_slow(self.reader.m2id, state['p2'], mid_state)
		self.set_pos_slow(self.reader.m1id, state['p1'], self.init_state['p1'])
		self.set_pos_slow(self.reader.m2id, mid_state, self.init_state['p2'])
		# self.init_state = self.read_state() # starting point 
