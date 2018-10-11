from tkinter import *
from tkinter.filedialog import askopenfilename
import pandas as pd
import numpy as numpy

# import tkinter as tk
# import Pmw

# class choiceBox( Frame ):
#    def __init__( self, listItems ):
#       Frame.__init__( self )
#       self.pack( expand = YES, fill = BOTH )
#       self.master.title( "Select" )

#       self.listBox = Pmw.ScrolledListBox( self,
#                                           items = listItems,
#                                           listbox_height = 5,
#                                           vscrollmode = "static",
#                                           listbox_selectmode = EXTENDED )
#       self.listBox.pack( side = LEFT, expand = YES, fill = BOTH,padx = 5, pady = 5 )

#       self.copyButton = Button( self,text = ">>>", command = self.addColor )
#       self.copyButton.pack( side = LEFT, padx = 5, pady = 5 )

#       self.chosen = Pmw.ScrolledText( self,text_height = 6,text_width = 20 )
#       self.chosen.pack( side = LEFT, expand = YES, fill = BOTH,padx = 5, pady = 5 )

#    def addColor( self ):
#       self.chosen.clear()
#       selected = self.listBox.getcurselection()

#       if selected:
#          for item in selected:
#             self.chosen.insert( END, item + "\n" )

# colorNames = ( "A", "B", "C", "D")
# choiceBox( colorNames ).mainloop()

row_ = 0
v_col = []

def make_plot():
   for col_ in range(num_cols):
      print(str(dyno_data.columns[col_]) + " : " + str(v_col[col_].get()))

   columns_ = []
   for check_ in range(len(v_col)):
      if v_col[check_].get() == True:
         columns_.append(check_)
      else:
         pass

   print(columns_)

   x_column = columns_[0]
   y_column = columns_[1]
   x_array = dyno_data.loc[1:, x_column]
   y_array = dyno_data.loc[1:, y_column]

   title_ = dyno_data.at[0, y_column] + " vs. " + dyno_data.at[0, x_column]

   plt.figure(1)
   plt.title(title_)
   plt.ylabel(dyno_data.at[0, y_column])
   plt.xlabel(dyno_data.at[0, x_column])
   plt.grid(True, which='major')

   plt.scatter(x_array, y_array, s=0.05, marker = ".")
   plt.show()

   return


# def printSelection(i):
#    print(v_col[i].get())

def printSelection():
    print(v_curr.get())
    print(v_torq.get())
    print(v_vel_.get())

fields = [' fm_elevation_cur',
         ' dyno_torque_value',
         'rel_time_sec',
         ' fm_elevation_vel',
         ' fm_elevation_goal_current',
         ' fm_elevation_pwm_limit']
header_ = 4

root = Tk()
root.withdraw()
root.filename=askopenfilename()
print (root.filename)

dyno_data = pd.read_csv(root.filename, header = header_)
dyno_data_ = dyno_data[[fields[0], fields[1], fields[2], fields[3]]]

u_curr_ = dyno_data.at[0, fields[0]]
u_torq_ = dyno_data.at[0, fields[1]]
u_rel_t_ = dyno_data.at[0, fields[2]]
u_vel_ = dyno_data.at[0, fields[3]]

curr_ = dyno_data_.loc[1:,fields[0]]
torq_ = dyno_data_.loc[1:,fields[1]]
rel_t_ = dyno_data_.loc[1:,fields[2]]
vel_ = dyno_data_.loc[1:,fields[3]]

master = Tk()

num_cols = len(dyno_data.columns)

if num_cols > 10:
   num_cols = 10
else:
   pass

# for col_ in range(num_cols):
#    v_col.append(IntVar())
#    v_col[-1].set(0)
#    c = Checkbutton(master, text=dyno_data.columns[col_], variable=v_col[col_], command=lambda i=col_: printSelection(col_), onvalue=1, offvalue=0)
#    # listbox.insert(END, Checkbutton(master, text=dyno_data.columns[col_], variable=v_col).grid(row=row_, sticky=W))
#    row_ = row_ + 1
#    c.pack()

# for col_ in range(num_cols):
#    print(str(dyno_data.columns[col_]) + " : " + str(v_col[col_].get()))

# Button(master, text='Plot', command=make_plot).pack()
# Button(master, text='Quit', command=master.quit).pack()

# Button(master, text='Plot', command=make_plot).grid(row=len(dyno_data.columns)+1, sticky=W, pady=4)
# Button(master, text='Quit', command=master.quit).grid(row=len(dyno_data.columns)+2, sticky=W, pady=4)

v_curr = IntVar()
v_curr.set(0)
v_torq = IntVar()
v_torq.set(0)
v_vel_ = IntVar()
v_vel_.set(0)

Checkbutton(master, text="Current", variable = v_curr, command=printSelection(), onvalue=1, offvalue=0, state=ACTIVE).pack()

Checkbutton(master, text="Torque", variable = v_torq, command=printSelection(), onvalue=1, offvalue=0, state=ACTIVE).pack()

Checkbutton(master, text="Velocity", variable = v_vel_, command=printSelection(), onvalue=1, offvalue=0, state=ACTIVE).pack()

Button(master, text='Plot', command=printSelection).pack()
Button(master, text='Quit', command=master.quit).pack()

master.mainloop()
