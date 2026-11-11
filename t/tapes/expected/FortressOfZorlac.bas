#AUTOSTART=1
#AUTOSTART_LINE=3100
#SYSVARS=0,$B8,$0B,$E1,$58,$E2,$58,$FA,$5B,$FA,$5B,$37,$5C,$D2,$58,0,$C0,$37,$5C,$37,$5C,$0B,$5D,$40,0,2,$39,8,$FF,$FF,$FF,$1F,$D3,$58,$B8,$0B,0,$71,$E2,$8D,$0C,0,0,$17,$9F,0,0,$BC,$21,$18,$40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,$76,0,0,0,0,0,$82,$40,0,0,0,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$85,$92,$92,$93,$90

#VARS L=0
#VARS V=5
#VARS F=250
#VARS N=33,32,1,431
#VARS A=18358
#VARS W=34
#VARS S=0
#VARS R=200

    1  REM 
#ASM [$4082]
        defb    $00,$00,$80,$80,$00,$80,$80,$80,$00,$00,$80,$80,$00,$80,$00,$00,$00,$00,$00,$00,$80,$00,$80,$80,$80,$80,$80,$00,$80,$80,$00,$80,$00,$80,$80,$80,$80,$00,$00,$00,$00,$80,$80,$80,$80,$00,$80,$80,$00,$80,$80,$00,$00,$80,$80,$00,$00,$80,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$00,$00,$08,$08,$08,$08,$00,$08,$00,$08,$08,$08,$08,$08,$08,$08,$08,$00,$08,$08,$08,$08,$08,$08,$08,$08,$08,$00,$08,$08,$00,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$EE,$40,$17,$41,$13,$02,$10,$05,$29,$00,$00,$00,$00,$00 ; -
#ENDASM [$4126]

    2  REM 
#ASM [$412C]

L412C:
        ld      hl, $40BB
        ld      ($411A), hl
        ld      hl, $4082
        ld      ($4118), hl
        ld      a, $14
        ld      ($411E), a
        ld      a, $09
        ld      ($411F), a
        ld      a, $39
        ld      ($4120), a
        call    L4191
        call    L415B
        call    L415B
        dec     b
        dec     b
        dec     c
        dec     c
        ld      ($411C), bc
        jp      L418A

L415B:
        inc     b
        inc     c
        ld      ($411C), bc
        ld      ($4118), de
        ld      a, ($4120)
        sub     $08
        ld      ($4120), a
        ld      h, $00
        ld      l, a
        add     hl, de
        ld      ($411A), hl
        ld      a, ($411E)
        sub     $02
        ld      ($411E), a
        ld      a, ($411F)
        sub     $02
        ld      ($411F), a
        nop
        nop
        nop
        call    L4191

L418A:
        ret
#ENDASM [$418B]

    3  REM 
#ASM [$4191]

L4191:
        ld      de, ($411A)
        ld      a, (de)
        ld      hl, ($411A)
        dec     hl
        ld      b, $00
        push    af
        ld      a, ($4120)
        ld      c, a
        pop     af
        lddr
        ld      hl, ($4118)
        ld      (hl), a
        push    hl
        ld      bc, ($411C)
        call    L4212
        ex      de, hl
        pop     hl
        push    bc
        ld      a, ($411E)
        ld      bc, $0020

L41B9:
        ldi
        inc     bc
        dec     a
        cp      $00
        jr      z, L41C6
        ex      de, hl
        add     hl, bc
        ex      de, hl
        jr      L41B9

L41C6:
        ld      bc, ($411F)
        ld      b, $00
        ldir
        ld      a, ($411E)
        ld      bc, $0022

L41D4:
        ldi
        inc     bc
        dec     a
        cp      $00
        jr      z, L41E3
        ex      de, hl
        and     a
        sbc     hl, bc
        ex      de, hl
        jr      L41D4

L41E3:
        ld      a, ($411F)

L41E6:
        dec     de
        dec     de
        ldi
        dec     a
        cp      $00
        jr      nz, L41E6
        pop     bc
        ex      de, hl
        ret
        defb    $3E,$F7,$BC,$28,$0D,$3E,$EF,$BC,$28,$08,$3E,$DF,$BC,$28,$03,$C3,$F5,$43,$AF,$B9,$CA,$F5,$43,$C3,$EA,$43 ; -
#ENDASM [$420C]

    4  REM 
#ASM [$4212]

L4212:
        push    de
        push    af
        ld      hl, (D_FILE)
        inc     hl
        xor     a
        ld      de, $0021

L421C:
        cp      b
        jr      z, L4223
        add     hl, de
        inc     a
        jr      L421C

L4223:
        ld      a, c
        ld      e, $00

L4226:
        cp      e
        jr      z, L422D
        inc     e
        inc     hl
        jr      L4226

L422D:
        pop     af
        pop     de
        ret
        defb    $00,$00 ; -
#ENDASM [$4232]

    5  REM 
#ASM [$4238]

L4238:
        ld      hl, $4082
        ld      de, ($411C)
        push    de
        ld      a, b
        sub     d
        ld      d, $00
        ld      e, a
        add     hl, de
        pop     de
        ld      a, c
        sub     e
        cp      $01
        jr      z, L4252
        jr      nc, L425C
        ld      (hl), $00
        ret

L4252:
        ld      d, $00
        ld      a, $39
        nop
        ld      e, a
        add     hl, de
        ld      (hl), $00
        ret

L425C:
        ld      d, $00
        ld      a, $39
        nop
        add     a, a
        sub     $08
        ld      e, a
        add     hl, de
        ld      (hl), $00
        ret
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$426E]

    6  REM 
