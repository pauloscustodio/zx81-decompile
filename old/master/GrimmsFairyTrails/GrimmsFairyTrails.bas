#ASM

D_FILE             equ $400C
DF_CC              equ $400E


        defb $76, $76

        ; Start of unknown area $4084 to $4087
        defb $00, $00, $00, $00
        ; End of unknown area $4084 to $4087

;--------------------------------------------------------------------------------
; input is BC with col-row, A with char to print
; output is HL with screen address and D with previous char
;--------------------------------------------------------------------------------

PUT_CHAR_AT:
        ld h, $00
        ld l, c
        sla l
        sla l
        sla l
        sla l
        sla l
        rl h
        ld e, c
        ld d, $00
        add hl, de              ; HL=C*33
        ld e, b
        add hl, de              ; HL=C*33+B
        ld de, (D_FILE)         ; get display file
        add hl, de
        ld de, $0001            ; add one
        add hl, de
        ld d, (hl)              ; read previous char
        ld (hl), a              ; write char
        ret


        ; Start of unknown area $40A9 to $40DF
        defb $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $40A9 to $40DF

;--------------------------------------------------------------------------------
; Fill 22 center lines of screen with character in BACKGROUND+1
;--------------------------------------------------------------------------------

FILL_SCR:
        ld c, $16               ; C=22 lines
        ld hl, (D_FILE)
        inc hl
        ld de, $0021            ; size of row

fill_row:
        ld b, $20               ; B=32 chars
        adc hl, de              ; move to next row
        ld (fill_line_addr), hl

BACKGROUND:
        ld (hl), $80
        inc hl
        dec b
        jp nz, BACKGROUND
        ld hl, (fill_line_addr)
        dec c
        jp nz, fill_row
        ret


fill_line_addr:
        defw $63BD

        ; Start of unknown area $4101 to $4101
        defb $00
        ; End of unknown area $4101 to $4101

;--------------------------------------------------------------------------------
; Fill a rectangle of dots before the maze walls are drawn
;--------------------------------------------------------------------------------

DRAW_DOTS:
        ld b, $1E               ; B=30 column 30
        ld c, $0F               ; C=15 row 15

draw_next_dot:
        ld a, $1B               ; A=dot
        call PUT_CHAR_AT        ; put a dot
        dec c
        ld a, c
        cp $01
        jr nz, draw_next_dot
        ld c, $0F               ; C=row 15
        dec b
        jr nz, draw_next_dot
        ret


        ; Start of unknown area $4117 to $4117
        defb $00
        ; End of unknown area $4117 to $4117

;--------------------------------------------------------------------------------
; Draw walls
;--------------------------------------------------------------------------------

DRAW_WALLS1:
        ld c, $0A               ; C=row 10
        ld b, $1D               ; B=column 29

wall_next_dot1:
        ld a, $80               ; char=solid block
        call PUT_CHAR_AT        ; put a char
        dec c
        ld a, c
        cp $06
        jr nz, wall_next_dot1   ; repeat until row 6
        ld c, $0A               ; C=row 10
        dec b
        dec b                   ; C=column -= 2
        ld a, b
        cp $0F                  ; repeat until column 15
        jr nz, wall_next_dot1
        ld c, $0A               ; C=row 10

wall_next_dot2:
        ld b, $0E               ; B=column 14

wall_next_dot3:
        ld a, $80               ; char=solid block
        call PUT_CHAR_AT        ; put a char
        dec c
        ld a, c
        cp $06
        jr nz, wall_next_dot3   ; repeat until row 6
        ld c, $0A               ; C=row 10
        dec b
        dec b
        ld a, b
        cp $00
        jr nz, wall_next_dot3
        ld b, $10
        ld c, $0A

wall_next_dot4:
        ld a, $80
        call PUT_CHAR_AT
        dec b
        ld a, b
        cp $0E
        jr nz, wall_next_dot4
        ld c, $07
        ld b, $10

wall_next_dot5:
        ld a, $80
        call PUT_CHAR_AT
        dec b
        ld a, b
        cp $0E
        jr nz, wall_next_dot5
        ret


        ; Start of unknown area $4167 to $416D
        defb $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $4167 to $416D


DRAW_WALLS2:
        ld b, $1D

code2:
        ld c, $0C

wall_next_dot6:
        ld a, $80
        call PUT_CHAR_AT
        dec b
        call PUT_CHAR_AT
        dec b
        dec b
        ld a, b

