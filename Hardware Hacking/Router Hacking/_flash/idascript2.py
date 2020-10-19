import idautils, os
from time import sleep

def searchtext(patterntext):
	result = []
	funcList = idautils.Functions()
	for func in funcList:
		currentFunc = idaapi.get_func(func)
		funcEnd = currentFunc.endEA
		addr = func
		while addr < funcEnd:
			asm = idc.GetDisasm(addr)
			if asm.find(patterntext) != -1:
				print("{}: {}".format(hex(addr)[:-1], asm))
				result.append(addr)
			addr = idc.next_head(addr)
	return result

def searchtext2(patterntext):
	result = []
	funcList = idautils.Functions()
	for func in funcList:
		currentFunc = idaapi.get_func(func)
		funcEnd = currentFunc.endEA
		addr = func
		while addr < funcEnd:
			asm = idc.GetDisasm(addr)
			if asm.find(patterntext) != -1:
				result.append(addr)
			addr = idc.next_head(addr)
	return result

def searchtext3(pattern1, pattern2):
	result = []
	funcList = idautils.Functions()
	for func in funcList:
		currentFunc = idaapi.get_func(func)
		funcEnd = currentFunc.endEA
		addr = func
		while addr < funcEnd:
			asm = idc.GetMnem(addr)
			asm1 = idc.GetDisasm(addr)
			if asm.find(pattern1) != -1 and asm1.find(pattern2) != -1:
				result.append(addr)
			addr = idc.next_head(addr)
	return result

def filterlist(list, type, mode, v):
	filtrate = []
	if type == 0:
		filter = "unk_"
	else:
		filter = "off_"
	
	for i in list:
		asm = idc.GetDisasm(i)
		idx = asm.index(filter)+4
		addr = int(asm[idx:idx+8],16)
		if mode == 0:
			if addr < v:
				filtrate.append(i)
		else:
			if addr > v:
				filtrate.append(i)
	return filtrate

def getcaller(callee):
	caller = []
	callers = 0
	funcList = idautils.Functions()
	for func in funcList:
		currentFunc = idaapi.get_func(func)
		funcEnd = currentFunc.endEA
		addr = func
		while addr < funcEnd:
			asm = idc.GetDisasm(addr)
			if asm.find(callee) > 0:
				if func not in caller:
					caller.append(func)
					callers += 1
			addr = idc.next_head(addr)
	print("Callers: {}".format(callers))
	return caller

def getaddress(list, shift):
	addr = ""
	addresses = []
	address = []
	for i in list:
		asm = idc.GetDisasm(i)
		idx = asm.find("0x")+2
		for j in asm[idx:]:
			if (ord(j) > 0x2F and ord(j) < 0x3A) or (ord(j) > 0x40 and ord(j) < 0x47):
				addr += j
			else:
				break
		addresses.append(int(addr, 16) << shift)
		addr = ""
	
	for i in addresses:
		if i not in address:
			address.append(i)
	
	return address

def getaddressfile(file):
	infile = open(file, 'rb')
	address = []
	for i in infile:
		line = i.split()
		if line[0] == "node:":
			idx = len(line[5]) - 9
			addr = line[5][idx:-1]
			address.append(int(addr,16))
	return address

def getaddressfile2(file):
	infile = open(os.path.abspath(file), 'rb')
	address = []
	for i in infile:
		address.append(int(i,16))
	return address

def funccallto(address, callee):
	callsto = []
	call = 0
	addr = address
	currentFunc = idaapi.get_func(addr)
	funcEnd = currentFunc.endEA
	while addr < funcEnd:
		asm = idc.GetDisasm(addr)
		if asm.find(callee) > 0:
			callsto.append(addr)
			call += 1
		addr = idc.next_head(addr)
	#print("Called {} times".format(call))
	return call
	
def tofunc(list):
	funclist = []
	for i in list:
		func = idaapi.get_func(i)
		funcstart = func.startEA
		if funcstart not in funclist:
			funclist.append(funcstart)
	return funclist

def mostcalls(callee):
	callerfunc = []
	callcount = []
	order = []
	highest = 0
	caller = getcaller(callee)
	for callers in caller:
		calls = funccallto(callers, callee)
		callcount.append(calls)
		callerfunc.append(callers)
		if calls > highest:
			highest = calls
	
	for i in range(0, highest):
		j = 0
		for call in callcount:
			if call > i and call < i+2:
				order.append(j)
			j += 1
	
	for rank in order:
		print("{}: {} times".format(hex(callerfunc[rank])[:-1], callcount[rank]))
	
