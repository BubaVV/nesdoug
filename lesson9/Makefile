NAME = lesson9
include ../general/general.mk

.DEFAULT_GOAL := All

All: $(NAME).nes $(NAME)b.nes 


# i really dont know how to correctly define all these rules
$(NAME)b.o: $(NAME)b.s
	$(CA65) $(NAME)b.s


$(NAME).nes: $(NAME)b.o reset.o asm4c.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o asm4c.o nes.lib


$(NAME)b.s: $(NAME)b.c
	$(CC65) -Oi $(NAME)b.c --add-source

$(NAME)b.nes: $(NAME)b.o resetB.o asm4c.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME)b.nes resetB.o $(NAME)b.o asm4c.o nes.lib

resetB.o: resetB.s
	$(CA65) resetB.s


clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) $(NAME)b.s
	$(RM) *.o
