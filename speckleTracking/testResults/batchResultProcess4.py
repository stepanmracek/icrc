#! /usr/bin/env python2
import numpy
import operator

f = open("batchResult4.txt")

NAME = 1
DATA = 2
state = NAME

msDict = {}
pssDict= {}
diffDict = {}
allDict = {}
name = ''

def sort(d):
	return sorted(d.iteritems(), key=operator.itemgetter(1))

for line in f:
	if state == NAME:
		if line.strip() == '':
			state = DATA
		else:
			name = name + line

	elif state == DATA:
		if line.strip() == '':
			if name in msDict:
				print name
				exit()

			msDict[name] = ms
			pssDict[name] = pss
			diffDict[name] = diff
			
			name = ''
			state = NAME

		else:
			items = line.split()
			ms = float(items[0])
			pss = float(items[1])
			diff = float(items[2])

msSorted = sort(msDict)
print msSorted[0][0], msSorted[0][1]
print '------------------------------------------------'
pssSorted = sort(pssDict)
print pssSorted[0][0], pssSorted[0][1]
print '------------------------------------------------'
diffSorted = sort(diffDict)
print diffSorted[0][0], diffSorted[0][1]
print '------------------------------------------------'

msValues = msDict.values()
msMean = numpy.mean(msValues)
msStd = numpy.std(msValues)

pssValues = pssDict.values();
pssMean = numpy.mean(pssValues)
pssStd = numpy.std(pssValues)

diffValues = diffDict.values()
diffMean = numpy.mean(diffValues)
diffStd = numpy.std(diffValues)

out = open('distribution', 'w')
for i in msDict:
	msDict[i] = (msDict[i] - msMean) / msStd
	pssDict[i] = (pssDict[i] - pssMean) / pssStd
	diffDict[i] = (diffDict[i] - diffMean) / diffStd
	allDict[i] = msDict[i] + pssDict[i] + diffDict[i]

	print >>out, msDict[i], pssDict[i], diffDict[i]



allSorted = sort(allDict)
print allSorted[0][0], allSorted[0][1]

print len(allSorted)