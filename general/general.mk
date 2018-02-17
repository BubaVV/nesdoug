#general and mostly system-wide includes for Nesdoug tutorial
#path definitions
CC65 = cc65
CA65 = ca65
LD65 = ld65

#select correct delete command for Win or Unix
ifdef ComSpec
    RM=del
else
    RM=rm -f
endif 

#$(NAME) should be defined in your lesson Makefile
ifndef NAME
$(error You shouldn't run this makefile directly. Include it to your lesson makefile and define NAME equal to lesson name, e.g. lesson1)
endif

$(NAME).nes: $(NAME).o reset.o nes.cfg
	$(LD65) -C nes.cfg -o $(NAME).nes reset.o $(NAME).o nes.lib
#	Cleaning of *.o spoils lesson4 compiling. Better move it to clean target
#	$(RM) *.o

reset.o: reset.s
	$(CA65) reset.s

asm4c.o: asm4c.s
	$(CA65) asm4c.s

$(NAME).o: $(NAME).s
	$(CA65) $(NAME).s

$(NAME).s: $(NAME).c
	$(CC65) -Oi $(NAME).c --add-source

clean:
	$(RM) *.nes
	$(RM) $(NAME).s
	$(RM) *.o
