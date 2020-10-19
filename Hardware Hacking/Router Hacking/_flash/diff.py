import sys

in1 = open(sys.argv[1], 'rb')
in2 = open(sys.argv[2], 'rb')
in3 = open(sys.argv[3], 'rb')
in4 = open(sys.argv[4], 'rb')
out = open(sys.argv[5], 'wb')

list1 = []
list2 = []
list3 = []
list4 = []
list = []

for a in in1:
	list1.append(int(a,16))
	
for b in in2:
	list2.append(int(b,16))
	
for c in in3:
	list3.append(int(c,16))
	
for d in in4:
	list4.append(int(d,16))
	
for q in list1:
	if q not in list2 and q not in list3 and q not in list4:
		list.append(q)

for w in list2:
	if w not in list1 and w not in list3 and w not in list4:
		list.append(w)

for e in list3:
	if e not in list1 and e not in list2 and e not in list4:
		list.append(e)

for r in list4:
	if r not in list1 and r not in list2 and r not in list3:
		list.append(r)
		
for k in list:
	out.write("{}\n".format(hex(k)[:-1]))

in1.close()
in2.close()
in3.close()
in4.close()
out.close()