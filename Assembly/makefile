output = out.hex
temporary = temp.preprocessed
target = ROM.asm
assemble: customasm.exe
	cpp -x assembler-with-cpp -nostdinc -CC -undef -P $(target) > $(temporary)
	./customasm.exe $(temporary) -p
	rm -f $(temporary)

sim: customasm.exe
	cpp -x assembler-with-cpp -nostdinc -CC -undef -P $(target) > $(temporary)
	./customasm.exe $(temporary) -f logisim8 -p
	rm -f $(temporary)

file: customasm.exe
	cpp -x assembler-with-cpp -nostdinc -CC -undef -P $(target) > $(temporary)
	./customasm.exe $(temporary) -f logisim8 -o $(temporary)
	# start at line 2 (remove first line with text "v2.0 raw"). This workaround may need to be removed in the future
	tail -n +2 $(temporary) > $(output)
	rm -f $(temporary)

rom: customasm.exe
	cpp -x assembler-with-cpp -nostdinc -CC -undef -P $(target) > $(temporary)
	./customasm.exe $(temporary) -o $(output)
	rm -f $(temporary)
