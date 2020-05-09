@echo off

Rem || If all your code is in a subfolder you can set a prefix to be added automatically (don't use quotation marks).
Rem || Otherwise, leave this line as: set PREFIX=
set PREFIX=Examples\

del "temp.assembled" 2>nul
echo ASSEMBLE AND SEND A PROGRAM
echo Place this file next to customasm.exe, send_file.exe and any required assembler files or folders.
echo Make sure the computer is in programming mode and the Arduino is ready.
echo.
IF [%PREFIX%] == [] GOTO ask_name
echo Using prefix: "%PREFIX%" this can be changed by editing AssembleAndSend.bat
echo.

:ask_name
set /p "file=Enter assembler file name(s): %PREFIX%"

customasm -o temp.assembled -f deccomma %PREFIX%%file%
send_file.exe temp.assembled
del "temp.assembled" 2>nul

pause