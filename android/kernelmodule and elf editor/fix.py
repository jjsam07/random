import sys, struct, zlib

#inputFile = raw_input("File: ")
#shift = raw_input("[0] Shifted by:")
#patched = 0 # index of patched section

affectedSect= [] # index of affected sections
#amountShift = 0

#if shift == '':
	#amountShift = 0
#else:
#	amountShift = int(shift)

#f = 0

#ELF header
e_shoff = 0x20 # start of the section header table 4-bytes
e_shnum = 0x30 # number of entries in the section header table 2-bytes
e_shstrndx = 0x32 # index of the section header table entry that contains the section names 2-bytes

#Section header //all 4-bytes
sh_name = 0x0
sh_offset = 0x10
sh_size = 0x14

myCRC = 0xfac6783c #crc that is compared against the kernel

def get_RawData(addr, size):
	f.seek(addr)
	if size == 1:
		return f.read(1)
	elif size == 2:
		return f.read(2)
	elif size == 4:
		return f.read(4)
		
def get_Data(addr, size):
	f.seek(addr)
	if size == 1:
		return struct.unpack('b', f.read(1))[0]
	elif size == 2:
		return struct.unpack('<H', f.read(2))[0]
	elif size == 4:
		return struct.unpack('<I', f.read(4))[0]

def get_ShdrByIndex(index):
	idx = index * 40
	f.seek(get_Data(e_shoff,4)+idx-amountShift)
	return f.read(40)

def get_ShdrAddrByIndex(index):
	idx = index * 40
	return get_Data(e_shoff,4)+idx-amountShift

def get_ShdrMember(Shdr, index):
	return struct.unpack('<I', Shdr[index:index+4])[0]

def get_Section(addr, size):
	f.seek(addr)
	return f.read(size)
	
def get_SectName(strAddr, idx):
	loc = strAddr + idx
	f.seek(loc)
	name = ""
	c = "a"
	while c != '\x00':
		c = f.read(1)
		if c != '\x00':
			name += c
		else:
			break
	return name

def get_AffectedSect():
	patchedAddr = get_ShdrMember(get_ShdrByIndex(patched),sh_offset)
	currentSectAddr = 0
	for i in range(0, get_Data(e_shnum,2)):
		if i == patched:
			continue
		currentSectAddr = get_ShdrMember(get_ShdrByIndex(i),sh_offset)
		if currentSectAddr > patchedAddr:
			affectedSect.append(i)

def applyPatch():
	outFile.seek(get_ShdrAddrByIndex(patched)+sh_size)
	outFile.write(struct.pack('<I',get_ShdrMember(get_ShdrByIndex(patched),sh_size)-amountShift))
	for i in affectedSect:
		outFile.seek(e_shoff)
		outFile.write(struct.pack('<I',get_Data(e_shoff,4)-amountShift))
		outFile.seek(get_ShdrAddrByIndex(i)+sh_offset)
		outFile.write(struct.pack('<I', get_ShdrMember(get_ShdrByIndex(i),sh_offset)-amountShift))

def patchVersion():
	isAffected = 0
	for i in affectedSect:
		if versionIndex == i:
			isAffected = 1

	if isAffected > 0:
		outFile.seek(get_ShdrMember(get_ShdrByIndex(versionIndex),sh_offset)-amountShift)
	else:
		outFile.seek(get_ShdrMember(get_ShdrByIndex(versionIndex),sh_offset))

	outFile.write(struct.pack('<I',myCRC))

def getHeaders():
	global f 
	f = open(sys.argv[3],'rb')
	global amountShift
	amountShift = int(sys.argv[2])
	print("Section Header at: {}".format(hex(get_Data(e_shoff,4)-amountShift)))
	print("String table at: {}\nIndices: {}".format(hex(get_ShdrMember(get_ShdrByIndex(get_Data(e_shstrndx,2)),sh_offset)-amountShift),get_Data(e_shnum,2)))
	for i in range(0,get_Data(e_shnum,2)):
		offset = get_ShdrMember(get_ShdrByIndex(i),sh_offset)
		size = get_ShdrMember(get_ShdrByIndex(i),sh_size)
		_crc = hex(zlib.crc32(get_Section(offset,size)) & 0xffffffff)[:-1]
		name = get_SectName(get_ShdrMember(get_ShdrByIndex(get_Data(e_shstrndx,2)),sh_offset)-amountShift,get_ShdrMember(get_ShdrByIndex(i),sh_name))
		print("I: {} Offset: {} Size: {} CRC: {} Name: {}".format(i, hex(offset), hex(size), _crc, name))
	f.close()
	sys.exit()

def setPatch():
	global f
	global outFile
	global patched
	global amountShift
	global versionIndex
	f = open(sys.argv[3],'rb')
	amountShift = int(sys.argv[2])
	for i in range(0,get_Data(e_shnum,2)):
		name = get_SectName(get_ShdrMember(get_ShdrByIndex(get_Data(e_shstrndx,2)),sh_offset)-amountShift,get_ShdrMember(get_ShdrByIndex(i),sh_name))
		if name == '__versions':
			versionIndex = i
		if name == '.gnu.linkonce.this_module':
			patched = i

	get_AffectedSect()
	f.seek(0)
	outFile = open(sys.argv[3][:-3]+'_'+'fixed.ko','wb')
	outFile.write(f.read())
	applyPatch()
	patchVersion()
	######### patch relocation offset #####################
	outFile.seek(get_ShdrMember(get_ShdrByIndex(patched+1),sh_offset)+8-amountShift)
	print(hex(get_ShdrMember(get_ShdrByIndex(patched+1),sh_offset)+8-amountShift))
	outFile.write(struct.pack('<I',get_Data(get_ShdrMember(get_ShdrByIndex(patched+1),sh_offset)+8-amountShift,4)-amountShift))
	print(hex(get_Data(get_ShdrMember(get_ShdrByIndex(patched+1),sh_offset)+8-amountShift,4)))
	#######################################################
	outFile.close()
	f.close()
	sys.exit()

def extractSect():
	indexSect = int(sys.argv[2])
	global f
	f = open(sys.argv[3],'rb')
	name = get_SectName(get_ShdrMember(get_ShdrByIndex(get_Data(e_shstrndx,2)),sh_offset),get_ShdrMember(get_ShdrByIndex(indexSect),sh_name))
	#w = open('nullbyte','wb')
	#w.write(name)
	#w.close()
	outFile = open(sys.argv[3][:-3]+'_'+name[:-1],'wb')
	outFile.write(get_Section(get_ShdrMember(get_ShdrByIndex(indexSect),sh_offset),get_ShdrMember(get_ShdrByIndex(indexSect),sh_size)))
	outFile.close()
	f.close()
	sys.exit()



if len(sys.argv) > 1:
	if sys.argv[1] == '-r':
		getHeaders()
	if sys.argv[1] == '-p':
		setPatch()
	if sys.argv[1] == '-x':
		extractSect()

print("No arguments")