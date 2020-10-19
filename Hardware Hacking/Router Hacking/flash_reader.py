import serial, sys, binascii
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

sleep(2)

while(1):
	mode, spi_command, spi_byte0, spi_byte1, spi_byte2, spi_byte3, spi_setting, output_bytes = raw_input("> ").split()
	if mode == "exit":
		break
	ser.write(bytearray([int(mode, 16), int(spi_command, 16), int(spi_byte0, 16), int(spi_byte1, 16), int(spi_byte2, 16), int(spi_byte3, 16), int(spi_setting, 16)]))
	print(binascii.hexlify(ser.read(int(output_bytes, 16))))

ser.close()