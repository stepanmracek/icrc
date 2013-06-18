#! /usr/bin/env python

f = open("batchResult4.txt")

NAME = 1
DATA = 2
state = NAME

perSegmentStrainDict = {}
strainDict = {}
diffDict = {}
name = ''

for line in f:
	if state == NAME:
		if line.strip() == '':
			state = DATA
		else:
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
			name = ''

print("minMeanStrain: %f" % minMeanStrain)
print(minMeanStrainName)
print()

print("minPerSegmentStrain: %f" % minPerSegmentStrain)
print(minPerSegmentStrainName)
print()

print("minStrainSum: %f" % minStrainSum)
print(minStrainSumName)
print()

print("minDiff: %f" % minDiff)
print(minDiffName)
print()

print("minTotalSum: %f" % minTotalSum)
print(minTotalSumName)
print()