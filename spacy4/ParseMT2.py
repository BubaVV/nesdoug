#!/usr/bin/python3

# written by Doug Fraker 2016
# this file parses output from NES Screen Tool Metatile 'save bank'
# converts to a format ca65 can read
# only works for 1 metasprite at a time

import sys

try:
    sys.argv[1]
except (IndexError, NameError):
	print ("\n!!ERROR: expected filename as an argument!!")
	filename = input("Enter name of metasprite file now: ")
else:	
	filename = sys.argv[1]

oldfile = open(filename, 'rb')
newfile = open('metasprite.txt', 'w')  # warning, this will overwrite old file !!!!!!!!!!!!!!!!!!!!!

a = ""
b = 0
x = 0
y = 0
c = 0
X_offset = 0
Y_offset = 0

newfile.write("Metasprite:\n\n.byte    ;number of sprites\n\n")
x = oldfile.read(1) # first 2 are X,Y offsets
X_offset = int.from_bytes(x, byteorder='big', signed=False)
y = oldfile.read(1)
Y_offset = int.from_bytes(y, byteorder='big', signed=False)





for i in range(0, 64):
	b=oldfile.read(1)
	a=int.from_bytes(b, byteorder='big', signed=False)
	
	if (a == 255):
		break
	
	a=a-Y_offset
	a=hex(a & 0xff) # some trick from Stack Overflow to force 0-ff hex values, see link below
	a=a[2:]

	
	newfile.write(".byte $" + a + ", ")

	b=oldfile.read(1)
	a=hex(ord(b))
	a=a[2:]
	newfile.write("$" + a + ", ")

	b=oldfile.read(1)
	a=hex(ord(b))
	a=a[2:]
	newfile.write("$" + a + ", ")

	b=oldfile.read(1)
	a=int.from_bytes(b, byteorder='big', signed=False)
	a=a-X_offset
	a=hex(a & 0xff) # some trick from Stack Overflow to force 0-ff hex values, see link below
	a=a[2:]
	newfile.write("$" + a + "\n")
	
	c+=1 # count sprites
	
	
newfile.seek(21)
a = str(c)
newfile.write(a)
print (a + " sprites\n")

oldfile.close
newfile.close


# http://stackoverflow.com/questions/3235515/how-to-print-a-signed-integer-as-hexadecimal-number-in-twos-complement-with-pyt



