@echo off

set name="lesson1"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source
ca65 reset.s
ca65 %name%.s

ld65 -C nes.cfg -o %name%.nes reset.o %name%.o nes.lib

del *.o

pause

%name%.nes
