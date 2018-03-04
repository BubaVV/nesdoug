NAME = lesson13
include ../general/general.mk

$(NAME).nes: $(NAME).o reset.o asm4c.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o asm4c.o nes.lib

clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) *.o