def mostcallslist(list, callee):
	callerfunc = []
	callcount = []
	order = []
	highest = 0
	for callers in list:
		calls = funccallto(callers, callee)
		callcount.append(calls)
		callerfunc.append(callers)
		if calls > highest:
			highest = calls
	
	for i in range(0, highest):
		j = 0
		for call in callcount:
			if call > i and call < i+2:
				order.append(j)
			j += 1
	
	for rank in order:
		print("{}: {} times".format(hex(callerfunc[rank])[:-1], callcount[rank]))

def searchfunc(address, patterntext):
	results = []
	currentFunc = idaapi.get_func(address)
	funcEnd = currentFunc.endEA
	addr = address
	while addr < funcEnd:
		asm = idc.GetDisasm(addr)
		if asm.find(patterntext) != -1:
			print(hex(addr)[:-1])
			results.append(addr)
		addr = idc.next_head(addr)
	return results
	
def searchfunc2(address, pattern1, pattern2):
	results = []
	currentFunc = idaapi.get_func(address)
	funcEnd = currentFunc.endEA
	addr = address
	while addr < funcEnd:
		asm = idc.GetDisasm(addr)
		asm1 = idc.GetMnem(addr)
		if asm1.find(pattern1) != -1 and asm.find(pattern2) != -1:
			print(hex(addr)[:-1])
			results.append(addr)
		addr = idc.next_head(addr)
	return results
	
def searchfunclist(list, patterntext):
	results = []
	for address in list:
		currentFunc = idaapi.get_func(address)
		#print(hex(address)[:-1])
		funcEnd = currentFunc.endEA
		addr = address
		while addr < funcEnd:
			asm = idc.GetDisasm(addr)
			if asm.find(patterntext) > 0:
				if address not in results:
					print(hex(address)[:-1])
					results.append(address)
			addr = idc.next_head(addr)
	print("Results: {}".format(len(results)))
	return results

def searchlist(list, patterntext):
	results = []
	for address in list:
		asm = idc.GetDisasm(address)
		if asm.find(patterntext) > 0:
			if address not in results:
				#print(hex(address)[:-1])
				results.append(address)
	return results

def generatetree(address):
	currentFunc = idaapi.get_func(address)
	funcEnd = currentFunc.endEA
	addr = address
	calltree = []
	calls = 0
	while addr < funcEnd:
		asm = idc.GetDisasm(addr)
		if asm.find("sub_") > 0 and idc.GetMnem(addr) == "jal":
			call = int(asm[asm.index("sub_")+4:], 16)
			if call not in calltree:
				calltree.append(call)
				calls += 1
		addr = idc.next_head(addr)
	if calls == 0:
		return 0
	return calltree

def functree(tree):
	result = []
	for branch in tree:
		results = generatetree(branch)
		if result != 0:
			result.append(results)
	return result

def searchtree(tree):
	for branch in tree:
		if branch != 0:
			for twig in branch:
				searchfunc(twig, "PRINT_")
		else:
			continue

def printresult(list):
	for i in list:
		print("{}: {}".format(hex(i)[:-1],idc.GetDisasm(i)))
	print("Results: {}".format(len(list)))

def main(pattern1, pattern2, pattern3, pattern4):
	search1 = searchtext3("li", pattern1)
	search2 = searchtext3("li", pattern2)
	search3 = searchtext3("li", pattern3)
	search4 = searchtext3("li", pattern4)
	#search3 = searchtext3("sll", pattern3)
	addr1 = tofunc(search1)
	addr2 = tofunc(search2)
	addr3 = tofunc(search3)
	addr4 = tofunc(search4)
	#addr3 = tofunc(search3)
	
	for i in addr1:
		if i in addr2 and i in addr3 and i in addr4:
			print(hex(i)[:-1])
	#addresses = getaddress(search2, 16)
	#offsets = []
	#results = len(addresses)

"""	
	for addr in addresses:
		if addr < strptr:
			print(hex(strptr - addr)[:-1])
			offsets.append(str(hex(strptr - addr)[2:-1]).upper())
		if addr > strptr:
			print(hex(addr - strptr)[:-1])
			offsets.append(str(hex(addr - strptr)[2:-1]).upper())

	print("Results: {}".format(results))
	sleep(2)
	for j in offsets:
		print("Search for: \'{}\'".format(j))
		searchtext(j)
"""