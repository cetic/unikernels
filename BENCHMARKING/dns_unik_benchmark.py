#!/usr/bin/python3
import sys
import subprocess
import shutil
import os.path
import csv
import time

RUN=1
TIME=300
CLIENTS=100
THREADS=4
QPS=100
QPS_INCREMENT=100

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print("DNS Server Benchmarking Tool - CETIC Internship 2017/18 on Unikernels\n\tUsage: python3 dns_unik_benchmark.py [server_ip_address]")
		sys.exit(1)
	
	SVR_IP=sys.argv[1]
	
	if shutil.which("dnsperf") is None:
		print("ERROR: dnsperf is not installed, please run the install_bench_tools.sh script.")
		sys.exit(1)
		
	if os.path.isfile("dns_unik_benchmark.csv") is not True:
		subprocess.run("touch dns_unik_benchmark.csv", stdout=subprocess.PIPE, shell=True)
	
	with open("dns_unik_benchmark.csv", "w") as file:
		writer = csv.writer(file)
		writer.writerow(["TEST_RUN", "QUERIES_SENT", "QUERIES_COMPLETED", "QUERIES_LOST", "QUERIES_PER_SECOND", "AVG_LATENCY_SECONDS", "MIN", "MAX", "LATENCY_STD_DEV_SECONDS"])
		
	while True:
		print("########## LAUNCHING TEST " + str(RUN) + " ##########")
		print("Queries per seconds: " + str(QPS)+ "\n")
		# launch the dnsperf command
		result = subprocess.run("dnsperf -s " + SVR_IP + " -d queryfile-example-current -l " + str(TIME) + " -T " + str(THREADS) + " -c " + str(CLIENTS) + " -Q " + str(QPS), stdout=subprocess.PIPE, shell=True)
		#print(result.stdout.decode('utf-8'))
		
		# convert output and store it in the csv file
		list = result.stdout.decode("utf-8").split('\n')
		for index in range(len(list)):
			if list[index].startswith("  Queries sent:"):
				queriesSent=list[index].split()[2]
				queriesCompleted=list[index+1].split()[2]
				queriesLost=list[index+2].split()[2]
				queriesPerSecond=list[index+7].split()[3]
				avgLatency=list[index+9].split()[3]
				latencyMin=list[index+9].split()[5].replace(",", "")
				latencyMax=list[index+9].split()[7].replace(")", "")
				latencyStdDev=list[index+10].split()[3]
				#print("TEST_RUN, QUERIES_SENT, QUERIES_COMPLETED, QUERIES_LOST, QUERIES_PER_SECOND, AVG_LATENCY_SECONDS, MIN, MAX, LATENCY_STD_DEV_SECONDS")
				print("Queries sent:\t\t" + queriesSent + "\nQueries completed:\t" + queriesCompleted + "\nQueries Lost:\t\t" + queriesLost + \
						"\nQueries per second:\t" + queriesPerSecond + "\nAverage latency:\t" + avgLatency + "\nMinimum latency:\t" + latencyMin + \
						"\nMax latency:\t\t" + latencyMax + "\nLatency Std Dev:\t" + latencyStdDev)
				with open("dns_unik_benchmark.csv", "a") as file:
					writer = csv.writer(file)
					writer.writerow([RUN, queriesSent, queriesCompleted, queriesLost, queriesPerSecond, avgLatency, latencyMin, latencyMax, latencyStdDev])
				break
		
		print("########## TEST " + str(RUN) + " COMPLETE ##########\n")
		# increase the TEST_RUN value and increment the number of queries per second
		RUN=RUN+1
		QPS=QPS+QPS_INCREMENT
		# sleep for 30 seconds between tests
		time.sleep(30)