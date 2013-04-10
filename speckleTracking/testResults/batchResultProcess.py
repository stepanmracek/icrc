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

minMeanStrain = 1e300
minPerSegmentStrain = 1e300
minStrainSum = 1e300
minDiff = 1e300
minTotalSum = 1e300

state = BEGIN

for line in f:
	if state == BEGIN:
		index = line.find('"')
		name = line[index+1:]
		state = NAME
	elif state == NAME:
		if line.strip() == '"':
			state = STDDEV
		else:
			name = name + line
	elif state == STDDEV:
		items = line.split()
		meanStrain = float(items[1])
		perSegmentStrain = float(items[2])
		strainSum = float(items[3])
		state = DIFF
	elif state == DIFF:
		items = line.split()
		diff = float(items[1])
		state = BEGIN

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

print("minTotalSum: %f\n" % minTotalSum)
print(minTotalSumName)
print()