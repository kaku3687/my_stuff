############################################################
#Project	      : Quick Plotter
#
#Version       : v2.0
#
#Name		      : quick_plot.py
#
#Author		   : t_randhawa
#
#Date		      : 11/09/2018
#
#Purpose	      : Selects a .csv file and presents a gui for
#			        selecting two fields to plot. 
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
s_ = 0.5
marker_ = "x"

c_title=''
x_title=''
y_title=''
set_title=False

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
      self.tab_d = BooleanVar()
      self.t_stamp = BooleanVar()
      self.f_disp = Text(master, height=1, width=30)
      self.s_titles=IntVar()

def get_titles():
   titles_ = Tk()
   
   Label(titles_, text="Chart Title").grid(row=0, column=0)
   Label(titles_, text="X Axis").grid(row=1, column=0)
   Label(titles_, text="Y Axis").grid(row=2, column=0)

   chart_ = Entry(titles_)
   x_axis = Entry(titles_)
   y_axis = Entry(titles_)

   chart_.grid(row=0, column=1)
   x_axis.grid(row=1, column=1)
   y_axis.grid(row=2, column=1)

   def set_titles():
      global c_title
      global x_title
      global y_title

      c_title=chart_.get()
      x_title=x_axis.get()
      y_title=y_axis.get()
      
      global set_title
      set_title=True

      titles_.quit()
      titles_.destroy()
      return

   Button(titles_, text='Enter', command=set_titles).grid(row=3)
  
   titles_.mainloop()

def quit_all():
   tk_rows.quit()
   tk_rows.destroy()
   master.quit()
   master.destroy()

def get_rows():
   current_datafile.header_ = int(header_.get())
   current_datafile.units = int(units.get())
   current_datafile.d_start = int(d_start.get())
   current_datafile.reg_ = reg_.get()

   head_v = Label(tk_rows, text=header_.get()).grid(row=0, column=2)
   unit_v = Label(tk_rows, text=units.get()).grid(row=1, column=2)
   d_startv = Label(tk_rows, text=d_start.get()).grid(row=2, column=2)
   reg_v = Label(tk_rows, text=reg_.get()).grid(row=3, column=2)

# Print current button state to screen
def printSelection(i):
   print(v_col[i].get())

# Calculate total seconds from weird DACEE time
def time_calc(time_stamp):
   time_ = time_stamp.split(":")
   hour_ = float(time_[0])
   minute_ = float(time_[1])
   second_ = float(time_[2])

   time_ = hour_*3600 + minute_*60 + second_

   return time_

# Generate plot of parameters based on selection (first two only)
def make_plot():
   if current_datafile.s_titles.get() == True:
      get_titles()

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
   time_array = []

   if current_datafile.t_stamp.get() == True and current_datafile.fields[x_column] == 'Time':
      x_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[x_column]]
      time_array = x_array.values
      for i in range(len(time_array)):
         time_array[i] = time_calc(time_array[i])
      x_array = time_array
   else:
      x_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[x_column]]
   y_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[y_column]]

   # title_ = current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]] + \
            # " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]]

   title_ = ''

   plt.figure(1)
   global set_title
   if set_title==True:
      plt.title(c_title)
      plt.ylabel(y_title)
      plt.xlabel(x_title)
      set_title=False
   else:
      plt.ylabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]])
      plt.xlabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]])
   plt.grid(True, which='major')

   plt.scatter(x_array, y_array, s=s_, marker = marker_)
   plt.show()

  
   return

def make_1Dplot():
   if current_datafile.s_titles.get() == True:
      get_titles()

   for col_ in range(current_datafile.num_cols):
      print(str(current_datafile.data_.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   y_column = columns_[0]
   y_array = current_datafile.data_.loc[current_datafile.d_start:, current_datafile.fields[y_column]]
   x_column = np.arange(0, len(y_array))
   x_array = x_column

   # title_ = current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]] + \
            # " vs. " + current_datafile.data_.at[current_datafile.units, current_datafile.fields[x_column]]

   title_ = ''

   plt.figure(1)
   global set_title
   if set_title==True:
      plt.title(c_title)
      plt.ylabel(y_title)
   else:
      plt.ylabel(current_datafile.data_.at[current_datafile.units, current_datafile.fields[y_column]])
   plt.grid(True, which='major')

   plt.scatter(x_array, y_array, s=s_, marker = marker_)
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
   print(current_datafile.tab_d)

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

   test_name = current_datafile.filename.split('/')[-1]
   current_datafile.f_disp.config(state=NORMAL)
   current_datafile.f_disp.delete('1.0',END)
   current_datafile.f_disp.pack()
   current_datafile.f_disp.insert(END, test_name)
   current_datafile.f_disp.config(state=DISABLED)

   print (master.filename)
   if current_datafile.tab_d.get() == True:
      current_datafile.data_ = pd.read_csv(master.filename, header = current_datafile.header_, skiprows=0, sep='\t')  
   else:
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

head_v = Label(tk_rows, text=" ").grid(row=0, column=2)
unit_v = Label(tk_rows, text=" ").grid(row=1, column=2)
d_startv = Label(tk_rows, text=" ").grid(row=2, column=2)
reg_v = Label(tk_rows, text=" ").grid(row=3, column=2)

header_.grid(row=0, column=1)
units.grid(row=1, column=1)
d_start.grid(row=2, column=1)
reg_.grid(row=3, column=1)

Button(tk_rows, text='Enter', command=get_rows).grid(row=4)

# Create plot and quit buttons
Button(master, text='1DPlot', command=make_1Dplot).pack()
Button(master, text='2DPlot', command=make_plot).pack()
Button(master, text='3DPlot', command=make_3dplot).pack()
#Button(master, text='Histogram', command=make_hist).pack()
Button(master, text='Select Data', command=select_data).pack()
Checkbutton(master, text='Tab Delim', variable=current_datafile.tab_d).pack()
Checkbutton(master, text='Timestamp', variable=current_datafile.t_stamp).pack()
Checkbutton(master, text='Set Titles', variable=current_datafile.s_titles).pack()
Button(master, text='Quit', command=quit_all).pack()

tk_rows.mainloop()
master.mainloop()
