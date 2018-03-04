
dirs := .		\
		./cpu

#		./src/util	\
#		./src/dbg	\
#		./src/vm

define getTargets 
	$(foreach 				\
		cdir, 				\
		$(dirs), 			\
		$(patsubst                      \
			%$(1), 			\
			%$(1).o, 		\
			$(wildcard $(cdir)/*$(1))\
			) 			\
		)
endef

#c_objs := $(patsubst %.c,%.c.o,$(wildcard *.c))
#asm_objs := $(patsubst %.asm,%.asm.o,$(wildcard *.asm))

c_objs := $(call getTargets,.c)
asm_objs := $(call getTargets,.asm)

#vpath % . src src/mem
kernel.elf: $(asm_objs) $(c_objs)
	@echo '# Linking..'
	ld -m elf_i386 -Ttext 0x100000 -o kernel.elf $^
	# ld -m elf_i386 -T ./sections-layout -o kernel.elf $^
	@echo '# Finished.'
	chmod +x ./scripts/remount-tftp-share-to-mnt.sh 
	chmod +x ./scripts/start-qemu-for-debugger.sh 

%.c.o:
	@echo '# Compiling $@ ..'
	gcc -m32 -c -g3 -fno-stack-protector -ffreestanding -nostdinc -o $@ $(patsubst %.c.o,%.c,$@) -I./include
	@echo '#'

%.asm.o:
	@echo '# Compiling $@ ..'
	nasm -g -felf -o $@ $(patsubst %.asm.o,%.asm,$@)
	@echo '#'

clean: 
	@echo '# Clean.'
	rm -f $(c_objs) $(asm_objs)
	@echo '#'

kernel: kernel.elf clean
	cp -v -f kernel.elf /mnt/

rebuild: clean kernel
