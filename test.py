############################################################
#Project	    :
#
#Version       	: 
#
#Name		    : test.py
#
#Author		   	: t_randhawa
#
#Date		    : 
#
#Purpose	    : 
#		  
#
#Rev History	: 
#
#TODO          	: 
#############################################################

import numpy as np

def sort_(array_):
	for i in range(1,len(array_)):
		j = i-1
		curr_ = array_[i]
		while j >= 0 and array_[j] > curr_:
			array_[j+1] = array_[j]
			j = j-1
		array_[j+1] = curr_
	return array_


arr_ = np.random.randint(0, 20, 10)
print("Original Array:" + str(arr_))

new_arr_ = sort_(arr_)
print("Sorted Array:" + str(new_arr_))