#ASM [$4274]
        defb    $16,$08,$00,$00,$00,$00,$00,$00,$87,$07,$80,$84,$04,$00,$84,$03,$07,$00,$00,$02,$80,$01,$00,$00,$07,$80,$84,$00,$00,$87,$80,$04,$00,$00,$81,$00,$82,$00,$00,$00,$00,$00,$00 ; -

L429F:
        ld      hl, ($411C)
        ld      a, l
        add     a, $03
        ld      ($4274), a
        ld      bc, ($4274)
        push    bc
        ld      a, ($42EF)
        cp      $00
        jr      z, L42C2
        dec     b
        call    L42D1
        pop     bc
        ld      a, b
        cp      $06
        ret     nz
        xor     a

L42BE:
        ld      ($42EF), a
        ret

L42C2:
        inc     b
        call    L42D1
        pop     bc
        ld      a, b
        cp      $0A
        ret     nz
        ld      a, $01
        jr      L42BE
        defb    $00,$00 ; -

L42D1:
        ld      ($4274), bc
        call    L4212
        ex      de, hl
        ld      hl, $4277
        ld      a, $08

L42DE:
        ld      bc, $0005
        ldir
        dec     a
        cp      $00
        ret     z
        ex      de, hl
        ld      bc, $001C
        add     hl, bc
        ex      de, hl
        jr      L42DE
        defb    $01,$00,$00,$00,$00 ; -
#ENDASM [$42F4]

    7  REM 
#ASM [$42FA]
        defb    $81,$80,$80,$80,$82,$82,$82,$81,$83,$80,$00,$80,$80,$80,$00,$07,$07,$84,$03,$80,$84,$80,$80,$80,$07,$81,$80,$80,$80,$82,$82,$81,$83,$82,$81,$00,$80,$80,$80,$00,$07,$84,$03,$07,$84,$84,$80,$80,$80,$07,$81,$80,$80,$80,$82,$80,$83,$82,$81,$81,$00,$80,$80,$80,$00,$80,$03,$07,$84,$84,$84,$80,$80,$80,$07,$03,$00,$09 ; -

L4348:
        ld      bc, ($4346)

L434C:
        call    L4212
        ex      de, hl
        ld      hl, $4345
        ld      a, $01
        cp      (hl)
        jr      nz, L4361
        inc     a
        ld      ($4345), a
        ld      hl, $42FA
        jr      L4377

L4361:
        ld      a, $02
        cp      (hl)
        jr      nz, L436F
        inc     a
        ld      ($4345), a
        ld      hl, $4313
        jr      L4377

L436F:
        ld      a, $01
        ld      ($4345), a
        ld      hl, $432C

L4377:
        ld      a, $05

L4379:
        ld      bc, $0005
        ldir
        dec     a
        cp      $00
        ret     z
        ex      de, hl
        ld      bc, $001C
        add     hl, bc
        ex      de, hl
        jr      L4379
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$438F]

    8  REM 
#ASM [$4395]
        defb    $CD,$BB,$02,$ED,$4B,$46,$43,$3E,$FF,$BD,$28,$54,$3D,$BD,$28,$2B,$3D,$BD,$28,$27,$3D,$3D,$BD,$28,$2D,$3E,$F7,$BD,$28,$28,$3E,$EF,$BD,$28,$38,$3E,$DF,$BD,$28,$33,$3E,$F7,$94,$30,$24 ; -

L43C2:
        ld      a, ($411C)
        sub     $08
        cp      c
        jr      z, L43F5
        call    L4405
        inc     c
        jr      L43F5

L43D0:
        ld      a, $10
        cp      b
        jr      z, L43F5
        call    L4405
        inc     b
        jr      L43F5

L43DB:
        ld      a, $03
        cp      b
        jr      z, L43F5
        call    L4405
        dec     b
        jr      L43F5

L43E6:
        xor     a
        cp      c
        jr      z, L43F5
        call    L4405
        dec     c
        jr      L43F5
        defb    $C5,$CD,$31,$44,$C1 ; -

L43F5:
        ld      ($4346), bc
        call    L4348
        ret
        defb    $00,$00 ; -
#ENDASM [$43FF]

    9  REM 
#ASM [$4405]

L4405:
        push    bc
        ld      bc, ($4346)
        call    L4212
        ld      a, $05
        ld      de, $001C

L4412:
        ld      b, $05

L4414:
        ld      (hl), $00
        inc     hl
        djnz    L4414
        add     hl, de
        dec     a
        cp      $00
        jr      nz, L4412
        pop     bc
        ret
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$4426]

   10  REM 
#ASM [$442C]
        defb    $00,$11,$12,$0A,$12 ; -

L4431:
        ld      a, ($442C)
        cp      $01
        ret     z
        ld      hl, ($4346)
        inc     h
        inc     h
        inc     l
        inc     l
        inc     l
        inc     l
        ld      ($442D), hl
        ld      ($442F), hl
        ld      b, h
        ld      c, l
        call    L4212
        ld      (hl), $34
        ld      a, $01
        ld      ($442C), a
        ret
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$4458]

   11  REM 
#ASM [$445E]
        defb    $00,$00 ; -

L4460:
        xor     a
        ld      ($445E), a
        ld      ($445F), a
        ld      a, ($442C)
        cp      $00
        ret     z
        ld      bc, ($442D)
        call    L4212
        ld      a, $34
        cp      (hl)
        jr      nz, L447B
        ld      (hl), $00

L447B:
        ld      de, ($411C)
        ld      a, e
        dec     a
        sub     c
        jr      z, L448F
        jr      c, L448F
        inc     hl

L4487:
        ld      (hl), $34
        inc     c
        ld      ($442D), bc
        ret

