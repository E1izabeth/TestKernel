MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM               equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
; multiboot-заголовок (nasm-синтаксис)

; [SECTION .multiboot_header]
_multiboot_header:
        align 4
        dd    MULTIBOOT_HEADER_MAGIC
        dd    MULTIBOOT_HEADER_FLAGS
        dd    CHECKSUM

[BITS 32]
[EXTERN kernel_main]
[GLOBAL _start]
[SECTION .text]
        align 16
_start:
	;mov esp, 0x200000-4
	mov esp, stack_end
	;push MULTIBOOT_HEADER_MAGIC
	;push MULTIBOOT_HEADER_FLAGS
	;push CHECKSUM
	;push MULTIBOOT_MEMORY_INFO
	push ebx
	cli
	call kernel_main


	
[SECTION .bss]
        align 16
stack_begin:
		resb 4096  ; Reserve 4 KiB stack space
stack_end:
        align 16
