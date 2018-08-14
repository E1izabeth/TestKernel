[BITS 32]
[SECTION .text]
[EXTERN isr_handler]
[EXTERN irq_handler]

[GLOBAL irq_common_stub]
irq_common_stub:
   pusha                    ; �������� � ���� ���������� ��������� edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               ; ������� 16 ����� �������� eax = ds.
   push eax                 ; ���������� ����������� �������� ������
   push esp

   mov ax, 0x10  ; �������� �������� ������ ����
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call irq_handler

   pop esp
   pop eax        ; ������������ ������������� ����������� �������� ������
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   popa           ; ����������� �� ����� �������� edi,esi,ebp...
   add esp, 8     ; ������� �� ����� ��� ������, �������� � ���� ����� ISR
   sti
   iret           ; ����������� �� ����� ��������� ���� ��������: CS, EIP, EFLAGS, SS � ESP 

[GLOBAL idt_flush]    
idt_flush:
   mov eax, [esp+4]  ; ����� ��������� �� IDT, ������������ � �������� ���������.
   lidt [eax]        ; ��������� ��������� IDT
   sti
   ret

[GLOBAL gdt_flush]    
gdt_flush:
   mov eax, [esp+4]  ; ����� ��������� �� GDT, ������������ � �������� ���������.
   lgdt [eax]        ; ��������� ��������� GDT
   mov ax, 0x10      ; 0x10 - �������� �� ������� ������
   mov ds, ax	       
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov cs, ax
   jmp 0x08:.flush

   .flush:
      ret
