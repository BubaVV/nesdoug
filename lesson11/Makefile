NAME = lesson11
include ../general/general.mk

#Should work both with Py2 and Py3
PYTHON = python

.DEFAULT_GOAL := All

All: $(NAME).nes $(NAME)b.nes 	
	python fceux_symbols4.py lesson11

# i really dont know how to correctly define all these rules
$(NAME)b.o: $(NAME)b.s
	$(CA65) $(NAME)b.s

$(NAME)b.s: $(NAME)b.c
	$(CC65) -Oi $(NAME)b.c --add-source


$(NAME).nes: $(NAME).o reset.o asm4c.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o asm4c.o nes.lib -Ln labels.txt

$(NAME)b.nes: $(NAME)b.o reset.o asm4c.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME)b.nes reset.o $(NAME)b.o asm4c.o nes.lib -Ln labels.txt

clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) $(NAME)b.s
	$(RM) *.o
	$(RM) labels.txt
	$(RM) *.nl
