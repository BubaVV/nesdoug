NAME = lesson8c
include ../general/general.mk

$(NAME).nes: $(NAME).o reset.o asm4cC.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o asm4cC.o nes.lib

asm4cC.o: asm4cC.s
	$(CA65) asm4cC.s

clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) *.o