L448F:
        inc     hl
        ld      a, e
        add     a, $04
        sub     c
        jr      z, L44B6
        xor     a
        cp      (hl)
        jr      z, L4487
        ld      a, $80
        cp      (hl)
        jr      z, L44AB
        ld      a, $08
        cp      (hl)
        jr      z, L44AB
        ld      a, $01
        ld      ($445F), a
        jr      L44B6

L44AB:
        inc     c
        ld      (hl), $00
        call    L4238
        ld      a, $01
        ld      ($445E), a

L44B6:
        xor     a
        ld      ($442C), a
        ret
        defb    $00,$00,$00,$00,$00,$00,$00,$00,$00 ; -
#ENDASM [$44C4]

   12  REM 
#ASM [$44CA]
        defb    $00,$80,$80,$80,$00,$00,$00 ; -

L44D1:
        ld      bc, $0301
        call    L4212
        ex      de, hl
        ld      b, $0B

L44DA:
        ld      hl, $44CA
        ldi
        ldi
        ldi
        djnz    L44DA
        ld      a, $05

L44E7:
        ld      b, $03
        ld      hl, $44CA

L44EC:
        push    bc
        ld      bc, $0020
        ex      de, hl
        add     hl, bc
        pop     bc
        ex      de, hl
        ldi
        djnz    L44EC
        dec     a
        cp      $00
        jr      nz, L44E7
        ld      bc, $0020
        ex      de, hl
        add     hl, bc
        ex      de, hl
        ld      b, $0A

L4505:
        ld      hl, $44CA
        ld      a, (hl)
        ld      (de), a
        inc     hl
        dec     de
        ld      a, (hl)
        ld      (de), a
        inc     hl
        dec     de
        ld      a, (hl)
        ld      (de), a
        nop
        dec     de
        djnz    L4505
        ld      bc, $0020
        ex      de, hl
        and     a
        sbc     hl, bc
        ex      de, hl
        ld      a, $05

L4520:
        ld      b, $03
        ld      hl, $44CA

L4525:
        push    af
        ld      a, (hl)
        ld      (de), a
        pop     af
        push    bc
        ld      bc, $0021
        ex      de, hl
        and     a
        sbc     hl, bc
        pop     bc
        ex      de, hl
        inc     hl
        djnz    L4525
        dec     a
        cp      $00
        jr      nz, L4520
        ld      a, ($44CA)
        ld      ($44CD), a
        ld      a, ($44CB)
        ld      ($44CA), a
        ld      a, ($44CC)
        ld      ($44CB), a
        ld      a, ($44CD)
        ld      ($44CC), a
        ld      bc, $0B0D
        jp      L434C
#ENDASM [$4559]

   13  REM 
#ASM [$455F]
        defb    $00,$81,$84,$07,$00,$81,$86,$07,$82,$81,$82,$07,$00,$81,$84,$07,$84,$81,$86,$07,$00,$81,$82,$07,$00,$81,$80,$00,$07,$82,$80,$81,$80,$07,$80,$80,$81,$00,$07,$80,$80,$81,$80,$07,$80,$84,$81,$00,$80,$07,$00,$84,$07,$82,$00,$82,$81,$84,$06,$82,$07,$82,$00,$84,$81,$84,$06,$82,$07,$84,$00,$84,$81,$82,$00,$81,$80,$80,$80,$82,$82,$82,$82,$82,$80,$00,$80,$00,$80,$00,$80,$84,$84,$84,$84,$84,$80,$80,$80,$07,$04 ; -

L45C4:
        ld      de, $0019
        ld      a, ($45C3)
        inc     a
        cp      $05
        jr      nz, L45D1
        ld      a, $01

L45D1:
        ld      ($45C3), a
        ld      hl, $4546
        ld      b, a

L45D8:
        add     hl, de
        djnz    L45D8
        push    hl
        ld      bc, ($4346)
        call    L4212
        ex      de, hl
        pop     hl
        jp      L4377
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$45ED]

   14  REM 
#ASM [$45F3]
        defb    $00,$00,$00,$00,$00,$00,$00,$00,$34,$00,$00,$00,$00,$00,$00,$00,$00,$FA,$45 ; -

L4606:
        ld      hl, ($4604)
        dec     hl
        ld      bc, $45F3
        and     a
        sbc     hl, bc
        jr      nc, L4617
        ld      hl, $45FB
        jr      L4618

L4617:
        add     hl, bc

L4618:
        push    hl
        ld      ($4604), hl
        ld      bc, $0D12
        call    L4212
        pop     de
        ex      de, hl
        ld      bc, $0008
        ldir
        ret
#ENDASM [$462A]

   15  REM 
#ASM [$4630]

L4630:
        ld      a, ($411C)
        sub     $02
        ld      c, a
        ld      b, $05
        call    L4647
        nop
        call    L4647
        call    L4647
        nop
        call    L4647
        ret

L4647:
        call    L4212
        ld      (hl), $81
        ld      de, $0021
        add     hl, de
        ld      (hl), $02
        inc     b
        inc     b
        inc     b
        inc     b
        ret
        defb    $00,$00,$00 ; -
#ENDASM [$465A]

   16  REM 
#ASM [$4660]
        defb    $00,$11,$00 ; -

L4663:
        ld      a, ($4662)
        cp      $01
        ret     z
        ld      a, ($411C)
        sub     $03
        ld      l, a
        ld      a, ($4347)
        ld      h, a
        ld      a, $05
        sub     h
        jr      c, L467C
        ld      h, $05
        jr      L4690

L467C:
        ld      a, $09
        sub     h
        jr      c, L4685
        ld      h, $09
        jr      L4690

L4685:
        ld      a, $0D
        sub     h
        jr      c, L468E
        ld      h, $0D
        jr      L4690

L468E:
        ld      h, $11

