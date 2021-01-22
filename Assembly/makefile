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
	./customasm.exe $(temporary) -f deccomma -o $(temporary)
	./GEN_LW.exe $(temporary)
	rm -f $(temporary)

rom: customasm.exe
	cpp -x assembler-with-cpp -nostdinc -CC -undef -P $(target) > $(temporary)
	./customasm.exe $(temporary) -o $(output)
	rm -f $(temporary)
