#!/bin/sh
umount /mnt
mount -t cifs -o domain=DESKTOP-09N87DV,user=LocalUser,dir_mode=0777,file_mode=077 \\\\192.168.44.1\\tftp  /mnt