L4690:
        ld      ($4660), hl
        ld      a, $01
        ld      ($4662), a
        ret
#ENDASM [$4699]

   17  REM 
#ASM [$469F]
        defb    $00 ; -

L46A0:
        xor     a
        ld      ($469F), a
        ld      bc, ($4660)
        call    L4212
        ld      (hl), $00
        xor     a
        cp      c
        jr      nz, L46B5
        ld      ($4662), a
        ret

L46B5:
        dec     c
        dec     hl
        cp      (hl)
        jr      z, L46BF
        ld      a, $34
        cp      (hl)
        jr      nz, L46C6

L46BF:
        ld      (hl), $04
        ld      ($4660), bc
        ret

L46C6:
        inc     (hl)
        ld      (L46C6), a
        ld      a, $01
        ld      ($469F), a
        ret
        defb    $00,$00,$00,$00,$00 ; -
#ENDASM [$46D5]

   18  REM 
#ASM [$46DB]
        defb    $61,$00 ; -

L46DD:
        ld      hl, ($46DB)
        inc     hl
        ld      de, $00C8
        and     a
        sbc     hl, de
        jr      z, L46EE
        add     hl, de
        ld      ($46DB), hl
        ret

L46EE:
        ld      hl, $0000
        ld      ($46DB), hl
        call    L47E1
        ld      bc, ($411C)
        dec     c
        ld      ($411C), bc
        dec     c
        dec     c
        call    L4212
        ld      a, $14

L4707:
        ld      bc, $000D
        ld      d, h
        ld      e, l
        inc     hl
        ldir
        dec     hl
        ld      (hl), $00
        ld      bc, $0014
        add     hl, bc
        dec     a
        cp      $00
        jr      nz, L4707
        ret
#ENDASM [$471C]

   19  REM 
#ASM [$4722]
        defb    $38,$04,$00,$1D,$1C,$24,$1C,$10,$27,$E8,$03,$64,$00,$0A,$00,$01,$00,$00,$00,$01 ; -

L4736:
        xor     a
        ld      ($4735), a
        ld      hl, ($4722)
        ld      ($4733), hl
        ld      bc, $4724
        ld      hl, $4729
        push    hl

L4747:
        xor     a
        pop     hl
        ld      e, (hl)
        inc     hl
        ld      d, (hl)
        dec     hl
        push    hl

L474E:
        ld      hl, ($4733)
        and     a
        sbc     hl, de
        ld      ($4733), hl
        jr      c, L475C
        inc     a
        jr      L474E

L475C:
        add     hl, de
        cp      $00
        jr      nz, L476A
        ld      a, ($4735)
        cp      $00
        ld      a, $00
        jr      z, L4773

L476A:
        add     a, $1C
        push    af
        ld      a, $01
        ld      ($4735), a
        pop     af

L4773:
        ld      ($4733), hl
        ld      (bc), a
        inc     bc
        pop     hl
        inc     hl
        inc     hl
        push    hl
        ld      a, e
        cp      $01
        jr      nz, L4747
        pop     hl
        ret
        defb    $00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; -
#ENDASM [$478D]

   20  REM 
#ASM [$4793]

L4793:
        ld      a, ($445F)
        cp      $01
        ret     z
        ld      a, ($445E)
        cp      $00
        ret     z
        xor     a
        ld      ($445E), a
        ld      de, $0003
        ld      hl, ($442F)
        ld      h, $00
        and     a
        sbc     hl, de
        ex      de, hl
        ld      hl, ($4722)
        add     hl, de
        add     hl, de
        add     hl, de
        add     hl, de
        add     hl, de
        ld      ($4722), hl
        call    L4736
        ld      hl, (D_FILE)
        ld      de, $0012
        add     hl, de
        ex      de, hl
        ld      hl, $4724
        ld      b, $05

L47CA:
        ld      a, (hl)
        cp      $00
        jr      z, L47D5
        ldi
        inc     bc
        djnz    L47CA
        ret

L47D5:
        inc     hl
        djnz    L47CA
        ret
        defb    $00,$00 ; -
#ENDASM [$47DB]

   21  REM 
#ASM [$47E1]

L47E1:
        ld      bc, ($442D)
        call    L4212
        ld      a, $34
        cp      (hl)
        ret     nz
        ld      (hl), $00
        ret

L47EF:
        ldi
        ld      h, b
        ld      b, (hl)
        call    L4212
        ld      (hl), $00
        ret
#ENDASM [$47F9]

   22  REM 
#ASM [$47FF]

L47FF:
        ld      b, $3A
        ld      hl, $4082

L4804:
        ld      (hl), $80
        inc     hl
        djnz    L4804
        ld      b, $32

L480B:
        ld      (hl), $08
        inc     hl
        djnz    L480B
        ld      b, $2A

L4812:
        ld      (hl), $80
        inc     hl
        djnz    L4812
        ld      hl, $0215
        ld      ($411C), hl
        ld      hl, $0000
        ld      ($46DB), hl
        ld      ($445E), hl
        ld      hl, $0900
        ld      ($4346), hl
        ld      hl, $0100
        ld      ($4660), hl
        xor     a
        ld      ($442C), a
        ld      ($4662), a
        ld      ($469F), a
        ret
        defb    $00,$00 ; -
#ENDASM [$483F]

   23  REM 
#ASM [$4845]
        defb    $2D,$2E,$39,$00,$26,$33,$3E,$00,$31,$2A,$39,$39,$2A,$37,$00,$3C,$2D,$2A,$33,$00,$37,$2A,$26,$29,$3E ; -

L485E:
        ld      bc, $1703
        call    L4212
        ex      de, hl
        ld      hl, $4845
        ld      bc, $0019
        ldir
        ret
        defb    $00,$00,$00 ; -

