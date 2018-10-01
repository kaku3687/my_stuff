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
import subprocess
import paramiko
import time

#operator IPs
ap_ip = "169.168.0.200"
lan_ip = "10.128.10.178"
eth_ip = "10.42.2.2"
vm_ip = "10.128.10.161"

ip_hostname = dict([
	(lan_ip, 'operator_lan'),
	(ap_ip, 'operator_ap'),
	(eth_ip, 'operator_eth'),
	(vm_ip, 'operator_vm')
])

date_ = str(datetime.date.today())
operator_ = raw_input("Enter your name: ")
time_ = (datetime.datetime.now().time().strftime("%H-%M-%S"))
test_name = raw_input("Enter the name of this test: ")

roslog_dir = "/home/micra/.ros/log/latest/"
home_dir = "/home/micra/"
wks_dir = "/home/micra/micra/src"
path_ = home_dir + date_ + "_" + time_ + "_" + test_name

ssh = paramiko.SSHClient()

#Create directory on High_Brain
os.mkdir(path_)
os.chdir(path_)

#Open File with testname and date
filename = str(date_ + " " + time_ + " " + test_name  + ".txt")
file_ = open(filename,"w")

descrip_ = raw_input("Write a short description of this run: ")

#Get master
master_ = raw_input("What master will be running? l(ocal)/h(igh_brain)")
if master_ == 'l':
	master_ = "high_brain"
	iface_ = "csim"
elif master_ == 'h':
	master_ = "high_brain"
	iface_ = "rosc"
else:
	print ("Master not recognized!")
	exit()
print (master_)


#Get run.py parameters
#Mantis Control option:
armctl_ = raw_input("Launch Mantis Control? y/n: ")
drv_ = raw_input("Driving enabled? man/nav/none: ")
rit_ = raw_input("Robot Interaction tools? y/n: ")
aft_ = raw_input("Affordance Templates? y/n: ")
if rit_ == 'n' and aft_ == 'n':
	if drv_ == 'none':
		planner_ = raw_input("Planner to use? m/t/b: ")
	elif drv_ == 'man':
		planner_ = "m"
	else:
		planner_ = "b"
else:
	planner_ = "b"

runpy_param = "bmh " + iface_ + " -gn -f"
#Mantis Control
if armctl_ == 'y':
	runpy_param = runpy_param + " -c"
else:
	pass

#Driving
if drv_ == 'man' or drv_ == 'nav':
	runpy_param = runpy_param + " -v"
else:
	pass


#Craftsman Tools
runpy_param = runpy_param + " -p" + planner_


file_.write("Testname: " + test_name + "\n")
file_.write("Date: " + date_ + "\n")
file_.write("Time: " + time_ + "\n")
file_.write("\n")
file_.write(descrip_ + "\n")
file_.write("\n")
file_.write("\tOperator: " + operator_ + "\n")
file_.write("\n")
file_.write("\tRun.py config: " + runpy_param + "\n")
file_.write("\t\tMantis Control: " + armctl_  + "\n")
file_.write("\t\tDriving: " + drv_  + "\n")
file_.write("\t\tRobot Interaction Tools: " + rit_  + "\n")
file_.write("\t\tAffordance Templates: " + aft_  + "\n")
file_.write("\t\tPlanner: " + planner_ + "\n")
file_.write("\n")

#Close File
print (filename)
file_.close()
print("Test file written")

#exit()

#Get ethercat config from low_brain to high_brain
print ("Retrieving ethercat status from low_brain")
eth_cmd = "ethercat master | ssh high_brain 'cat > " + path_  + "/eth_config'"
ssh.load_system_host_keys()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect("low_brain")
st_in,st_out,st_err = ssh.exec_command(eth_cmd)
ssh.close()


#Get operator
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
	exit()
print(new_opr)


#Append user bash with ROS_IP, ROS_HOSTNAME and ROS_MASTER_URI
ssh.connect(new_opr)
st_in,st_out,st_err = ssh.exec_command("sed -i 's/OPERATOR=.*/OPERATOR=" + new_opr + "/' ~/.bashrc")
st_in,st_out,st_err = ssh.exec_command("sed -i 's/MASTER=.*/MASTER=" + master_ + "/' ~/.bashrc")
st_in,st_out,st_err = ssh.exec_command("source ~/.bashrc")
ssh.close()



#Launch
os.chdir(wks_dir)

if armctl_ == "y":
	if drv_ != "none":
		rnp = subprocess.Popen(["xterm","-e","python","/home/micra/micra/src/run.py","bmh",iface_,"-gn","-f","-p"+planner_,"-c","-v"])
	else:
		rnp = subprocess.Popen(["xterm","-e","python","/home/micra/micra/src/run.py","bmh",iface_,"-gn","-f","-p"+planner_,"-c"])

else:
	if drv_ != "none":
		rnp = subprocess.Popen(["xterm","-e","python","/home/micra/micra/src/run.py","bmh",iface_,"-gn","-f","-p"+planner_,"-v"])
	else:
		rnp = subprocess.Popen(["xterm","-e","python","/home/micra/micra/src/run.py","bmh",iface_,"-gn","-f","-p"+planner_])




#rnp = subprocess.Popen(["python","/home/micra/micra/src/run.py","bmh",iface_,"-gn","-f","-c","-v","-p"+planner_])

time.sleep(25)

if rit_ == 'y':
	subprocess.Popen(["xterm","/home/micra/bin/rit_run.sh"])
else:
	pass
if aft_ == 'y':
	subprocess.Popen(["xterm","/home/micra/bin/at_run.sh"]) 
else:
	pass
if drv_ == 'nav':
	subprocess.Popen(["xterm","/home/micra/bin/nav_run.sh"])
else:
	pass

rnp.wait()

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
if os.path.exists("~/.smbclient.conf"):
	smba_login = "smbclient //mss01-nas/users/ -W motivss.local -U trandhawa -A ~/.smbclient.conf -D trandhawa"
	smba_cmd = "-c \'lcd /home/micra; put " + folder_n + ".zip\'"

	os.system(smba_login + " " + smba_cmd)


#Then try to the operator station
else:
	os.chdir(home_dir)
	os.system("scp -r ./" + folder_n + ".zip " + new_opr + ":$HOME")


