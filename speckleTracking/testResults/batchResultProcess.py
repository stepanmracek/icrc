#! /usr/bin/env python

f = open("batchResult.txt")

BEGIN = 1
NAME = 2
STDDEV = 3
DIFF = 4

name = ""
meanStrain = 0.0
perSegmentStrain = 0.0
strainSum = 0.0
diff = 0.0
totalSum = 0.0
state = BEGIN

for line in f:
	if state == BEGIN:
		index = line.find('"')
		name = line[index:]
		print(name)