code5:
        cp $02
        jr nz, wall_next_dot6

code3:
        ld b, $1D

code4:
        ld c, $03

wall_next_dot7:
        ld a, $80
        call PUT_CHAR_AT
        dec b
        call PUT_CHAR_AT
        dec b
        dec b
        ld a, b

code6:
        cp $02
        jr nz, wall_next_dot7
        ret


        ; Start of unknown area $4197 to $41A1
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $02, $1E
        ; End of unknown area $4197 to $41A1


var4:
        defb $10

var5:
        defb $09

        ; Start of unknown area $41A4 to $41AB
        defb $02, $1E, $00, $00, $09, $18, $20, $62
        ; End of unknown area $41A4 to $41AB


var3:
        defb $01

        ; Start of unknown area $41AD to $41AD
        defb $01
        ; End of unknown area $41AD to $41AD


var2:
        defb $00

        ; Start of unknown area $41AF to $41AF
        defb $00
        ; End of unknown area $41AF to $41AF


var1:
        defb $00

        ; Start of unknown area $41B1 to $41D3
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00
        ; End of unknown area $41B1 to $41D3


L_41D4:
        ld hl, $41A1
        ld b, (hl)
        ld hl, $41A0
        ld c, (hl)
        ld a, $34
        call PUT_CHAR_AT
        ret


        ; Start of unknown area $41E2 to $41E5
        defb $00, $00, $00, $00
        ; End of unknown area $41E2 to $41E5


L_41E6:
        call L_41D4
        inc hl
        ld a, (hl)
        cp $80
        ret z
        cp $1B
        call z, L_4290
        ld de, var1
        ld a, (de)
        cp $C8
        ret z
        ld a, $00
        call PUT_CHAR_AT
        inc b
        ld a, $34
        call PUT_CHAR_AT
        ld a, b
        ld ($41A1), a
        ret


        ; Start of unknown area $420A to $420D
        defb $00, $00, $00, $00
        ; End of unknown area $420A to $420D


L_420E:
        call L_41D4
        dec hl
        ld a, (hl)
        cp $80
        ret z
        cp $1B
        call z, L_4290
        ld de, var1
        ld a, (de)
        cp $C8
        ret z
        ld a, $00
        call PUT_CHAR_AT
        dec b
        ld a, $34
        call PUT_CHAR_AT
        ld a, b
        ld ($41A1), a
        ret


        ; Start of unknown area $4232 to $4235
        defb $00, $00, $00, $00
        ; End of unknown area $4232 to $4235


L_4236:
        call L_41D4
        ld de, $0021
        sbc hl, de
        ld a, (hl)
        cp $80
        ret z
        cp $1B
        call z, L_4290
        ld de, var1
        ld a, (de)
        cp $C8
        ret z
        ld a, $00
        call PUT_CHAR_AT
        dec c
        ld a, $34
        call PUT_CHAR_AT
        ld a, c
        ld ($41A0), a
        ret


        ; Start of unknown area $425E to $4262
        defb $00, $00
        defb $00, $00, $00
        ; End of unknown area $425E to $4262


L_4263:
        call L_41D4
        ld de, $0021
        adc hl, de
        ld a, (hl)
        cp $80
        ret z
        cp $1B
        call z, L_4290
        ld de, var1
        ld a, (de)
        cp $C8
        ret z
        ld a, $00
        call PUT_CHAR_AT
        inc c
        ld a, $34
        call PUT_CHAR_AT
        ld a, c
        ld ($41A0), a
        ret


        ; Start of unknown area $428B to $428F
        defb $00, $00, $00, $00, $00
        ; End of unknown area $428B to $428F


L_4290:
        ld a, (var1)
        add a, $01
        ld (var1), a
        ret


        ; Start of unknown area $4299 to $432F
        defb $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $CD, $F9, $43, $CD
        defb $30, $43, $3A, $AE, $41, $FE, $3D, $C8, $C3, $1C, $43, $00, $00, $00, $00, $00
        ; End of unknown area $4299 to $432F


L_4330:
        ld a, ($41A0)
        ld hl, $41A4
        sub (hl)
        jp m, L_4352
        ld d, $02

L_433C:
        ld b, a
        ld a, ($41A1)
        ld hl, $41A5
        sub (hl)
        jp m, L_4359
        ld e, $02

L_4349:
        ld c, a
        ld a, c
        cp b
        jp m, L_4360
        jp p, L_4369

