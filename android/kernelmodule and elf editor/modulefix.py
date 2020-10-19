import sys, struct, zlib
	
def get_RawData(addr, size, f):
	f.seek(addr)
	if size == 1:
		return f.read(1)
	elif size == 2:
		return f.read(2)
	elif size == 4:
		return f.read(4)
	elif size == 4:
		return f.read(4)

def get_Data(addr, size, f):
	f.seek(addr)
	if size == 1:
		return struct.unpack('B', f.read(1))[0]
	elif size == 2:
		return struct.unpack('<H', f.read(2))[0]
	elif size == 4:
		return struct.unpack('<I', f.read(4))[0]
	elif size == 8:
		return struct.unpack('<Q', f.read(8))[0]
		
def write_Data(addr, data, size, f):
	f.seek(addr)
	if size == 1:
		f.write(struct.pack('B', data))
	elif size == 2:
		f.write(struct.pack('<H', data))
	elif size == 4:
		f.write(struct.pack('<I', data))
	elif size == 8:
		f.write(struct.pack('<Q', data))


def _e_phoff(f):
	if endianness == 1:
		return get_Data(0x1C, 4, f)
	else:
		return get_Data(0x20, 8, f)
def _e_shoff(f):
	if endianness == 1:
		return get_Data(0x20, 4, f)
	else:
		return get_Data(0x28, 8, f)		
def _e_phentsize(f):
	if endianness == 1:
		return get_Data(0x2A, 2, f)
	else:
		return get_Data(0x36, 2, f)
def _e_shentsize(f):
	if endianness == 1:
		return get_Data(0x2E, 2, f)
	else:
		return get_Data(0x3A, 2, f)
def _e_shnum(f):
	if endianness == 1:
		return get_Data(0x30, 2, f)
	else:
		return get_Data(0x3C, 2, f)
def _e_phnum(f):
	if endianness == 1:
		return get_Data(0x2C, 2, f)
	else:
		return get_Data(0x38, 2, f)
def _e_shstrndx(f):
	if endianness ==  1:
		return get_Data(0x32, 2, f)
	else:
		return get_Data(0x3E, 2, f)

def init_offsets_and_constants(f):
	#ELF header
	global e_phoff
	global e_shoff
	global e_phentsize # 2-bytes
	global e_phnum # 2-bytes
	global e_shentsize # 2-bytes
	global e_shnum # 2-bytes
	global e_shstrndx # 2-bytes
	
	#Program header
	global p_offset
	global p_vaddr
	global p_filesz
	
	#Section header
	global sh_name # 4-bytes 
	global sh_type # 4-bytes
	global sh_flags
	global sh_addr
	global sh_offset
	global sh_size
	global sh_link # 4-bytes
	global sh_info # 4-bytes
	global sh_addralign
	global sh_entsize
	
	#Misc.
	global member_size
	global endianness

	endianness = get_Data(4, 1, f)
	
	if endianness == 1: # 32-bit
	
		#ELF header
		e_phoff = 0x1C
		e_shoff = 0x20
		e_phentsize = 0x2A
		e_shentsize = 0x2E
		e_shnum = 0x30
		e_phnum = 0x2C
		e_shstrndx = 0x32
		
		#Program header
		p_offset = 0x04
		p_vaddr = 0x08
		p_filesz = 0x10
		
		#Section header
		sh_name = 0x0
		sh_type = 0x04
		sh_flags = 0x08
		sh_addr = 0x0C
		sh_offset = 0x10
		sh_size = 0x14
		sh_link = 0x18
		sh_info = 0x1C
		sh_addralign = 0x20
		sh_entsize = 0x24
		
		member_size = 4
		
	else: # 64-bit
	
		#ELF header
		e_phoff = 0x20
		e_shoff = 0x28
		e_phentsize = 0x36
		e_shentsize = 0x3A
		e_shnum = 0x3C
		e_phnum =  0x38
		e_shstrndx = 0x3E
		
		#Program header
		p_offset = 0x08
		p_vaddr = 0x10
		p_filesz = 0x20
		
		#Section header
		sh_name = 0x0
		sh_type = 0x04
		sh_flags = 0x08
		sh_addr = 0x10
		sh_offset = 0x18
		sh_size = 0x20
		sh_link = 0x28
		sh_info = 0x2C
		sh_addralign = 0x30
		sh_entsize = 0x38
		
		member_size = 8

