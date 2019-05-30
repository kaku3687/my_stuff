import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

file_ = 'C:/Users/Owner/Documents/GZ4_SampleSet/GZ4_60-150_peak60_01041501.mdf'

headers_ = ['Time', 'Torque 1 (oz.in)', 'Angle 1', 'Angle 2']

data_ = pd.read_csv(file_, sep = '\t')
time_d = data_.loc[0:,headers_[0]]
torq_d = data_.loc[0:,headers_[1]]
angle_d = data_.loc[0:,headers_[3]]

rc_prev = ((torq_d[5]-torq_d[0])/5)
for i in range(10,len(torq_d)):
	rc_ = ((torq_d[i] - torq_d[i-10])/10)
	if abs(rc_ - rc_prev) > .05:
		print ("Torque Drop!!: ", torq_d[i])
		rc_prev = rc_
	else:
		rc_prev = rc_
