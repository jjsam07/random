import idautils
from time import sleep

def init_vars():
	global addr
	global funcEnd
	global returnaddr
	global depth
	
	addr = 0
	funcEnd = 0
	returnaddr = []
	depth = -1

def about_to_jump_sub(address):
	global fromjump
	global addr
	global funcEnd
	global returnaddr
	global depth
	returnaddr.append(idc.next_head(address))
	inst = idc.GetDisasm(address)
	idx = inst.index("sub_")+4
	addr = int(inst[idx:],16)
	currentFunc = idaapi.get_func(addr)
	funcEnd = currentFunc.endEA
	depth += 1
	fromjump = 1
	
def about_to_jump_loc(address):
	global addr
	global fromjump
	inst = idc.GetDisasm(address)
	idx = inst.index("loc_")+4
	addr = int(inst[idx:],16)
	fromjump = 1
	

def about_to_return():
	global addr
	global funcEnd
	global returnaddr
	global depth
	addr = returnaddr[depth]
	currentFunc = idaapi.get_func(addr)
	funcEnd = currentFunc.endEA
	returnaddr.pop()
	depth -= 1

def loopfunc(address):
	init_vars()
	global addr
	global funcEnd
	global fromjump
	calls = 0
	fromjump = 0
	addr = address
	currentFunc = idaapi.get_func(addr)
	funcEnd = currentFunc.endEA 
	while addr < funcEnd:
		asm = idc.GetDisasm(addr)
		mnem = idc.GetMnem(addr)
		if mnem == "jal" or mnem == "jalr" or mnem == "b" or mnem == "bal" or mnem == "j":
			if asm.find("sub_") > 0:
				about_to_jump_sub(addr)
			#if asm.find("loc_") > 0:
				#about_to_jump_loc(addr)
		if mnem == "jr":
			if asm.find("$ra") > 0:
				about_to_return()
		
		#print(asm)
		#sleep(0.25)
		#sys.stdout.write("\r{}".format(hex(addr)[:-1]))
		#sys.stdout.flush()
		if asm.find("PRINT") > 0:
			calls += 1
			#print(calls)
		
		if fromjump == 0:
			addr = idc.next_head(addr)
		else:
			fromjump = 0
	print(calls)