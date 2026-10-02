#ASM
KEYBOARD equ $02BB
D_FILE   equ $400C

SHOW_KEYS:
        call KEYBOARD           ; HL=key pressed

        ld de, (D_FILE)
        inc de

        ld b,8
h_digits:        
        rl h
        call print_digit
        djnz h_digits

        xor a
        ld (de), a
        inc de

        ld b,8
l_digits:        
        rl l
        call print_digit
        djnz l_digits

        jr SHOW_KEYS

print_digit:
        ld a,$1C                ; '0'
        jr nc, not_zero
        inc a                   ; '1' 
not_zero:
        ld (de),a               ; print digit
        inc de
        ret
#ENDASM

@AGAIN:
    LET L=USR &SHOW_KEYS
    goto @AGAIN

