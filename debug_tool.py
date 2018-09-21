############################################################
#Project	: RoboMantis Debugging
#
#Name		: debug_tool.py
#
#Author		: t_randhawa
#
#Date		: 09/21/2018
#
#Purpose	: Initiate a run.py session with automatic 
#		  configuration, log collection and transfer.
#
#Rev History	: 
#
#############################################################


import datetime
import os
import shutil

ap_ip = "169.168.0.200"
lan_ip = "10.128.10.178"
eth_ip = "10.42.2.2"
vm_ip = "10.128.10.161"

ip_hostname = dict([
	(lan_ip, 'operator_lan')
	(ap_ip, 'operator_ap')
	(eth_ip, 'operator_eth')
	(vm_ip, 'operator_vm')
])

date_ = str(datetime.date.today())
time_ = (datetime.datetime.now().time().strftime("%H-%M-%S"))
test_name = raw_input("Enter the name of this test:")

roslog_dir = "/home/micra/.ros/log/latest/"
home_dir = "/home/micra/"
wks_dir = "/home/micra/micra_dev/src"
path_ = home_dir + date_ + "_" + time_ + "_" + test_name

#Create directory
os.mkdir(path_)
os.chdir(path_)

#Open File with testname and date
filename = str(date_ + " " + time_ + " " + test_name  + ".txt")
file_ = open(filename,"w")

descrip_ = raw_input("Write a short description of this run:")

#Get ethercat config
os.system("ethercat slaves | cat > ethercat_config")

#Get master
master_ = raw_input("What master will be running? l(ocal)/h(igh_brain)")
cur_mstr = os.popen("echo $ROS_MASTER_URI").read()

#Get operator
cur_opr = os.popen("echo $ROS_IP")
conn_who = os.popen("who").read()


if lan_ip in conn_who:
	new_opr = ip_hostname[lan_ip]
elif vm_ip in conn_who:
	new_opr = ip_hostname[vm_ip]
elif ap_ip in conn_who:
	new_opr = ip_hostname[ap_ip]
elif eth_ip in conn_who:
	new_opr = ip_hostname[eth_ip]	
else:
	print ("No Operator Connected!")
	exit 1

#Check current operator config
ssh = paramiko.SSHClient()

#Clear current operator
if new_opr not curr_opr:
	os.popen("cat ~/.bashrc | grep " + curr_opr).read()
	if "#" 
	os.system("sed -i s/export\ ROS_IP=" + cur_opr + "/#export\ ROS_IP=" + cur_opr)

#Set new operator
os.system("sed -i s/export\ ROS_IP=" + new_opr + "/")

if master_ == "l":
	operator_ = "localhost"
else
	operator_ = raw_input("What is the operator ID? ap/lan/vm/eth")

#Set bash
if operator_ == "ap":

#Get run.py parameters
runpy_param = raw_input("Enter the run.py configuration you wish to launch: ")

file_.write("Testname: " + test_name + "\n")
file_.write("Date: " + date_ + "\n")
file_.write("Time: " + time_ + "\n")
file_.write("\n")
file_.write(descrip_ + "\n")
file_.write("\n")
file_.write("\tOperator: " + operator_ + "\n")
file_.write("\n")
file_.write("\tRun.py config: " + runpy_param + "\n")
file_.write("\n")

#Close File
print (filename)
file_.close()


#Launch
os.chdir(wks_dir)
os.system("python run.py " + runpy_param)


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
