#!/bin/sh
# qemu-system-i386 -curses -s -S -kernel /home/user/projects/TestKernel1/kernel.elf &

# (mkfifo ./pipeback) && (qemu-system-i386 -curses -s -S -kernel /home/user/projects/TestKernel1/kernel.elf < ./pipeback 2>&1 | nc -q 5 -r 127.0.0.1 4435 > ./pipeback) && (rm -f ./pipeback) &

echo go | nc -q 5 -r 127.0.0.1 4435 

# (nc -l -p 4435) && (qemu-system-i386 -curses -s -S -kernel /home/user/projects/TestKernel1/kernel.elf) &

