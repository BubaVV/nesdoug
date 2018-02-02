CC65 = cc65
CA65 = ca65
LD65 = ld65
NAME = lesson1

#select correct delete command for Win or Unix
ifdef ComSpec
    RM=del
else
    RM=rm
endif 

$(NAME).nes: $(NAME).o reset.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o nes.lib
	$(RM) *.o

reset.o: reset.s
	$(CA65) reset.s

$(NAME).o: $(NAME).s
	$(CA65) $(NAME).s

$(NAME).s: $(NAME).c
	$(CC65) -Oi $(NAME).c --add-source

clean:
	$(RM) $(NAME).nes
	$(RM) $(NAME).s
