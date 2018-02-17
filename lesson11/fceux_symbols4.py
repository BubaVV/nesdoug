#!/usr/bin/python3
#  written by rainwarrior, Brad Smith
#  http://forums.nesdev.com/viewtopic.php?f=10&t=11114
#
#  with a few changes by Doug Fraker

#  added

import sys

try:
	test = open("labels.txt", "r")
except:
	print("\nERROR: couldn't find file, labels.txt\n")
	raise

#  added
filename = sys.argv[1]
if filename == "":
	filename = "default"

#  original	
def label_to_nl(label_file, nl_file, range_min, range_max):
	labels = open(label_file, "rt").readlines()
	labs = {}
	sout = ""
	for line in labels:
		words = line.split()
		if (words[0] == "al"):
			adr = int(words[1], 16)
			sym = words[2]
			sym = sym.lstrip('.')
			if (sym[0] == '_' and sym[1] == '_'):
				continue # skip compiler internals
			if (adr >= range_min and adr <= range_max):
				if (adr in labs):
                    # multiple symbol
					text = labs[adr]
					textsplit = text.split()
					if (sym not in textsplit):
						text = text + " " + sym
						labs[adr] = text
				else:
					labs[adr] = sym
	for (adr, sym) in labs.items():
		sout += ("$%04X#%s#\n" % (adr, sym))
	open(nl_file, "wt").write(sout)
	print("debug symbols: " + nl_file)

#  these lines changed
if __name__ == "__main__":
	label_to_nl("labels.txt", filename+".nes.ram.nl", 0x0000, 0x7FFF)
	label_to_nl("labels.txt", filename+".nes.0.nl",   0x8000, 0xBFFF)
	label_to_nl("labels.txt", filename+".nes.1.nl",   0xC000, 0xFFFF)
