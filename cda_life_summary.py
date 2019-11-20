import os
import csv
import pandas as pd
import numpy as np

log_dir
log_name

fields = ['Log_File', 
			'Date', 
			'Duration sec',
			'Steps In', 
			'Degrees In', 
			'Degrees Out', 
			'Average Speed rpm', 
			'Average Current mA', 
			'Average Temperature C']

jcb_data = pd.read_csv(log_dir + '/' + log_name, usecols=fields, header=0)
cda_data = jcb_data[[fields[0],fields[1],fields[3],fields[4],fields[5],fields[8]]]

logs_ = cda_data.loc[1:fields[0]]
dates_ = cda_data.loc[1:fields[1]]
steps_ = cda_data.loc[1:fields[3]]
deg_in_ = cda_data.loc[1:fields[4]]
deg_out_ = cda_data.loc[1:fields[5]]
temps_ = cda_data.loc[1:fields[8]]

