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

def printStats(l, n):
	for i in l[0:n]:
		print i[0],i[1]
		print


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

printStats(sort(msDict), 5)
print '------------------------------------------------'
printStats(sort(pssDict), 5)
print '------------------------------------------------'
printStats(sort(diffDict), 5)
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
	ms = (msDict[i] - msMean) / msStd
	pss = (pssDict[i] - pssMean) / pssStd
	diff = (diffDict[i] - diffMean) / diffStd
	allDict[i] = ms + pss + diff

	print >>out, ms, pss, diff

for i in sort(allDict)[0:5]:
	print i[0],i[1]
	print msDict[i[0]]
	print pssDict[i[0]]
	print diffDict[i[0]]

print len(allDict)