L_4352:
        ld d, $01
        neg
        jp L_433C


L_4359:
        ld e, $01
        neg
        jp L_4349


L_4360:
        ld a, d
        cp $01
        jp z, L_4388
        jp nz, L_4372

L_4369:
        ld a, e
        cp $01
        jp z, L_439E
        jp nz, L_43B4

L_4372:
        call L_4477
        cp $80
        ret nz
        call L_44C0
        cp $80
        ret nz
        call L_44E8
        cp $80
        ret nz
        call L_449A
        ret


L_4388:
        call L_449A
        cp $80
        ret nz
        call L_44C0
        cp $80
        ret nz
        call L_44E8
        cp $80
        ret nz
        call L_4477
        ret


L_439E:
        call L_44E8
        cp $80
        ret nz
        call L_449A
        cp $80
        ret nz
        call L_4477
        cp $80
        ret nz
        call L_44C0
        ret


L_43B4:
        call L_44C0
        cp $80
        ret nz
        call L_4477
        cp $80
        ret nz
        call L_449A
        cp $80
        ret nz
        call L_44E8
        ret


        ; Start of unknown area $43CA to $43F8
        defb $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $23
        ; End of unknown area $43CA to $43F8


L_43F9:
        ld a, ($4025)
        nop
        ld a, ($4026)
        ld b, a
        ld a, ($4025)
        cp c
        jp nz, L_440D
        ld a, ($4026)
        cp b
        ret z

L_440D:
        ld a, ($4026)
        ld b, a
        ld a, ($4025)
        ld c, a
        cp $FF
        ret z
        ld e, $FF
        ld d, $00
        ld a, b
        rrca
        ld b, a
        ld a, e
        sbc a, a
        or $26
        ld l, $05
        sub l

L_4426:
        add a, l
        scf
        ld e, a
        ld a, c
        rra
        ld c, a
        ld a, e
        jr c, L_4426
        ld c, b
        dec l
        ld l, $01
        jr nz, L_4426
        ld hl, $007D
        ld e, a
        add hl, de
        ld a, (hl)
        ld ($43F8), a
        cp $21
        call z, L_420E
        cp $22
        call z, L_4263
        cp $23
        call z, L_4236
        cp $24
        call z, L_41E6
        ret


        ; Start of unknown area $4453 to $445B
        defb $F9, $43, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $4453 to $445B


L_445C:
        ld hl, $41A5
        ld b, (hl)
        ld hl, $41A4
        ld c, (hl)
        ld a, $08
        call PUT_CHAR_AT
        ret


        ; Start of unknown area $446A to $4476
        defb $7A, $44, $C9, $A2, $44, $C9
        defb $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $446A to $4476


L_4477:
        call L_445C
        ld de, $0021
        adc hl, de
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        inc c
        ld a, $08
        call PUT_CHAR_AT
        ld a, c
        ld ($41A4), a
        ret


        ; Start of unknown area $4499 to $4499
        defb $00
        ; End of unknown area $4499 to $4499


L_449A:
        call L_445C
        ld de, $0021
        sbc hl, de
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        dec c
        ld a, $08
        call PUT_CHAR_AT
        ld a, c
        ld ($41A4), a
        ret


        ; Start of unknown area $44BC to $44BF
        defb $41, $C9, $9F, $44
        ; End of unknown area $44BC to $44BF


L_44C0:
        call L_445C
        inc hl
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        inc b
        ld a, $08
        call PUT_CHAR_AT
        ld a, b
        ld ($41A5), a
        ret


        ; Start of unknown area $44DE to $44E7
        defb $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $44DE to $44E7


L_44E8:
        call L_445C
        dec hl
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        dec b
        ld a, $08
        call PUT_CHAR_AT
        ld a, b
        ld ($41A5), a
        ret


        ; Start of unknown area $4506 to $4523
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00
        ; End of unknown area $4506 to $4523


L_4524:
        ld hl, $41A9
        ld b, (hl)
        ld hl, $41A8
        ld c, (hl)
        ld a, $08
        call PUT_CHAR_AT
        ret


        ; Start of unknown area $4532 to $4532
        defb $88
        ; End of unknown area $4532 to $4532


L_4533:
        call L_4524
        ld de, $0021
        adc hl, de
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        inc c
        ld a, $08
        call PUT_CHAR_AT
        ld a, c
        ld ($41A8), a
        ret


        ; Start of unknown area $4555 to $4555
        defb $CD
        ; End of unknown area $4555 to $4555


