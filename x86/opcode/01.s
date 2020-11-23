global start
section .text
start:
  mov ebx, eax
  mov [esi], ecx
  mov [eax], ecx
  mov [edi + 8], edx
  mov [ebx + 12345678], ebp