L4871:
        ld      bc, $1703
        call    L4212
        ld      b, $19

L4879:
        ld      (hl), $00
        inc     hl
        djnz    L4879
        ret
        defb    $00,$00,$00 ; -
#ENDASM [$4882]

   24  REM 
#ASM [$4888]
        defb    $00,$85,$01,$85,$00,$00,$02,$80,$03,$00,$00,$87,$80,$04,$00,$00,$81,$00,$82,$00,$00,$00,$00,$00,$00,$87,$07,$80,$84,$04,$00,$84,$03,$07,$00,$00,$02,$80,$01,$00,$00,$00,$00,$00,$00,$01 ; -

L48B6:
        ld      bc, ($411C)
        inc     c
        inc     c
        inc     c
        ld      b, $05
        call    L4212
        ld      de, $001C
        ld      a, $0E

L48C7:
        push    af
        ld      b, $05

L48CA:
        ld      a, (hl)
        xor     $80
        ld      (hl), a
        inc     hl
        djnz    L48CA
        add     hl, de
        pop     af
        dec     a
        cp      $00
        jr      nz, L48C7
        ret
        defb    $EB,$21,$9C,$48,$C3,$77,$43,$3E,$02,$BE,$20,$1F,$3C,$32,$B5,$48,$EB,$23,$36,$05,$23,$36,$02,$23,$36,$05,$11,$1F,$00,$19,$36,$03,$23,$23,$36,$01,$11,$A3,$00,$19,$36,$80,$C9,$3E,$01,$32,$B5,$48,$EB,$23,$23,$36,$00,$23,$36,$85,$11,$21,$00,$19,$36,$03,$11,$A5,$00,$19,$36,$80,$C9,$00,$00,$00 ; -
#ENDASM [$4921]

   25  REM 
#ASM [$4927]
        defb    $ED,$4B,$1C,$41,$0C,$0C,$0C,$06,$05,$ED,$43,$46,$43,$CD,$05,$44,$06,$0E,$ED,$43,$46,$43,$CD,$05,$44,$C9,$00,$00,$00,$00,$00,$00,$00,$00 ; -
#ENDASM [$4949]

   26  REM 
#ASM [$494F]
        defb    $01,$00 ; -

L4951:
        call    L46DD
        call    L49D1
        call    L429F
        call    L412C
        call    L4460
        call    L4793
        ld      a, ($445F)
        cp      $01
        ret     z
        call    L4630
        call    L46A0
        ld      a, ($469F)
        cp      $01
        ret     z
        call    L4663
        ld      hl, ($494F)
        ld      de, $0000

L497E:
        inc     de
        and     a
        sbc     hl, de
        jp      z, L4951
        add     hl, de
        jr      L497E
        defb    $00,$00,$00,$00,$00,$00,$00 ; -
#ENDASM [$498F]

   27  REM 
#ASM [$4995]
        defb    $3E,$BF,$BD,$28,$06,$3E,$7F,$BD,$C2,$F0,$43,$3A,$1C,$41,$C3,$C5,$43,$00,$00,$00,$00,$00,$00,$00 ; -
#ENDASM [$49AD]

   28  REM 
#ASM [$49B3]

L49B3:
        ld      bc, $0200
        call    L4212
        ld      a, $14

L49BB:
        ld      b, $20

L49BD:
        ld      (hl), $00
        inc     hl
        djnz    L49BD
        inc     hl
        dec     a
        cp      $00
        jr      nz, L49BB
        ret
#ENDASM [$49C9]

   29  REM 
#ASM [$49CF]
        defb    $DF,$FB ; -

L49D1:
        call    KEYBOARD
        ld      bc, ($4346)
        ld      ($49CF), hl
        ld      de, $FFFF
        and     a
        sbc     hl, de
        ret     z
        ld      hl, ($49CF)
        bit     4, l
        call    z, L4431
        ld      hl, ($49CF)
        ld      bc, ($4346)
        bit     5, l
        call    z, L4431
        ld      hl, ($49CF)
        ld      bc, ($4346)
        bit     3, l
        jp      z, L43DB
        bit     2, l
        jp      z, L43DB
        bit     0, l
        jp      z, L43D0
        bit     1, l
        jp      z, L43D0
        bit     6, l
        jr      z, L4A18
        bit     7, l
        ret     nz

L4A18:
        nop
        nop
        nop
        nop
        nop
        bit     5, h
        jp      z, L43E6
        bit     4, h
        jp      z, L43E6
        bit     3, h
        jp      z, L43E6
        bit     2, h
        jp      z, L43C2
        bit     1, h
        jp      z, L43C2
        ret
        defb    $00,$00,$00,$00,$00,$00 ; -
#ENDASM [$4A3D]


@Line___30:
   30  CLS 
   34  POKE 17924,252
   35  POKE 17925,69
   40  PRINT  AT 5,11;"WELCOME TO";
   50  PRINT  AT 7,5;"\.'\''\'. \''\:'\' \ '\':\'' \.'\''\'. \.'\''\'. \: \ .\' ";
   60  PRINT  AT 8,5;"\:.\..\.:  \:   \ :  \:.\..\.: \:    \:'\'.";
   70  PRINT  AT 9,5;"\:  \ :  \:   \ :  \:  \ : \'.\..\.' \:  \'.";\00
   80  PRINT  AT 17,3;"COPR., PAUL CARLSON, 1982";
   90  PRINT  AT 21,0;"DO YOU WANT INSTRUCTIONS? (Y/N)";
  100  POKE 17158,128
  110  POKE 17183,128
  120  POKE 17208,128

