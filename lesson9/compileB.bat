@echo off

set name="lesson9b"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source
ca65 resetB.s
ca65 %name%.s
ca65 asm4c.s

ld65 -C nes.cfg -o %name%.nes resetB.o %name%.o asm4c.o nes.lib

del *.o

pause

%name%.nes
