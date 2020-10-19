import serial, sys
from time import sleep

ser = serial.Serial()
ser.port = 'COM3'
ser.baudrate = 1000000
ser.bytesize = 8
ser.parity = 'N'
ser.stopbits = 1
ser.timeout = 1.0
ser.write_timeout = 1.0
ser.open()

sleep(3)

binary_file = open(sys.argv[1], 'wb')

upper = 0
middle = 0

ser.write(bytearray([0, 0, 0, 0, 0, 0, 0]))
sleep(3)

while(((upper << 8) | (middle)) < 0x8000):
	sys.stdout.write("{:.2f}%\r".format((float((upper << 8)|(middle))*100) / float(0x7FFF)))
	sys.stdout.flush()
	binary_file.write(ser.read(256))
	if middle != 0xff:
		middle += 1
	else:
		middle = 0
		if upper != 0xff:
			upper += 1
		else:
			break
	
binary_file.close()
ser.close()