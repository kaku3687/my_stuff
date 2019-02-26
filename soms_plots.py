############################################################
#Project	      : DACEE LIFE TEST PLOTTING
#
#Version       : 
#
#Name		      : soms_plots.py
#
#Author		   : t_randhawa
#
#Date		      : 02/13/2019
#
#Purpose	      : Generates current and pwm from life test 
#					.csv files.
#		  
#
#Rev History	:
#
#
#TODO          : 
#############################################################

from tkinter import *
from tkinter import filedialog
from tkinter.filedialog import askopenfilename
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from dyno_fxns import convert_np
import pandas as pd
import matplotlib.pyplot as plt
import sys
import numpy as np
import os

HDR_START = 0
UNITS = 0
DATA_START = 1

X_1 = 'Time'
Y_1 = 'Mtr1 mA'
Y_2 = 'PWM1 %'

class data_file:
	def __init__(self):
		self.data_ = pd.DataFrame()
		self.filename = ""
		self.directory = ""
		self.c_count = 0
		self.header_ = 2
		self.units = 2
		self.d_start = 3
		self.testname = ''

		self.time = pd.DataFrame()
		self.curr = pd.DataFrame()
		self.pwm = pd.DataFrame()

def plot():
	return

def quit_all():
	master.quit()
	master.destroy()

def file_data():
	soms_data.data_ = pd.DataFrame()
	master.filename=askopenfilename()
	soms_.filename = master.filename

	soms_.testname = soms_data.filename.split('/')[-1]

	soms_.data = pd.read_csv(soms_datafilename, header=HDR_START)

	soms_.time = soms_pd.loc[1:, X_1]
	soms_.curr = soms_pd.loc[1:, Y_1]
	soms_.pwm = soms_pd.loc[1:, Y_2]

def sort_(array_):
	for i in range(1,len(array_)):
		j = i-1
		curr_ = array_[i]
		while j >= 0 and int(array_[j]) > int(curr_):
			array_[j+1] = array_[j]
			j = j-1
		array_[j+1] = curr_
	return array_

def select_data():
	master.directory=filedialog.askdirectory(title="Select directory with data...")
	soms_.directory = master.directory

	#List and sort files in directory
	test_files = os.listdir(soms_.directory)
	tests_ = []
	test_temp = ''
	date_ = []
	time_ = []
	for test_ in test_files:
		if '.csv' in test_:
			test_temp = test_.replace('.csv','')
			date_, time_ = test_temp.split('_')
			tests_.append(time_)
	tests_ = sort_(tests_)

	csv_files = []

	for tme_ in time_:
		csv_files.append(master.directory + '/' + date_ + '_' + tme_)
	

	print (csv_files)

master = Tk()

soms_ = data_file()

#def main():

# Launch GUI to select folder with days data
Button(master, text='Select Data', command=select_data).pack()
Button(master, text='Quit', command=quit_all).pack()

# Stitch together .csv files (just TIME, MTR1_MA, PWM1_%)


# Save Current and PWM plots in /Charts directory

master.mainloop()