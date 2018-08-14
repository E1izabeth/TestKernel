[BITS 32]
[SECTION .text]
[EXTERN isr_handler]
[EXTERN irq_handler]

[GLOBAL irq_common_stub]
irq_common_stub:
   pusha                    ; Помещает в стек содержимое регистров edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               ; Младшие 16 битов регистра eax = ds.
   push eax                 ; Сохранение дескриптора сегмента данных
   push esp

   mov ax, 0x10  ; Загрузка сегмента данных ядра
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call irq_handler

   pop esp
   pop eax        ; Перезагрузка оригинального дескриптора сегмента данных
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   popa           ; Выталкиваем из стека значения edi,esi,ebp...
   add esp, 8     ; Очищаем из стека код ошибки, помещаем в стек номер ISR
   sti
   iret           ; Выталкиваем из стека следующие пять значений: CS, EIP, EFLAGS, SS и ESP 

[GLOBAL idt_flush]    
idt_flush:
   mov eax, [esp+4]  ; Берет указатель на IDT, передаваемый в качестве параметра.
   lidt [eax]        ; Загружает указатель IDT
   sti
   ret

[GLOBAL gdt_flush]    
gdt_flush:
   mov eax, [esp+4]  ; Берет указатель на GDT, передаваемый в качестве параметра.
   lgdt [eax]        ; Загружает указатель GDT
   mov ax, 0x10      ; 0x10 - смещение на сегмент данных
   mov ds, ax	       
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov cs, ax
   jmp 0x08:.flush

   .flush:
      ret
