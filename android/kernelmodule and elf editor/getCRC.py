import struct, sys

base = 0xffffffc000080000;
count = 1
for i in sys.argv:
#	if i == '-b':
#		base = int(sys.argv[i+1], 16)
	if i == '-k':
		kernel = open(sys.argv[count],'rb')
		count += 1
		continue
	if i == '-m':
		map = open(sys.argv[count],'rb')
		count += 1
		continue
	if i == '-o':
		out = open(sys.argv[count],'wb')
		count += 1
		continue
	count += 1

for i in map:
	current = i.split()
	if current[2][0:10] == '__kcrctab_':
		offset = int(current[0],16) - base
		kernel.seek(offset)
		crc = struct.unpack('<Q',kernel.read(8))[0]
		out.write('{} {}\n'.format(hex(crc).strip('L'),current[2][10:]))

map.close()
out.close()
kernel.close()