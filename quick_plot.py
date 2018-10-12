############################################################
#Project	: Quick Plotter
#
#Name		: quick_plot.py
#
#Author		: t_randhawa
#
#Date		: 10/12/2018
#
#Purpose	: Selects a .csv file and presents a gui for
#			  selecting two fields to plot. 
#		  
#
#Rev History	: 
#
#############################################################

from tkinter import *
from tkinter.filedialog import askopenfilename
import pandas as pd
import matplotlib.pyplot as plt
import sys

row_ = 0
v_col = []
fields = []
header_ = 4
col_limit = 40

# Print current button state to screen
def printSelection(i):
   print(v_col[i].get())

# Generate plot of parameters based on selection (first two only)
def make_plot():
   for col_ in range(num_cols):
      print(str(data_.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   x_column = columns_[0]
   y_column = columns_[1]
   x_array = data_.loc[1:, fields[x_column]]
   y_array = data_.loc[1:, fields[y_column]]

   title_ = data_.at[0, fields[y_column]] + " vs. " + data_.at[0, fields[x_column]]

   plt.figure(1)
   plt.title(title_)
   plt.ylabel(data_.at[0, fields[y_column]])
   plt.xlabel(data_.at[0, fields[x_column]])
   plt.grid(True, which='major')

   plt.scatter(x_array, y_array, s=0.05, marker = ".")
   plt.show()

   return


if len(sys.argv) == 1:
	header_ = sys.argv[0]
else:
	pass

# Open file selection dialog
master = Tk()
master.filename=askopenfilename()
print (master.filename)

# Read all headers 
data_ = pd.read_csv(master.filename, header = header_)
fields = list(data_.columns)

num_cols = len(data_.columns)

# FIXME: Need a scrollbar so we don't have to worry about giant off-screen gui's
if num_cols > col_limit:
   num_cols = col_limit
else:
   pass

# Create check-button list
for col_ in range(num_cols):
   v_col.append(IntVar())
   v_col[-1].set(0)
   c = Checkbutton(master, text=data_.columns[col_], variable=v_col[col_], command=lambda i=col_: printSelection(col_), onvalue=1, offvalue=0)
   row_ = row_ + 1
   c.pack()

for col_ in range(num_cols):
   print(str(data_.columns[col_]) + " : " + str(v_col[col_].get()))

# Create plot and quit buttons
Button(master, text='Plot', command=make_plot).pack()
Button(master, text='Quit', command=master.quit).pack()

master.mainloop()