@Line__140:
  140  LET L= USR &L44D1
  150  LET L= USR &L4606
  155  IF  INKEY$ ="" THEN  GOTO @Line__140
  160  IF  INKEY$ ="N" THEN  GOTO @Line__190
  170  IF  INKEY$ <>"Y" THEN  GOTO @Line__140
  180  GOSUB @Line_1000

@Line__190:
  190  GOSUB @Line_2000
  195  LET W=28
  200  LET S=0
  202  LET R=200
  205  POKE 18146,R
  210  POKE 18210,0
  220  POKE 18211,0
  230  LET L= USR &L47FF
  250  PRINT  AT 0,10;"SCORE: 0"

@Line__260:
  260  LET W=W+1
  270  IF W=34 THEN  GOTO @Line__640
  280  POKE 17158,W
  290  POKE 17183,W
  300  POKE 17208,W

@Line__310:
  310  LET L= USR &L4630
  320  LET L= USR &L485E

@Line__340:
  340  LET L= USR &L412C
  350  LET L= USR &L429F
  360  LET L= USR &L4348
  370  IF  INKEY$ ="" THEN  GOTO @Line__340
  380  LET L= USR &L4871
  390  LET L= USR &L4951
  400  LET L= USR &L47E1
  410  LET L= USR &L47EF
  420  IF  PEEK 17503=1 THEN  GOTO @Line__520
  430  FOR N=1 TO 32
  440  LET L= USR &L45C4
  450  LET L=L+N
  460  NEXT N
  470  LET L= USR &L4405
  475  POKE 18016,0
  480  POKE 17452,0
  485  POKE 18018,0
  490  POKE 17222,0
  500  POKE 17223,9
  505  POKE 18079,0
  510  GOTO @Line__260

@Line__520:
  520  FOR N=1 TO 50
  540  LET L= USR &L48B6
  545  LET L=L+0
  547  LET L=L+0
  550  NEXT N
  560  LET S= PEEK 18210+256* PEEK 18211
  570  LET S=S+F*(( PEEK 17455)-3)
  575  PRINT  AT 0,17;S
  580  POKE 18211, INT (S/256)
  590  POKE 18210,(S-256* PEEK 18211)
  592  IF R>20 THEN  LET R=R-20
  595  POKE 18146,R
  600  LET L= USR &L49B3
  620  LET L= USR &L47FF
  630  GOTO @Line__310

@Line__640:
  640  LET L= USR &L49B3
  650  PRINT  AT 0,4;"FINAL";
  660  PRINT  AT 10,13;"\:'\''\''\''\''\':"
  670  PRINT  AT 11,13;"\: GAME\ :"
  680  PRINT  AT 12,13;"\: OVER\ :"
  690  PRINT  AT 13,13;"\:.\..\..\..\..\.:"
  700  PRINT  AT 16,6;"PLAY AGAIN? (Y OR N)"

@Line__720:
  720  IF  INKEY$ ="N" THEN  STOP 
  730  IF  INKEY$ ="Y" THEN  GOTO @Line__190
  740  GOTO @Line__720

@Line_1000:
 1000  CLS 
 1010  PRINT  AT 0,10;"YOUR GOAL:"
 1020  PRINT  AT 2,0;"TO SCORE AS MANY POINTS AS YOU   CAN BY DESTROYING THE ALIEN AND BLOCKS OF HIS FORTRESS."
 1030  PRINT  AT 6,0;"YOU HAVE FIVE SHIPS TO DESTROY   THE ALIEN AS MANY TIMES AS YOU  CAN.  WARNING - THE ALIEN GUNS  ARE INDESTRUCTIBLE."
 1040  PRINT  AT 11,0;"POINTS ARE AWARDED BASED ON      SKILL LEVEL AND THE DISTANCE    YOUR SHIP IS FROM THE LEFT EDGE OF THE SCREEN WHEN IT FIRES."
 1050  PRINT  AT 16,0;"DESTROYING THE ALIEN IS WORTH    FIFTY TIMES THE POINTS AWARDED  FOR DESTROYING A BLOCK OF HIS   FORTRESS."
 1060  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."

@Line_1070:
 1070  IF  INKEY$ ="" THEN  GOTO @Line_1070
 1080  CLS 
 1090  PRINT  AT 0,3;"YOUR SHIPS CONTROL PANEL:"
 1100  PRINT  AT 3,2;"\:'\''\''\''\''\''\''\''\''\''\''\':    \:'\''\''\''\''\''\''\''\''\''\''\':    \: 1 2 3 4 5 \ :    \: 6 7 8 9 0 \ :    \:  Q W E R T\ :    \:  Y U I O P\ :    \:.\..\..\..\..\..\..\..\..\..\..\.:    \:.\..\..\..\..\..\..\..\..\..\..\.:    MOVE SHIP UP    FIRE TORPEDO"\00
 1110  PRINT  AT 11,0;"\:'\''\''\''\''\''\''\''\''\''\':  \:'\''\''\''\''\''\''\': \:'\''\''\''\''\''\''\''\''\':\: A S D F G\ :  \:  H J K\ : \:  L ENTER\ :\:  Z X C V \ :  \: B N M \ : \: . SPACE \ :\:.\..\..\..\..\..\..\..\..\..\.:  \:.\..\..\..\..\..\..\.: \:.\..\..\..\..\..\..\..\..\.: MOVE SHIP     MOVE      MOVE      DOWN        SHIP      SHIP                  LEFT      RIGHT"
 1120  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."

@Line_1130:
 1130  IF  INKEY$ ="" THEN  GOTO @Line_1130
 1140  RETURN 

