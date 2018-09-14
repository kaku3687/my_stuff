#Test space for building python scripts

import datetime
import os
import shutil

date_ = str(datetime.date.today())
time_ = (datetime.datetime.now().time().strftime("%H-%M-%S"))
test_name = raw_input("Enter the name of this test:")

roslog_dir = "/home/micra/.ros/log/latest/"
home_dir = "/home/micra/"
path_ = home_dir + date_ + "_" + time_ + "_" + test_name

#Create directory
os.mkdir(path_)
os.chdir(path_)

#Open File with testname and date
filename = str(date_ + " " + time_ + " " + test_name  + ".txt")
file_ = open(filename,"w")

descrip_ = raw_input("Write a short description of this run:")

file_.write("Testname: " + test_name + "\n")
file_.write("Date: " + date_ + "\n")
file_.write("Time: " + time_ + "\n")
file_.write("\n")
file_.write(descrip_ + "\n")

#Close File
print (filename)
file_.close()

#Copy roslog contents into folder
path_ = path_ + "/" + "roslogs/"
os.mkdir(path_)

log_files = os.listdir(roslog_dir)
os.chdir(roslog_dir)

for files_ in log_files:
	shutil.copy(files_,path_)

#Create a zip folder
folder_n = date_ + "_" + time_ + "_" + test_name
os.chdir(home_dir)
zip_it = "zip -r " + folder_n + ".zip " + folder_n
os.system(zip_it)

#Try transferring test log to motiv servers
smba_login = "smbclient //mss01-nas/users/ -W motivss.local -U trandhawa -A ~/.smbclient.conf -D trandhawa"
smba_cmd = "-c \'lcd /home/micra; put " + folder_n + ".zip\'"

os.system(smba_login + " " + smba_cmd)