L_4556:
        call L_4524
        ld de, $0021
        sbc hl, de
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        dec c
        ld a, $08
        call PUT_CHAR_AT
        ld a, c
        ld ($41A8), a
        ret


        ; Start of unknown area $4578 to $4587
        defb $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $4578 to $4587


L_4588:
        ld a, (var3)
        cp $02
        jp z, L_4596
        ld a, $3D
        ld (var2), a
        ret


L_4596:
        call L_477C
        ret


        ; Start of unknown area $459A to $459B
        defb $00, $00
        ; End of unknown area $459A to $459B


L_459C:
        call L_4524
        inc hl
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        inc b
        ld a, $08
        call PUT_CHAR_AT
        ld a, b
        ld ($41A9), a
        ret


L_45BA:
        call L_4524
        dec hl
        ld a, (hl)
        bit 7, a
        ret nz
        cp $34
        call z, L_4588
        cp $08
        ret z
        call PUT_CHAR_AT
        dec b
        ld a, $08
        call PUT_CHAR_AT
        ld a, b
        ld ($41A9), a
        ret


        ; Start of unknown area $45D8 to $464F
        defb $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $45D8 to $464F


L_4650:
        call L_43F9
        ld a, ($41AD)
        cp $02
        jp nz, L_466F
        ld a, $01
        ld ($41AD), a
        call L_4330
        call L_46B4
        ld a, (var2)
        cp $3D
        ret z
        jp LEVEL_DELAY


L_466F:
        add a, $01
        ld ($41AD), a

LEVEL_DELAY:
        ld c, $05               ; POKEd from basic 5 for fastest, 30 for slowest

L_4676:
        ld b, $FA

L_4678:
        ld a, $01
        dec b
        jr nz, L_4678
        dec c
        jr nz, L_4676
        ld a, (var1)
        cp $FA
        ret z
        ld hl, ($41AA)
        ld b, (hl)
        ld a, b
        cp $34
        call z, L_474A
        jp L_4650


        ; Start of unknown area $4693 to $46B3
        defb $50, $46, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00
        ; End of unknown area $4693 to $46B3


L_46B4:
        ld a, ($41A0)
        ld hl, $41A8
        sub (hl)
        jp m, L_46D6
        ld d, $02

L_46C0:
        ld b, a
        ld a, ($41A1)
        ld hl, $41A9
        sub (hl)
        jp m, L_46DD
        ld e, $02

L_46CD:
        ld c, a
        ld a, c
        cp b
        jp m, L_46E4
        jp p, L_46ED

L_46D6:
        ld d, $01
        neg
        jp L_46C0


L_46DD:
        ld e, $01
        neg
        jp L_46CD


L_46E4:
        ld a, d
        cp $01
        jp z, L_470C
        jp nz, L_46F6

L_46ED:
        ld a, e
        cp $01
        jp z, L_4715
        jp nz, L_471E

L_46F6:
        call L_4533
        cp $80
        ret nz
        call L_45BA
        cp $80
        ret nz
        call L_459C
        cp $80
        ret nz
        call L_4556
        ret


L_470C:
        call L_4556
        cp $80
        ret nz
        nop
        nop
        nop

L_4715:
        call L_45BA
        cp $80
        ret nz
        jp L_46F6


L_471E:
        call L_459C
        cp $80
        ret nz
        jp L_46F6


        ; Start of unknown area $4727 to $4749
        defb $00, $00, $00, $00, $00, $21, $A2, $41, $46
        defb $21, $A3, $41, $4E, $3E, $97, $CD, $88, $40, $22, $AA, $41, $C9, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $4727 to $4749


L_474A:
        ld a, (var3)
        add a, $01
        ld (var3), a
        ld b, $10
        ld c, $09
        ld a, $00
        call PUT_CHAR_AT
        ld a, $10
        ld ($41A1), a
        ld a, $0F
        ld ($41A0), a
        ld a, $34
        call L_41D4
        ret


        ; Start of unknown area $476B to $477B
        defb $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $476B to $477B


L_477C:
        dec a
        ld (var3), a
        ld a, $10
        ld ($41A1), a
        ld a, $0F
        ld ($41A0), a
        ld a, $34
        push bc
        push de
        push hl
        call L_41D4
        pop hl
        pop de
        pop bc
        ld a, $00
        ret


        ; Start of unknown area $4798 to $47A8
        defb $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00
        ; End of unknown area $4798 to $47A8


