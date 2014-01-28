######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################
#!/usr/bin/python

import json
from pprint import pprint

att_file=open("att.json")
att=json.load(att_file)

min=att['requestedMem']['min']
max=att['requestedMem']['max']
t=att['requestedMem']['timestamp']
size=len(att['requestedMem']['min'])

for i in range(size):
	print (t[i],t[i]/att['ticksPerSecond'], min[i], max[i])
