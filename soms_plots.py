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
s_ = 0.5
marker_ = "x"
_dpi_ = 600

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
		self.date = ''
		self.log_f = ''

		self.time = pd.DataFrame()
		self.curr = pd.DataFrame()
		self.pwm = pd.DataFrame()

# Calculate total seconds from weird DACEE timestamp
def time_calc(time_stamp):
   time_ = time_stamp.split(":")
   hour_ = float(time_[0])
   minute_ = float(time_[1])
   second_ = float(time_[2])

   time_ = hour_*3600 + minute_*60 + second_

   return time_

def plot():
	file_data()
	time_array = soms_.time.values
	for i in range(len(soms_.time)):
		time_array[i] = time_calc(time_array[i])
	soms_.time = time_array

	# title_ = current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]] + \
	        # " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]]

	os.chdir(soms_.directory)

	plt.figure(1)
	plt.title(soms_.log_f)
	plt.ylabel(Y_1)
	plt.xlabel(X_1)
	plt.grid(True, which='major')

	plt.scatter(soms_.time, soms_.curr, s=s_, marker = marker_)
	plt.savefig(Y_1 + "_" + soms_.log_f + '.png', dpi = _dpi_)

	plt.figure(2)
	plt.title(soms_.log_f)
	plt.ylabel(Y_2)
	plt.xlabel(X_1)
	plt.grid(True, which='major')

	plt.scatter(soms_.time, soms_.pwm, s=s_, marker = marker_)
	plt.savefig(Y_2 + "_" + soms_.log_f + '.png', dpi = _dpi_)

	return

def quit_all():
	master.quit()
	master.destroy()

def file_data():
	log_dir = soms_.directory + "/" + soms_.log_f

	soms_.data = pd.read_csv(log_dir, header=HDR_START)

	soms_.time = soms_.data.loc[1:, X_1]
	# print ("Soms Time: \n")
	# print (soms_.time)
	soms_.curr = soms_.data.loc[1:, Y_1]
	# print ("Soms Current: \n")
	# print (soms_.curr)
	soms_.pwm = soms_.data.loc[1:, Y_2]

	return

def sort_(array_):
	for i in range(1,len(array_)):
		j = i-1
		curr_ = array_[i]
		while j >= 0 and int(array_[j]) > int(curr_):
			array_[j+1] = array_[j]
			j = j-1
		array_[j+1] = curr_
	return array_

def stitch_f(array_):
	fname = 'Log_' + soms_.date + '.csv'
	soms_.log_f = fname
	os.chdir(soms_.directory)

	if fname in os.listdir():
		print ("Log file already exists!!")
		quit_all()
		return

	else:
		fout = open(fname,"a")

		for line in open(array_[0]):
			fout.write(line)
		for file in range(1,len(array_)):
			f = open(array_[file])
			f.__next__()
			for line in f:
				fout.write(line)
			f.close()
		fout.close()
		return


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

	soms_.date=date_
	csv_files = []

	for tme_ in tests_:
		csv_files.append(master.directory + '/' + date_ + '_' + tme_ + '.csv')
	

	print (csv_files)
	stitch_f(csv_files)

	print ("Generating Plots...")
	plot()

master = Tk()

soms_ = data_file()

#def main():

# Launch GUI to select folder with days data
Button(master, text='Select Data', command=select_data).pack()
#Button(master, text='Generate Plots', command=plot).pack()
Button(master, text='Quit', command=quit_all).pack()

# Stitch together .csv files (just TIME, MTR1_MA, PWM1_%)


# Save Current and PWM plots in /Charts directory

master.mainloop()