; This is a simple assembly microbenchmark to show the effects of value prediction
; It stores a value in memory, then accesses it a few times to train the value predictor
; Then it flushes the cache line and performs a bunch of no-ops to clear the pipeline
; Then it reloads the value from memory and performs some arithmetic operations with it
; Without value prediction, the processor has to stall to wait for these values before arithmetic operations can be performed
; With value prediction, the processor can predict the values and continue executing instructions without stalling

section .data
    my_int dd 100               ; Define a 32-bit integer with the initial value 42

section .text
    global _start

_start:
    ; Store a value in my_int
    mov eax, 100               ; Load the value to be stored in EAX
    mov [my_int], eax          ; Store the value into memory (my_int) so it goes to the l1 cache
    mov ecx, 15

    ;noops x50 to give the 
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop


; train the value predictor
_loop:
    dec ecx
    ; some noop padding because of branch prediction
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ; load value in
    mov eax, [my_int]          ; Load the value from my_int into EAX
    ; Do some addition with the value loaded from memory
    
    add ebx, eax
    imul ebx, eax



    ; if ecx is exactly 8, we want to load a new value into my_int
    cmp ecx, 8                  ; Compare ecx to 8
    jne _cont                   ; If ecx != 8, jump to _loop_cont
    mov eax, 200                ; Load the new value to be stored in EAX
    mov [my_int], eax           ; Store the new value into memory (my_int)

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

_cont:
    ; if ecx is greater than 1, jump back to _loop
    cmp ecx, 1                  ; Compare ecx to 1
    jg _loop                    ; If ecx > 1, jump back to _loop


    cmp ecx, 0                  ; Compare ecx to 0
    jz _end_loop                    ; If ecx == 0, jump back to _loop
    
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    ; Flush the cache line containing my_int
    clflush [my_int]              ; Flush the cache line containing my_int

    ; 185 no ops without a loop to make sure the pipeline is cleared
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    JMP _loop                       

_end_loop:

    ; Exit the program (for Linux)
    mov eax, 1                 ; sys_exit
    xor ebx, ebx               ; exit code 0
    int 0x80
