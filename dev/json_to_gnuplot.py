#!/usr/bin/python
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 01/2014
#    LICENSE  : CeCILL-C
#    FILE     : dev/json_to_gnuplot.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
############################################################

############################################################
import json
import sys
from pprint import pprint

############################################################
att_file=open(sys.argv[1],'r')
att=json.load(att_file)
if len(sys.argv) == 3:
	selected=sys.argv[2]
else:
	selected='requestedMem'

############################################################
min=att[selected]['min']
max=att[selected]['max']
t=att[selected]['timestamp']
size=len(att[selected]['min'])

############################################################
for i in range(size):
	print ("%d	%f	%d	%d"%(t[i],float(t[i])/float(att['ticksPerSecond']), min[i], max[i]))
