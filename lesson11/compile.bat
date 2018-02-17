@echo off

set name="lesson11"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source -g
ca65 reset.s
ca65 %name%.s -g
ca65 asm4c.s -g

ld65 -C nes.cfg -o %name%.nes reset.o %name%.o asm4c.o nes.lib -Ln labels.txt

del *.o

pause

fceux_symbols4.py lesson11

%name%.nes
