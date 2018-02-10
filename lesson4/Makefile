NAME = lesson4
include ../general/general.mk

.DEFAULT_GOAL := All

All: $(NAME).nes $(NAME)b.nes 


# i really dont know how to correctly define all these rules
$(NAME)b.o: $(NAME)b.s
	$(CA65) $(NAME)b.s

$(NAME)b.s: $(NAME)b.c
	$(CC65) -Oi $(NAME)b.c --add-source

$(NAME)b.nes: $(NAME)b.o reset.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME)b.nes reset.o $(NAME)b.o nes.lib

clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) $(NAME)b.s
	$(RM) *.o