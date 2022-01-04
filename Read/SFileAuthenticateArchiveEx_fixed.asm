_SFileAuthenticateArchiveEx proc near
  authresult      = Blizzard__Mopaq__AuthResult ptr -0Ch

55                       push    ebp
89 E5                    mov     ebp, esp
83 EC 38                 sub     esp, 38h
; *authresult = authresult_temp;
B9 05 00 00 00           mov     ecx, 5
8B 55 0C                 mov     edx, [ebp+authresult]
89 0A                    mov     [edx], ecx
; result = true;
B8 01 00 00 00           mov     eax, 1
C9                       leave
C3                       retn
_SFileAuthenticateArchiveEx endp
