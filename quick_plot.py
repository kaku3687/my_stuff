############################################################
#Project	: Quick Plotter
#
#Version : v2.0
#
#Name		: quick_plot.py
#
#Author		: t_randhawa
#
#Date		: 11/09/2018
#
#Purpose	: Selects a .csv file and presents a gui for
#			  selecting two fields to plot. 
#		  
#
#Rev History	: v2.0 - Added 3D plotting functionality
#                     - Added clean up for selecting new data
#                       so that you don't have to reopen 
#
#TODO          : Add basic 2D regression
#############################################################

from tkinter import *
from tkinter.filedialog import askopenfilename
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from dyno_fxns import convert_np
import pandas as pd
import matplotlib.pyplot as plt
import sys
import numpy as np

row_ = 0
v_col = []
col_limit = 41

class data_file:
   def __init__(self):
      self.data_ = pd.DataFrame()
      self.fields = []
      self.num_cols = 0
      self.filename = ""
      self.c_count = 0
      self.header_ = 2
      self.units = 2
      self.d_start = 3
      self.c = []
      self.reg_ = ""

def get_rows():
   current_datafile.header_ = int(header_.get())
   current_datafile.units = int(units.get())
   current_datafile.d_start = int(d_start.get())
   current_datafile.reg_ = reg_.get()

# Print current button state to screen
def printSelection(i):
   print(v_col[i].get())

# Generate plot of parameters based on selection (first two only)
def make_plot():
   for col_ in range(current_datafile.num_cols):
      print(str(current_datafile.data_.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   x_column = columns_[0]
   y_column = columns_[1]
   x_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[x_column]]
   y_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[y_column]]

   title_ = current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]] + \
            " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]]

   plt.figure(1)
   plt.title(title_)
   plt.ylabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]])
   plt.xlabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]])
   plt.grid(True, which='major')

   plt.scatter(x_array, y_array, s=0.2, marker = ".")
   plt.show()

   return

# Generate 3d plot of parameters based on selection (first three only)
def make_3dplot():
   for col_ in range(current_datafile.num_cols):
      print(str(current_datafile.data_.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   x_column = columns_[0]
   y_column = columns_[1]
   z_column = columns_[2]
   
   x_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[x_column]]
   y_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[y_column]]
   z_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[z_column]]

   x_ = convert_np(x_array[0:10000])
   y_ = convert_np(y_array[0:10000])
   z_ = convert_np(z_array[0:10000])

   # x_, y_ = np.meshgrid(x_,y_)

   title_ = current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]] + \
            " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]] + \
            " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[z_column]]

   fig = plt.figure()
   ax = plt.axes(projection='3d')
   plt.title(title_)
   plt.xlabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]])
   plt.ylabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]])
   
   surf = ax.scatter3D(x_, z_, y_, c=y_, cmap='RdBu')
   
   plt.show()

   return


# Generate plot of parameters based on selection (first selection only)
def make_hist():
   for col_ in range(current_datafile.num_cols):
      print(str(current_datafile.data_.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   num_bins = 10

   x_column = columns_[0]

   x_array = current_datafile.data_.loc[d_start:, current_datafile.fields[x_column]]

   x_ = convert_np(x_array[0:50])

   fig, ax = plt.subplots()
   n, bins, patches = ax.hist(x_, num_bins)
   plt.title(title_)
   plt.show()

   return

def create_button(col_):
   return Checkbutton(master, text=current_datafile.data_.columns[col_], \
            variable=v_col[col_], command=lambda i=col_: printSelection(col_), \
            onvalue=1, offvalue=0)

def remove_button():
   for chk in current_datafile.c:
      chk.destroy()
   
# Select data
def select_data():
   if current_datafile.c_count == 0:
      pass
   else:
      remove_button()

   current_datafile.c = []
   # Read data and headers
   current_datafile.num_cols = 0
   current_datafile.fields = []
   current_datafile.data_ = pd.DataFrame()
   master.filename=askopenfilename()
   current_datafile.filename = master.filename

   print (master.filename)
   current_datafile.data_ = pd.read_csv(master.filename, header = current_datafile.header_, skiprows=0)
   current_datafile.data_ = current_datafile.data_[current_datafile.data_.columns.drop(list(current_datafile.data_.filter(regex=current_datafile.reg_)))]
   current_datafile.fields = list(current_datafile.data_.columns)
   current_datafile.num_cols = len(current_datafile.data_.columns)
   print(current_datafile.fields)

   # FIXME: Need a scrollbar so we don't have to worry about giant off-screen gui's
   if current_datafile.num_cols > col_limit:
      current_datafile.num_cols = col_limit
   else:
      pass


   # Create check-button list
   for col_ in range(current_datafile.num_cols):
      v_col.append(IntVar())
      v_col[-1].set(0)
      new_button = create_button(col_)
      current_datafile.c.append(new_button)

   for i in current_datafile.c:
      i.pack()

   for col_ in range(current_datafile.num_cols):
      print(str(current_datafile.data_.columns[col_]) + " : " + str(v_col[col_].get()))

   current_datafile.c_count += 1

print ("Number of args: " + str(len(sys.argv)))

if len(sys.argv) == 2:
	header_ = int(sys.argv[1])
else:
	pass


# Open file selection dialog
master = Tk()
tk_rows = Tk()

current_datafile = data_file()

#Create entry fields for header location, unit location and data start
Label(tk_rows, text="Header Start").grid(row=0)
Label(tk_rows, text="Units").grid(row=1)
Label(tk_rows, text="Data Start").grid(row=2)
Label(tk_rows, text="Regex to Ignore").grid(row=3)

header_ = Entry(tk_rows)
units = Entry(tk_rows)
d_start = Entry(tk_rows)
reg_ = Entry(tk_rows)

header_.grid(row=0, column=1)
units.grid(row=1, column=1)
d_start.grid(row=2, column=1)
reg_.grid(row=3, column=1)

Button(tk_rows, text='Enter', command=get_rows).grid(row=4)

# Create plot and quit buttons
Button(master, text='2DPlot', command=make_plot).pack()
Button(master, text='3DPlot', command=make_3dplot).pack()
#Button(master, text='Histogram', command=make_hist).pack()
Button(master, text='Select Data', command=select_data).pack()
Button(master, text='Quit', command=master.quit).pack()

tk_rows.mainloop()
master.mainloop()