def get_ShdrByIndex(index, f):
	size = _e_shentsize(f)
	idx = index * _e_shentsize(f)
	seek = _e_shoff(f)+idx
	f.seek(seek)
	return f.read(size)

def get_ShdrAddrByIndex(index, f):
	idx = index * _e_shentsize(f)
	return _e_shoff(f)+idx

def get_ShdrMember(shdrIndex, member, f):
	if member == sh_name:
		return struct.unpack('<I', get_ShdrByIndex(shdrIndex, f)[sh_name:sh_name+4])[0]
	if member == sh_type:
		return struct.unpack('<I', get_ShdrByIndex(shdrIndex, f)[sh_type:sh_type+4])[0]
	if endianness == 1:
		return struct.unpack('<I', get_ShdrByIndex(shdrIndex, f)[member:member+member_size])[0]
	else:
		return struct.unpack('<Q', get_ShdrByIndex(shdrIndex, f)[member:member+member_size])[0]
	
def get_PhdrByIndex(index, f):
	size = _e_phentsize(f)
	idx = index * _e_phentsize(f)
	f.seek(_e_phoff(f)+idx)
	return f.read(size)

def get_PhdrAddrByIndex(index, f):
	idx = index * _e_phentsize(f)
	return _e_phoff(f)+idx

def get_PhdrMember(phdrIndex, member, f):
	if endianness == 1:
		return struct.unpack('<I', get_PhdrByIndex(phdrIndex, f)[member:member+member_size])[0]
	else:
		return struct.unpack('<Q', get_PhdrByIndex(phdrIndex, f)[member:member+member_size])[0]
	
def get_SectName(shdrIndex, f):
	shstrtab_offset = get_ShdrMember(_e_shstrndx(f), sh_offset, f)
	strIndex = get_ShdrMember(shdrIndex, sh_name, f)
	f.seek(shstrtab_offset+strIndex)
	name = ""
	c = "a"
	while c != '\x00':
		c = f.read(1)
		if c != '\x00':
			name += c
		else:
			break
	return name

def get_AffectedSect(patched_hdr_idx ,f):
	affectedSect = []
	patchedAddr = get_ShdrMember(patched_hdr_idx, sh_offset, f)
	for i in range(0, e_shnum):
		if i == patched_hdr_idx:
			continue
		if get_ShdrMember(i, sh_offset, f) > patchedAddr:
			affectedSect.append(i)
	return affectedSect

def extract_Section(shdrIndex, f):
	f.seek(get_ShdrMember(shdrIndex, sh_offset, f))
	return f.read(get_ShdrMember(shdrIndex, sh_size, f))

def patch_modversion(fIn, fOut, crcFile): #modulefix.py -p [INFILE] -o [OUTFILE] -crc [CRCFILE]
	fIn.seek(0)
	fOut.seek(0)
	fOut.write(fIn.read())
	versionIndex = 0
	for i in range(0, e_shnum):
		if get_SectName(i, fIn) == '__versions':
			versionIndex = i

	entries = get_ShdrMember(versionIndex, sh_size, fIn) / 64
	offset = get_ShdrMember(versionIndex, sh_offset, fIn)
	print(entries)
	name = ''
	for b in range(0, entries):
		c = 'a'
		fIn.seek(offset+(64*b)+member_size)
		while c != '\x00':
			c = fIn.read(1)
			if c != '\x00':
				name += c
			else:
				crcFile.seek(0)
				for j in crcFile:
					l = j.split()
					if l[1] == name:
						fOut.seek(offset+(64*b))
						print("Patching [{}]: {} at {}\n".format(b+1, name, hex(offset+(64*b))))
						if endianness == 1:
							fOut.write(struct.pack('<I', int(l[0], 16)))
							break
						else:
							fOut.write(struct.pack('<Q', int(l[0], 16)))
							break
				name = ''
				break
	fIn.close()
	fOut.close()
	crcFile.close()
	sys.exit()