BANNER_CHAR:
        defb $00

BANNER_COL:
        defb $07

        ; Start of unknown area $47AB to $47AD
        defb $00, $00, $00
        ; End of unknown area $47AB to $47AD


banner_bits_lu:
        defb $00, $87, $04, $83, $02, $85, $06, $81, $01, $86, $05, $82, $03, $84, $07, $80

        ; Start of unknown area $47BE to $47C1
        defb $00, $00
        defb $00, $00
        ; End of unknown area $47BE to $47C1

;--------------------------------------------------------------------------------
; Display one character in big format
;--------------------------------------------------------------------------------

DISPLAY_BANNER:
        ld hl, (D_FILE)         ; address of screen
        inc hl

SCR_OFFSET:
        ld bc, $01F1            ; input offset POKEd from BASIC
        ld a, (BANNER_COL)
        cp $01
        jp nz, skip_setting_df_cc
                                ; skip if column not 1
        adc hl, bc              ; for column 1 add input offset
        ld (DF_CC), hl

skip_setting_df_cc:
        ld hl, BANNER_CHAR      ; HL points at char to print
        ld a, (hl)              ; A has char to print
        and a
        rla
        rla                     ; multiply by 4
        ret c                   ; exit if bit 6 was 1
        rla                     ; multiply by 8
        ld d, $00
        rl d                    ; carry into D
        ld e, a                 ; DE=char*8
        ld hl, $1E00            ; base of CHARS table
        add hl, de              ; HL=address of char bitmap
        ld c, $04               ; 4 rows

banner_row:
        ld b, $04               ; 4 columns
        ld d, (hl)              ; get bitmap of two rows into DE
        inc hl
        ld e, (hl)
        inc hl
        push hl                 ; save char table pointer

banner_col:
        xor a                   ; do some magic
        rl d
        rla
        rl d
        rla
        rl e
        rla
        rl e
        rla
        ld hl, banner_bits_lu   ; lookup table of bitmaps
        add a, l
        ld l, a                 ; HL points to correct bitmap
        ld a, (hl)              ; A has the bitmap
        ld hl, (DF_CC)          ; HL has the print position
        ld (hl), a              ; print the character
        inc hl                  ; advance print position
        ld (DF_CC), hl          ; and store it
        djnz banner_col         ; next column
        push de                 ; save bitmap
        ld de, $001D            ; offset to next row
        add hl, de              ; add to DF_CC
        ld (DF_CC), hl          ; and store
        pop de                  ; restore bitmap
        pop hl                  ; restore chars pointer
        dec c
        jr nz, banner_row       ; next row
        ld de, $FF80            ; offset to next character
        ld hl, (DF_CC)          ; add to DF_CC
        add hl, de
        ld (DF_CC), hl          ; and store it
        ld a, (hl)              ; read screen char; Note: not needed
        ret


        ; Start of unknown area $4827 to $50D1
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        defb $00, $76
        ; End of unknown area $4827 to $50D1



#ENDASM

#INCREMENT=1
#AUTOSTART=1
#FAST=0

  990  SLOW 
  995  POKE &BACKGROUND+1,8
 1000  LET C1= USR &FILL_SCR
 1002  POKE &var1,0
 1004  POKE &var2,0
 
 1006  GOSUB @INTRO
# draw maze dots
 1010  LET C1= USR &DRAW_DOTS
# clear paths in lower line and inside home
 1011  PRINT  AT 15,1;"                              "
 1012  PRINT  AT 8,15;" "
 1014  PRINT  AT 8,16;" "
 1016  PRINT  AT 9,16;" "
 1018  PRINT  AT 9,15;" "
