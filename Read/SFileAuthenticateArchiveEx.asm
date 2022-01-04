_SFileAuthenticateArchiveEx proc near
  authresult      = Blizzard__Mopaq__AuthResult ptr -0Ch
  authresult_temp = Blizzard__Mopaq__AuthResult ptr  0Ch

55                       push    ebp
89 E5                    mov     ebp, esp
83 EC 38                 sub     esp, 38h
C7 44 24 18 FC 38 1E 01  mov     dword ptr [esp+18h], offset aArchive ; "ARCHIVE"
8B 45 1C                 mov     eax, [ebp+1Ch]
89 44 24 14              mov     [esp+14h], eax  ; exponent_length
8B 45 18                 mov     eax, [ebp+18h]
89 44 24 10              mov     [esp+10h], eax  ; exponent
8B 45 14                 mov     eax, [ebp+14h]
89 44 24 0C              mov     [esp+0Ch], eax  ; modulus_length
8B 45 10                 mov     eax, [ebp+10h]
89 44 24 08              mov     [esp+8], eax    ; modulus
8D 45 F4                 lea     eax, [ebp+authresult]
89 44 24 04              mov     [esp+4], eax    ; authresult_temp
8B 45 08                 mov     eax, [ebp+8]
89 04 24                 mov     [esp], eax      ; archive
E8 84 99 00 00           call    Blizzard::Mopaq::SFileAuthenticateArchiveEx
; *authresult = authresult_temp;
8B 4D F4                 mov     ecx, [ebp+authresult_temp]
8B 55 0C                 mov     edx, [ebp+authresult]
89 0A                    mov     [edx], ecx
; result = true;
BA 01 00 00 00           mov     edx, 1
; if (!Blizzard::Mopaq::SFileAuthenticateArchiveEx (...))
84 C0                    test    al, al
75 0F                    jnz     short return_now
E8 8E E4 FF FF           call    Blizzard::Mopaq::SFileGetLastError
89 04 24                 mov     [esp], eax
E8 86 D3 E5 FF           call    _SErrSetLastError
; result = false;
31 D2                    xor     edx, edx

                      return_now:
89 D0                    mov     eax, edx
C9                       leave
C3                       retn
_SFileAuthenticateArchiveEx endp
