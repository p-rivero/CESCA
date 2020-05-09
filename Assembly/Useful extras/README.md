# Assembly extras


## GEN_LW

If this file (or any file named "GEN_LW") is in the same folder as send_file.exe, the program will be converted to a RAM hex file that can be used in LogicWorks, instead of being sent to an Arduino programmer.
The contents of GEN_LW don't matter. The only thing checked is the name.

LogicWorks is an old circuit simulator and you probably aren't using it, so you can safely delete this file.

Otherwise you can move this file next to send_file.exe and use the generated file, which contains the RAM contents after the program has been loaded (select "Read data from a raw hex file" in the RAM Synthesizer).
The file contents depend greatly on the hardware of your RAM module, so you will need to modify the source code in logicworks.cc in order to make it fit your build.


## cesca_syntax.xml

This file provides custom syntax highlighting in Notepad++ for proramming in CESCA assembly. How to use:
 1. Open Notepad++ and go to Language -> User Defined Language -> Define your language
 2. Click "Import..." and select cesca_syntax.xml
 3. Restart Notepad++
 
Now the custom language "CESCA" is associated with .asm files in Notepad++. The coloring of your code should look similar to this:

![Syntax highlighting](https://github.com/p-rivero/CESCA/blob/master/Assembly/Useful%20extras/syntax%20example.png?raw=true)