# draw center walls
 1020  LET C1= USR &DRAW_WALLS1
 1022  POKE &DRAW_WALLS2+1,30
 1024  POKE &code2+1,14
 1026  POKE &code3+1,30
 1028  POKE &code4+1,5
 1030  POKE &code5+1,0
 1032  POKE &code6+1,0
 1034  LET C1= USR &DRAW_WALLS2
 1036  POKE &DRAW_WALLS2+1,29
 1038  POKE &code2+1,12
 1040  POKE &code3+1,29
 1041  POKE &var3,1
 1042  POKE &code4+1,3
 1044  POKE &code5+1,2
 1046  POKE &code6+1,2
 1048  LET C1= USR &DRAW_WALLS2
 1049  PRINT  AT 8,14;"."
 1050  PRINT  AT 3,1;".\::"
 1051  PRINT  AT 14,30;"."
 1052  PRINT  AT 12,1;".\::"
 1053  PRINT  AT 5,30;"."
 1056  PRINT  AT 18,5;"%S%C%O%R%E"
 1057  POKE &var4,16
 1058  POKE &var5,9
 1059  LET C1= USR 18220
 1060  POKE 16801,16
 1062  POKE 16800,15
 1064  PRINT  AT 15,16;"O"
 2020  PRINT  AT 6,16;"\@@"
 2022  PRINT  AT 6,15;"\@@"
 2024  POKE 16804,6
 2028  POKE 16805,16
 2030  POKE 16808,6
 2032  POKE 16809,15
 2035  POKE 16813,1
 2055  LET C1= USR 18000
 2070  PRINT  AT 18,11; PEEK 16816
 3000  PRINT  AT 20,3;"GO-AGAIN - PRINCE ? Y/N"
 3010  IF  INKEY$ ="Y" THEN  GOTO 3050
 3015  IF  INKEY$ ="N" THEN  GOTO 1
 3020  IF  INKEY$ ="" THEN  GOTO 3000
 3025  GOTO 3000
 3030  STOP 
 3050  LET C1= USR &FILL_SCR
 3060  POKE 16814,0
 3062  POKE 16816,0
 3064  GOTO 1010

@INTRO:
 4000  REM 
 4005  GOSUB @SHOW_BANNER
 4007  GOSUB @SHOW_STORY
 4010  PRINT  AT 2,1;"USING THE ROYAL ARROW KEYS"
 4012  PRINT  AT 3,1;"YOU MUST GUIDE PRINCE BILLY"
 4014  PRINT  AT 4,1;"THROUGH THE MAZE AND GET"
 4016  PRINT  AT 5,1;"ALL THE B-LERS"
 4017  PRINT  AT 7,1;"GOOD LUCK"
 4019  PRINT  AT 12,2;"WHAT LEVEL OF PLAY?"
 4020  PRINT  AT 14,2;"1=SUPER PRINCE"
 4030  PRINT  AT 15,2;"2=SKILLED HUNTER"
 4032  PRINT  AT 16,2;"3=SCHOOL DAZE"
 4034  PRINT  AT 17,2;"4=FIRST TIME"
 4036  PRINT  AT 18,2;"5=LEAD FEET"
 4038  PRINT  AT 19,2;"6=POISON APPLE"
@WAIT_LEVEL:
 4040  IF  INKEY$ ="" THEN  GOTO @WAIT_LEVEL
 4080  LET Y= CODE INKEY$ - CODE "0"
 4082  IF Y=1 THEN  POKE &LEVEL_DELAY+1,5
 4083  IF Y=2 THEN  POKE &LEVEL_DELAY+1,10
 4084  IF Y=3 THEN  POKE &LEVEL_DELAY+1,15
 4085  IF Y=4 THEN  POKE &LEVEL_DELAY+1,20
 4086  IF Y=5 THEN  POKE &LEVEL_DELAY+1,25
 4087  IF Y=6 THEN  POKE &LEVEL_DELAY+1,30
 4088  IF Y<1 OR Y>6 THEN  GOTO @WAIT_LEVEL
 4090  LET C1= USR &FILL_SCR
 4099  RETURN 

@SHOW_BANNER:
 5000  REM 
 5002  DIM A$(6,7)
 5004  LET A$(1)=" TIMEX "
 5006  LET A$(2)="*******"
 5008  LET A$(3)=" TIMEX "
 5010  LET A$(4)="GRIMM S"
 5012  LET A$(5)=" FAIRY "
 5014  LET A$(6)="TRAILS "
 5016  FOR J=1 TO 6
 5018  GOSUB @SET_START_OFFSET
 5020  FOR N=1 TO 7
 5022  LET B$=A$(J,N TO N)
 5024  POKE &BANNER_CHAR, CODE B$
 5026  POKE &BANNER_COL,N
 5028  LET C1= USR &DISPLAY_BANNER
 5030  NEXT N
 5032  FOR L=1 TO 3
 5034  NEXT L
 5036  IF J=3 THEN  LET C1= USR &FILL_SCR
 5037  IF J=4 THEN  PRINT  AT 5,24;"\.'"
 5038  NEXT J
 5040  REM \f5\c1\1e\1c\1a\1d\19\0b\35\37\2a\38\38\00\2a\33\39\2a\37\00\3c\2d\2a\33\00\37\2a\26\29\3e\0b
 5042  REM \ee\3e\0d
 5990  POKE &BACKGROUND+1,0
 5995  LET C1= USR &FILL_SCR
 5999  RETURN 

