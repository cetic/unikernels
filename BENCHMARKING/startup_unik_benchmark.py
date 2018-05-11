#!/usr/bin/python3
import sys
import subprocess
import os.path
import csv
import json
import time
import _thread
import os
import datetime as datetime
import random

BENCHMARK_TIME_SECONDS=60*20
MAX_NBR_VMS=150
NBR_VMS_INCREMENT=10
GUESTS=dict()
NBR_VMS=0

def run_cmd(COMMAND):
	return subprocess.run(COMMAND, stdout=subprocess.PIPE, shell=True)

def timeGuestStart(GUEST):
	response = 1
	startTime=datetime.datetime.now()
	while response is not 0:
		response = os.system("ping -c 1 " + GUESTS[GUEST] + "> /dev/null 2>&1")
	endTime=datetime.datetime.now()
	bootTime=(endTime-startTime).microseconds
	with open("startup_unik_benchmark.csv", "a") as file:
		writer = csv.writer(file)
		writer.writerow([NBR_VMS, GUEST, (bootTime/1000000)])	

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print("Application Startup Benchmarking Tool - CETIC Internship 2017/18 on Unikernels\n\tUsage: python3 startup_unik_benchmark.py [nbr of VMs]")
		sys.exit(1)
	
	NBR_VMS=int(sys.argv[1])
		
	if os.path.isfile("startup_unik_benchmark.csv") is not True:
		subprocess.run("touch startup_unik_benchmark.csv", stdout=subprocess.PIPE, shell=True)		
		with open("startup_unik_benchmark.csv", "w") as file:
			writer = csv.writer(file)
			writer.writerow(["NBR_OF_GUESTS", "GUEST_STOPPED", "STARTUP_TIME_SECONDS"])
	
	
	while NBR_VMS < MAX_NBR_VMS:
		print("### STOPPING PREVIOUS VMS")
		# stop all running VMs
		run_cmd("docker stop $(docker ps -a -q) 2> /dev/null")
		run_cmd("virsh list | grep running | awk '{ print $2}' | while read DOMAIN; do virsh destroy $DOMAIN; sleep 3; done")
		run_cmd("virsh list --all | grep VM_ | awk '{ print $2}' | while read DOMAIN; do virsh undefine $DOMAIN; sleep 3;	done")
		
		# create table of guest VMs
		GUESTS.clear()
		i = 10
		for x in range(int(NBR_VMS)):
			GUESTS[("VM_"+str(x))] = ("192.168.122." + str(i))
			i = i+1
		#print(GUESTS)
		
		run_cmd("sudo rm -rf GUESTS; mkdir GUESTS")
		# create and launch each VM
		print("### BUILDING UNIKERNEL IMAGES")
		for key, value in GUESTS.items():
			# modify config.json with the IP address
			data = {}  
			data["net"] = []  
			data["net"].append({  
				"iface": 0,
				"config": "static",
				"address": value ,
				"netmask": "255.255.255.0",
				"gateway": "192.168.122.1"
			})
			with open("UNIKERNEL/DNS/config.json", "w") as outfile:  
				json.dump(data, outfile)
			
			# for each entry in GUESTS build the unikernel and move the img to another folder
			run_cmd("cd UNIKERNEL/DNS && sudo rm -rf build && mkdir -p build && cd build && cmake .. && sudo make 2> /dev/null")
			run_cmd(("sudo mv UNIKERNEL/DNS/build/DNS.img GUESTS/" + key + ".img"))
			
			run_cmd(("virt-install \
						--virt-type qemu --name " + key + " \
						--vcpus 1 --ram 128 \
						--import --disk GUESTS/" + key +".img \
						--serial file,path=/tmp/" + key + ".log \
						--network network=default,model=virtio \
						--check path_in_use=off \
						--noautoconsole"))
			print(" - " + key + " built")
						
		print("### LAUNCHING BENCHMARK FOR " + str(BENCHMARK_TIME_SECONDS) + " SECONDS")					

		timeout = time.time() + BENCHMARK_TIME_SECONDS 
		while time.time() <= timeout:
			# TODO loop for a predetermined number of time
			# TODO stop a random VM, restart it, time its boot time
			# stop a VM
			RANDOM_GUEST="VM_"+str(random.randint(0, (int(NBR_VMS)-1)))
			run_cmd("virsh destroy " + RANDOM_GUEST)
			print(RANDOM_GUEST + " shut down")
			# launch a thread to start a VM
			_thread.start_new_thread(run_cmd, ("virsh start " + RANDOM_GUEST, ))
			# launch another thread to time boot time
			_thread.start_new_thread(timeGuestStart, (RANDOM_GUEST, ))
			print(RANDOM_GUEST + " started...")
			time.sleep(5)
		
		#increment number of VMs for next test
		NBR_VMS=NBR_VMS+NBR_VMS_INCREMENT
	
	# cleanup test bed
	run_cmd("virsh list --all | grep VM_ | awk '{ print $2}' | while read DOMAIN; do virsh destroy $DOMAIN; sleep 3; virsh undefine $DOMAIN; done")
	run_cmd("sudo rm -rf GUESTS")
	
	
	