def patch_sect_size(fIn, fOut, idx, shift_size): #modulefix.py -p [INFILE] -o [OUTFILE] -i [Section Index] -shift [Bytes shifted]
	affectedSect = get_AffectedSect(idx, fIn)
	phdr_to_patch = []
	
	for k in range(0, e_phnum):
		if get_PhdrMember(k, p_offset, fIn) > get_ShdrMember(idx, sh_offset, fIn):
			phdr_to_patch.append(k)
	
	fIn.seek(0)
	fOut.seek(0)
	fOut.write(fIn.read())
	
	#patch e_shoff
	write_Data(e_shoff_addr, e_shoff+shift_size, member_size, fOut)
	#patch sh_size
	write_Data(get_ShdrAddrByIndex(idx, fIn)+sh_size, get_ShdrMember(idx, sh_size, fIn)+shift_size, member_size, fOut)
	
	for i in affectedSect:
		print("Patching {}".format(get_SectName(i, fIn)))
		write_Data(get_ShdrAddrByIndex(i, fIn)+sh_offset, get_ShdrMember(i, sh_offset, fIn)+shift_size, member_size, fOut)

	for j in phdr_to_patch:
		write_Data(get_PhdrAddrByIndex(j, fIn)+p_offset, get_PhdrMember(j, p_offset, fIn)+shift_size, member_size, fOut)
		#write_Data(get_PhdrAddrByIndex(j, fIn)+p_vaddr, get_PhdrMember(j, p_vaddr, fIn)+shift_size, member_size, fOut)
	
	fIn.close()
	fOut.close()
	sys.exit()

def patch_ShdrMember(i, member, newValue, f): #modulefix.py -p [FILE] -i [Section Header Index] -m [MEMBER] -new [New Value] 
	shdraddr = get_ShdrAddrByIndex(i, f)
	if member == 'sh_name':
		write_Data(shdraddr+sh_name, newValue, 4, f)
		f.close()
		sys.exit()
	if member == 'sh_type':
		write_Data(shdraddr+sh_type, newValue, 4, f)
		f.close()
		sys.exit()
	if member == 'sh_flags':
		write_Data(shdraddr+sh_flags, newValue, member_size, f)
		f.close()
		sys.exit()
	if member == 'sh_addr':
		write_Data(shdraddr+sh_addr, newValue, member_size, f)
		f.close()
		sys.exit()
	if member == 'sh_offset':
		write_Data(shdraddr+sh_offset, newValue, member_size, f)
		f.close()
		sys.exit()
	if member == 'sh_size':
		write_Data(shdraddr+sh_size, newValue, member_size, f)
		f.close()
		sys.exit()
	if member == 'sh_link':
		write_Data(shdraddr+sh_link, newValue, 4, f)	
		f.close()
		sys.exit()		
	if member == 'sh_info':
		write_Data(shdraddr+sh_info, newValue, 4, f)
		f.close()
		sys.exit()
	if member == 'sh_addralign':
		write_Data(shdraddr+sh_addralign, newValue, member_size, f)
		f.close()
		sys.exit()
	if member == 'sh_entsize':
		write_Data(shdraddr+sh_entsize, newValue, member_size, f)
		f.close()
		sys.exit()
	print("Invalid section header member")
	f.close()
	sys.exit()

