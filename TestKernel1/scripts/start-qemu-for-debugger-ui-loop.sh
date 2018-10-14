#!/bin/sh

while true; do
	(nc -l -p 4435) && (qemu-system-i386 -curses -s -S -kernel /home/user/projects/TestKernel1/kernel.elf -append lalala)
done