@Line_2000:
 2000  CLS 
 2010  PRINT  AT 2,1;"SKILL LEVELS:"
 2020  PRINT  AT 4,15;"1 = SLOW"
 2030  PRINT  AT 6,15;"2 = AVERAGE"
 2040  PRINT  AT 8,15;"3 = FAST"
 2050  PRINT  AT 10,15;"4 = VERY FAST"
 2060  PRINT  AT 12,15;"5 = SUICIDAL"
 2070  PRINT  AT 16,6;"PRESS SKILL LEVEL..."

@Line_2080:
 2080  IF  INKEY$ ="" THEN  GOTO @Line_2080
 2090  LET V= CODE  INKEY$ -28
 2100  IF V<1 OR V>5 THEN  GOTO @Line_2080
 2105  CLS 
 2110  POKE 18767,1
 2120  POKE 18768,5-V
 2130  LET F=50*V
 2150  FOR N=1 TO 5
 2160  LET A=18353+N
 2170  POKE A,0
 2180  IF N<=V THEN  POKE A,25
 2190  NEXT N
 2200  RETURN 
 3000  SAVE "ATTAC%K"
 3100  GOTO @Line___30

#: Line___30                =     30
#: Line__140                =    140
#: Line__190                =    190
#: Line__260                =    260
#: Line__310                =    310
#: Line__340                =    340
#: Line__520                =    520
#: Line__640                =    640
#: Line__720                =    720
#: Line_1000                =   1000
#: Line_1070                =   1070
#: Line_1130                =   1130
#: Line_2000                =   2000
#: Line_2080                =   2080

#: KEYBOARD                 = $02BB =   699
#: ERR_NO                   = $4000 = 16384
#: FLAGS                    = $4001 = 16385
#: ERR_SP                   = $4002 = 16386
#: RAMTOP                   = $4004 = 16388
#: MODE                     = $4006 = 16390
#: PPC                      = $4007 = 16391
#: VERSN                    = $4009 = 16393
#: E_PPC                    = $400A = 16394
#: D_FILE                   = $400C = 16396
#: DF_CC                    = $400E = 16398
#: VARS                     = $4010 = 16400
#: DEST                     = $4012 = 16402
#: E_LINE                   = $4014 = 16404
#: CH_ADD                   = $4016 = 16406
#: X_PTR                    = $4018 = 16408
#: STKBOT                   = $401A = 16410
#: STKEND                   = $401C = 16412
#: BREG                     = $401E = 16414
#: MEM                      = $401F = 16415
#: FREE1                    = $4021 = 16417
#: DF_SZ                    = $4022 = 16418
#: S_TOP                    = $4023 = 16419
#: LAST_K                   = $4025 = 16421
#: DEBOUNCE                 = $4027 = 16423
#: MARGIN                   = $4028 = 16424
#: NXTLIN                   = $4029 = 16425
#: OLDPPC                   = $402B = 16427
#: FLAGX                    = $402D = 16429
#: STRLEN                   = $402E = 16430
#: T_ADDR                   = $4030 = 16432
#: SEED                     = $4032 = 16434
#: FRAMES                   = $4034 = 16436
#: COORDS_X                 = $4036 = 16438
#: COORDS_Y                 = $4037 = 16439
#: PR_CC                    = $4038 = 16440
#: S_POSN_COL               = $4039 = 16441
#: S_POSN_ROW               = $403A = 16442
#: CDFLAG                   = $403B = 16443
#: PRBUFF                   = $403C = 16444
#: MEMBOT                   = $405D = 16477
#: FREE2                    = $407B = 16507
#: PROG                     = $407D = 16509
#: L412C                    = $412C = 16684
#: L415B                    = $415B = 16731
#: L418A                    = $418A = 16778
#: L4191                    = $4191 = 16785
#: L41B9                    = $41B9 = 16825
#: L41C6                    = $41C6 = 16838
#: L41D4                    = $41D4 = 16852
#: L41E3                    = $41E3 = 16867
#: L41E6                    = $41E6 = 16870
#: L4212                    = $4212 = 16914
#: L421C                    = $421C = 16924
#: L4223                    = $4223 = 16931
#: L4226                    = $4226 = 16934
#: L422D                    = $422D = 16941
#: L4238                    = $4238 = 16952
#: L4252                    = $4252 = 16978
#: L425C                    = $425C = 16988
#: L429F                    = $429F = 17055
#: L42BE                    = $42BE = 17086
#: L42C2                    = $42C2 = 17090
#: L42D1                    = $42D1 = 17105
#: L42DE                    = $42DE = 17118
#: L4348                    = $4348 = 17224
#: L434C                    = $434C = 17228
#: L4361                    = $4361 = 17249
#: L436F                    = $436F = 17263
#: L4377                    = $4377 = 17271
#: L4379                    = $4379 = 17273
#: L43C2                    = $43C2 = 17346
#: L43D0                    = $43D0 = 17360
#: L43DB                    = $43DB = 17371
#: L43E6                    = $43E6 = 17382
#: L43F5                    = $43F5 = 17397
#: L4405                    = $4405 = 17413
#: L4412                    = $4412 = 17426
#: L4414                    = $4414 = 17428
#: L4431                    = $4431 = 17457
#: L4460                    = $4460 = 17504
#: L447B                    = $447B = 17531
#: L4487                    = $4487 = 17543
#: L448F                    = $448F = 17551
#: L44AB                    = $44AB = 17579
#: L44B6                    = $44B6 = 17590
#: L44D1                    = $44D1 = 17617
#: L44DA                    = $44DA = 17626
#: L44E7                    = $44E7 = 17639
#: L44EC                    = $44EC = 17644
#: L4505                    = $4505 = 17669
#: L4520                    = $4520 = 17696
#: L4525                    = $4525 = 17701
#: L45C4                    = $45C4 = 17860
#: L45D1                    = $45D1 = 17873
#: L45D8                    = $45D8 = 17880
#: L4606                    = $4606 = 17926
#: L4617                    = $4617 = 17943
#: L4618                    = $4618 = 17944
#: L4630                    = $4630 = 17968
#: L4647                    = $4647 = 17991
#: L4663                    = $4663 = 18019
#: L467C                    = $467C = 18044
#: L4685                    = $4685 = 18053
#: L468E                    = $468E = 18062
#: L4690                    = $4690 = 18064
#: L46A0                    = $46A0 = 18080
#: L46B5                    = $46B5 = 18101
#: L46BF                    = $46BF = 18111
#: L46C6                    = $46C6 = 18118
#: L46DD                    = $46DD = 18141
#: L46EE                    = $46EE = 18158
#: L4707                    = $4707 = 18183
#: L4736                    = $4736 = 18230
#: L4747                    = $4747 = 18247
#: L474E                    = $474E = 18254
#: L475C                    = $475C = 18268
#: L476A                    = $476A = 18282
#: L4773                    = $4773 = 18291
#: L4793                    = $4793 = 18323
#: L47CA                    = $47CA = 18378
#: L47D5                    = $47D5 = 18389
#: L47E1                    = $47E1 = 18401
#: L47EF                    = $47EF = 18415
#: L47FF                    = $47FF = 18431
#: L4804                    = $4804 = 18436
#: L480B                    = $480B = 18443
#: L4812                    = $4812 = 18450
#: L485E                    = $485E = 18526
#: L4871                    = $4871 = 18545
#: L4879                    = $4879 = 18553
#: L48B6                    = $48B6 = 18614
#: L48C7                    = $48C7 = 18631
#: L48CA                    = $48CA = 18634
#: L4951                    = $4951 = 18769
#: L497E                    = $497E = 18814
#: L49B3                    = $49B3 = 18867
#: L49BB                    = $49BB = 18875
#: L49BD                    = $49BD = 18877
#: L49D1                    = $49D1 = 18897
#: L4A18                    = $4A18 = 18968