def add_section(name, size, load, fIn, fOut): #modulefix.py -p [INFILE] -a -load [(OPTIONAL) Load Address] -name [Section Name] -size [Section Size] -o [OUTFILE]
	for i in range(0, _e_shnum(fIn)):
		if get_SectName(i, fIn) == '.text':
			header = get_ShdrByIndex(i, fIn)
	
	namelen = len(name)+1
	first = get_ShdrMember(_e_shstrndx(fIn), sh_offset, fIn)+get_ShdrMember(_e_shstrndx(fIn), sh_size, fIn)
	padding = 0
	padding2 = 0
	
	if ((first+namelen) & 0x3) > 0:
		padding = 4 - ((first+namelen) & 0x3)
	if ((padding+namelen+size+64) & 0xf) > 0:
		size += 16 - ((padding+namelen+size+64) & 0xf)
	newheader = ''
	newheader += struct.pack('<I', get_ShdrMember(_e_shstrndx(fIn), sh_size, fIn))
	newheader += header[sh_type:sh_type+4]
	newheader += header[sh_flags:sh_flags+member_size]
	
	if load > 0:
		if (load & 0x3) > 0:
			padding2 = 4 - (load & 0x3)
		newheader += struct.pack('<Q', load+padding2)
	else:
		newheader += struct.pack('<Q', get_ShdrMember(_e_shstrndx(fIn), sh_offset, fIn)+get_ShdrMember(_e_shstrndx(fIn), sh_size, fIn)+namelen+padding)
		
	newheader += struct.pack('<Q', get_ShdrMember(_e_shstrndx(fIn), sh_offset, fIn)+get_ShdrMember(_e_shstrndx(fIn), sh_size, fIn)+namelen+padding)
	newheader += struct.pack('<Q', size)
	newheader += header[sh_link:sh_link+4]
	newheader += header[sh_info:sh_info+4]
	newheader += header[sh_addralign:sh_addralign+member_size]
	newheader += header[sh_entsize:sh_entsize+member_size]
		
	fOut.seek(0)
	fIn.seek(0)
	fOut.write(fIn.read(first))
	fOut.write(name)
	fOut.write(struct.pack('B', 0))
	fOut.write((struct.pack('B', 0)*padding))
	print('{}'.format(hex(first+namelen)))
	print('{}'.format((first+namelen) & 0xf))
	print ((first+namelen) & 0xf) > 0
	print('padding {}'.format(padding))
	print('padding2 {}'.format(padding2))
	fOut.write((struct.pack('B', 0)*size))
	fOut.write(fIn.read())
	
	fOut.write(newheader)
	
	#patch e_shnum
	write_Data(e_shnum, _e_shnum(fIn)+1, 2, fOut)
	
	#patch e_shoff
	write_Data(e_shoff, _e_shoff(fIn)+size+namelen+padding, member_size, fOut)
	
	#patch .shstrtab size
	write_Data(get_ShdrAddrByIndex(_e_shstrndx(fIn), fIn)+namelen+size+padding+sh_size,  get_ShdrMember(_e_shstrndx(fIn), sh_size, fIn)+namelen, member_size, fOut)
	fIn.close()
	fOut.close()
	sys.exit()

