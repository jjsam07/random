import linecache
import sys

# Reserved prefixes
#
# byte_ word_ sub_ off_ 

inputName = raw_input("Input file [System.map]: ")

if inputName == '':
	inputName = "System.map"

outputName = "symbols.idc"
try:
	fin = open(inputName, "r")
except:
	print("{}: No such file or directory".format(inputName))
	sys.exit()
	
seen = []
reserved_prefix = ['byte_','word_']
reserved_prefix2 = ['sub_','off_','unk_','loc_']
fout = open(outputName, "w")
makename = "MakeName(0x{}, \"{}\");\n"
makecode = "MakeCode(0x{});\n"
fout.write("#include <idc.idc>\nstatic main(){\n")
i = 1
multiname = ""
dupcounter = 0

for x in fin in range(0,105):
	current = x.split()
	temp = linecache.getline(inputName, i+1)
	temp2 = linecache.getline(inputName, i-1)
	
	if i == 1:
		previous = ["","",""]
	else:
		previous = temp2.split()
		
	nextt = temp.split()
		
	if temp == '':
		nextt = ["","",""]
		
	if nextt[0] == current[0]:
		multiname += current[2]+".$."
		previous = current
		i += 1
		continue
	elif previous[0] == current[0]:
		multiname += current[2]
		if current[1] in ['t','T']:
			fout.write(makecode.format(current[0]))
		if multiname[:5] in reserved_prefix or multiname[:4] in reserved_prefix2:
			fout.write(makename.format(current[0], "_"+multiname))
		else:
			fout.write(makename.format(current[0], multiname))
		multiname = ""
		i += 1
		continue

	if current[2] not in seen:		
		if current[1] in ['t','T']:
			fout.write(makecode.format(current[0]))
		if current[2][:5] in reserved_prefix or current[2][:4] in reserved_prefix2:
			fout.write(makename.format(current[0], "_"+current[2]))
		else:
			fout.write(makename.format(current[0], current[2]))
		seen.append(current[2])
	else:		
		if current[1] in ['t','T']:
			fout.write(makecode.format(current[0]))
		if current[2][:5] in reserved_prefix or current[2][:4] in reserved_prefix2:
			fout.write(makename.format(current[0], "_"+current[2]+"$duplicate"+str(dupcounter)))
		else:
			fout.write(makename.format(current[0], current[2]+"$duplicate"+str(dupcounter)))
		dupcounter += 1
	i += 1
	
fout.write("}")
fin.close()
fout.close()