import os, sys

count = 1
for i in sys.argv:
	if i == '-i':
		infile = open(sys.argv[count],'rb')
		count += 1
		continue
	if i == '-o':
		out = open(sys.argv[count],'wb')
		count += 1
		continue
	count += 1

for i in infile:
	line = i.split()
	if line[0] == "node:":
		idx = len(line[5]) - 9
		addr = line[5][idx:-1]
		out.write("{}\n".format(addr))

infile.close()
out.close()