#: [$4000] $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#: [$4040] $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$bbb
#: [$4080] bb--------------------------------------------------------------
#: [$40C0] ----------------------------------------------------------------
#: [$4100] --------------------------------------bbbbbbCCCCCCCCCCCCCCCCCCCC
#: [$4140] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4180] CCCCCCCCCCCbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$41C0] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC--------------
#: [$4200] ------------bbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC--bbbbbbCCCCCCCC
#: [$4240] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----bbbbbb------------
#: [$4280] -------------------------------CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$42C0] CCCCCCCCCCCCCCC--CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----bbbbbb------
#: [$4300] ----------------------------------------------------------------
#: [$4340] --------CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4380] CCCCCCCCCC-----bbbbbb-------------------------------------------
#: [$43C0] --CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----CCCCCCCC--b
#: [$4400] bbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----bbbbbb-----CCCCCCCCCCCCCCC
#: [$4440] CCCCCCCCCCCCCCCCCCC-----bbbbbb--CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4480] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----
#: [$44C0] ----bbbbbb-------CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4500] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4540] CCCCCCCCCCCCCCCCCCCCCCCCCbbbbbb---------------------------------
#: [$4580] ----------------------------------------------------------------
#: [$45C0] ----CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC-----bbbbbb-------------
#: [$4600] ------CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbCCCCCCCCCCCCCCCC
#: [$4640] CCCCCCCCCCCCCCCCCCCCCCC---bbbbbb---CCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4680] CCCCCCCCCCCCCCCCCCCCCCCCCbbbbbb-CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$46C0] CCCCCCCCCCCCCCCC-----bbbbbb--CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4700] CCCCCCCCCCCCCCCCCCCCCCCCCCCCbbbbbb--------------------CCCCCCCCCC
#: [$4740] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4780] CCC----------bbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$47C0] CCCCCCCCCCCCCCCCCCCCCCCCC--bbbbbbCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbC
#: [$4800] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC--b
#: [$4840] bbbbb-------------------------CCCCCCCCCCCCCCCC---CCCCCCCCCCCCCC-
#: [$4880] --bbbbbb----------------------------------------------CCCCCCCCCC
#: [$48C0] CCCCCCCCCCCCCCCCCCCCCCCCC---------------------------------------
#: [$4900] ---------------------------------bbbbbb-------------------------
#: [$4940] ---------bbbbbb--CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4980] CCCCCCCC-------bbbbbb------------------------bbbbbbCCCCCCCCCCCCC
#: [$49C0] CCCCCCCCCbbbbbb--CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4A00] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC------bbb
#: [$4A40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4A80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4AC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4B00] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4B40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4B80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4BC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4C00] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4C40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4C80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4CC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4D00] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4D40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4D80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4DC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4E00] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4E40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4E80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4EC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4F00] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4F40] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4F80] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$4FC0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5000] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5040] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5080] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$50C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5100] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5140] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5180] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$51C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5200] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5240] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5280] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$52C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5300] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5340] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5380] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$53C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5400] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5440] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5480] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$54C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5500] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5540] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5580] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$55C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5600] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5640] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5680] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$56C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5700] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5740] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5780] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$57C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5800] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5840] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$5880] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
#: [$58C0] bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb###############################
#: [$5900] ################################################################
#: [$5940] ################################################################
#: [$5980] ################################################################
#: [$59C0] ################################################################
#: [$5A00] ################################################################
#: [$5A40] ################################################################
#: [$5A80] ################################################################
#: [$5AC0] ################################################################
#: [$5B00] ################################################################
#: [$5B40] ################################################################
#: [$5B80] ################################################################
#: [$5BC0] ##########################################################vvvvvv
#: [$5C00] vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