# set coordinates for start of banner text
@SET_START_OFFSET:
 6000  REM 
 6002  IF J=1 OR J=4 THEN  POKE &SCR_OFFSET+1,167
 6004  IF J=1 OR J=4 THEN  POKE &SCR_OFFSET+2,0
 6006  IF J=2 OR J=5 THEN  POKE &SCR_OFFSET+1,76
 6008  IF J=2 OR J=5 THEN  POKE &SCR_OFFSET+2,1
 6010  IF J=3 OR J=6 THEN  POKE &SCR_OFFSET+1,241
 6020  IF J=3 OR J=6 THEN  POKE &SCR_OFFSET+2,1
 6199  RETURN 

@SHOW_STORY:
 6200  REM 
 6202  PRINT  AT 1,0;"ONCE UPON A TIME, IN A FAR"
 6204  PRINT "AND DISTANT LAND THERE LIVED"
 6206  PRINT "THE FOLLOWING PEOPLE:"
 6208  PRINT 
 6210  PRINT "O  =  PRINCE BILLY"
 6212  PRINT "\@@  =  MAZE DWELLER MURPH"
 6213  PRINT 
 6214  PRINT "\@@  =  MAZE DWELLER DRAGO"
 6216  PRINT 
 6218  PRINT "THE MAZE DWELLERS WOULD CHASE "
 6220  PRINT "PRINCE BILLY DAY AND NIGHT"
 6222  PRINT 
 6224  PRINT "HIS ONLY HOPE WAS TO GATHER 250"
 6226  PRINT "B-LERS (...), HIS LIFE CRYSTALS."
 6228  PRINT 
 6230  PRINT "IF PRINCE BILLY COULD TOUCH"
 6232  PRINT "THE SACRED STONE OF ROSS  %*   "
 6234  PRINT "IN THE CENTER OF THE MAZE HE"
 6236  PRINT "CAN ESCAPE CAPTURE--ONLY ONCE"
 6385  PRINT  AT 21,1;"%P%R%E%S%S\::%E%N%T%E%R\::%W%H%E%N\::%R%E%A%D%Y"
 6387  INPUT Y$
 6390  POKE &BACKGROUND+1,128
 6392  LET C1= USR &FILL_SCR
 6399  RETURN 
 
# machine code loader in decimal
 9000  STOP 
 9010  INPUT A
 9020  INPUT B
 9030  PRINT B
 9040  POKE A,B
 9050  LET A=A+1
 9060  GOTO 9020

# machine code loader in hexadecimal 
 9070  INPUT X
 9080  LET A$=""
 9090  IF A$="" THEN  INPUT A$
 9100  IF A$="S" THEN  STOP 
 9110  POKE X,16* CODE A$+ CODE A$(2)-476
 9120  PRINT X;" ";A$
 9130  LET X=X+1
 9140  LET A$=A$(3 TO )
 9150  GOTO 9090
 
# save to tape using some ROM utility at $2000
 9200  PRINT "SET TAPE"
 9205  INPUT Y$
 9210  PRINT  USR 8192;"SAVE:P:GRIMM:"
 9212  CLS 
 9220  GOTO 1

#VARS CX=0
#VARS J=7,6,1,5017
#VARS N=8,7,1,5021
#VARS L=4,3,1,5033
#VARS Y=1
#VARS A$(6,7)=" TIMEX ","*******"," TIMEX ","GRIMM S"," FAIRY ","TRAILS "

#SYSVARS=\00\fa\23\f7\60\f8\60\10\64\11\64\86\64\e1\60\00\c0\86\64\86\64\20\5d\40\00\02\be\23\ff\ff\03\37\e4\60\c6\0f\00\28\50\6b\0c\46\ff\f1\d0\00\00\bc\21\18\40\00\1a\1c\2c\37\2e\32\32\1b\35\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\8e\e8\00\00\84\a0\00\00\00\80\80\80\80\80\80\80\89\96\90\00\00\00\00\00\00\00
#D_FILE=\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\76
#WORKSPACE=

