#! /usr/bin/env python

f = open("batchResult2.txt")

NAME = 1
DATA = 2

name = ""
meanStrain = 0.0
perSegmentStrain = 0.0
strainSum = 0.0
diff = 0.0
totalSum = 0.0

meanStrainDict = {}
perSegmentStrainDict = {}
strainSumDict = {}
diffDict = {}
totalSumDict = {}

minMeanStrain = 1e300
minPerSegmentStrain = 1e300
minStrainSum = 1e300
minDiff = 1e300
minTotalSum = 1e300

state = NAME

index = 0
for line in f:
	index += 1
	if (index == 10): break
	if state == NAME:
		if line.strip() == '':
			state = DATA
		else:
			print (line.strip())
			name = name + line

	elif state == DATA:
		if line.strip() != '':
			items = line.split()
			if (items[0] == 'cumulativeDiff'):
				diff = float(items[1])
				diffDict[name] = diff
			elif (items[0] == 'cumulativeStdDev'):
				meanStrain = float(items[1])
				meanStrainDict[name] = meanStrain
			elif (items[0] == 'cumulativePerSegmentStdDev'):
				perSegmentStrain = float(items[1])
				perSegmentStrainDict[name] = perSegmentStrain
		else:
			strainSum = meanStrain + perSegmentStrain
			totalSum = strainSum + diff

			if (meanStrain < minMeanStrain):
				minMeanStrainName = name
				minMeanStrain = meanStrain

			if (perSegmentStrain < minPerSegmentStrain):
				minPerSegmentStrain = perSegmentStrain
				minPerSegmentStrainName = name

			if (strainSum < minStrainSum):
				minStrainSum = strainSum
				minStrainSumName = name

			if (diff < minDiff):
				minDiff = diff
				minDiffName = name

			if (totalSum < minTotalSum):
				minTotalSum = totalSum
				minTotalSumName = name

			state = NAME