def getHeaders(f): #modulefix.py -h [FILE]
	print('{}'.format(_e_shstrndx(f)))
	f.seek(0)
	print("Section Header at: {}".format(hex(_e_shoff(f))))
	print("String table at: {}\nEntries: {}".format(hex(get_ShdrMember(_e_shstrndx(f), sh_offset, f)), _e_shnum(f)))
	print("{:4}{:11s}{:11s}{:11s}{:26s}{:11s}{:20s}{:12s}{:12s}".format('Ent', 'Start', 'End', 'VirtAddr', 'Size      (in Decimal)', 'CRC', 'Name', 'Type', 'Flag'))
	print("{:4}{:11s}{:11s}{:11s}{:26s}{:11s}{:20s}{:12s}{:12s}".format('---', '-----', '---', '--------', '----------------------', '---', '----', '----', '----'))
	for i in range(0, _e_shnum(f)):
		startoffset = get_ShdrMember(i, sh_offset, f)
		size = get_ShdrMember(i, sh_size, f)
		endoffset = (startoffset+size)-1
		virtaddr = get_ShdrMember(i, sh_addr, f)
		_crc = hex(zlib.crc32(extract_Section(i, f)) & 0xffffffff)[:-1]
		name = get_SectName(i, f)
		type = get_ShdrMember(i, sh_type, f)
		flags = get_ShdrMember(i, sh_flags, f)
		if endoffset < 0:
			endoffset = 0
		if virtaddr == startoffset:
			virtaddr = 0
		print("{:<4d}{:11s}{:11s}{:11s}{:11s}{:<15d}{:11s}{:20s}{:12s}{:12s}".format(i, hex(startoffset), hex(endoffset), hex(virtaddr), hex(size), size, _crc, name, hex(type), hex(flags)))
			
	f.seek(0)
	print("Program Header at: {}".format(hex(_e_phoff(f))))
	print("Entries: {}".format(_e_phnum(f)))
	print("{:8}{:15s}{:15s}{:15s}".format('Index', 'Offset', 'VirtAddr', 'Size'))
	print("{:8}{:15s}{:15s}{:15s}".format('-----', '------', '--------', '----'))
	for i in range(0, _e_phnum(f)):
		offset = get_PhdrMember(i, p_offset, f)
		size = get_PhdrMember(i, p_filesz, f)
		vaddr = get_PhdrMember(i, p_vaddr, f)
		print("{:<8d}{:15s}{:15s}{:15s}".format(i, hex(offset), hex(vaddr), hex(size)))	
	
	f.close()
	sys.exit()

def main():
	count = 1
	if sys.argv[1] == '-p':
		for i in sys.argv:
			if i == '-crc':
				crcFile = open(sys.argv[count], 'rb')
				count += 1
				continue
			if i == '-o':
				fOut = open(sys.argv[count], 'wb')
				count += 1
				continue
			if i == '-shift':
				shift_size = int(sys.argv[count])
				count += 1
				continue
			if i == '-size':
				size = int(sys.argv[count])
				count += 1
				continue
			if i == '-i':
				index = int(sys.argv[count])
				count += 1
				continue
			if i == '-n':
				name = sys.argv[count]
				count += 1
				continue
			if i == '-load':
				load = int(sys.argv[count], 16)
				count += 1
				continue
			if i == '-m':
				member = sys.argv[count]
				count += 1
				continue
			if i == '-new':
				newValue = int(sys.argv[count], 16)
				count += 1
				continue
			count += 1
		fIn = open(sys.argv[2], 'rb')
		init_offsets_and_constants(fIn)
		if '-shift' in sys.argv:
			patch_sect_size(fIn, fOut, index, shift_size)
		if '-a' in sys.argv:
			if '-load' in sys.argv:
				add_section(name, size, load, fIn, fOut)
			else:
				add_section(name, size, 0, fIn, fOut)
		if '-m' in sys.argv:
			fIn.close()
			fIn = open(sys.argv[2], 'rb+')
			patch_ShdrMember(index, member, newValue, fIn)
		patch_modversion(fIn, fOut, crcFile)

	if sys.argv[1] == '-h':
		f = open(sys.argv[2], 'rb')
		init_offsets_and_constants(f)
		getHeaders(f)
	
	if sys.argv[1] == '-x':
		f = open(sys.argv[2], 'rb')
		fout = open(sys.argv[3], 'wb')
		i = int(sys.argv[4])
		init_offsets_and_constants(f)
		fout.write(extract_Section(i ,f))
	print("No arguments")
	sys.exit()

if __name__ == '__main__':
	main()