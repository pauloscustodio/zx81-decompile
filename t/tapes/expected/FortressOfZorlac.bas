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

; --------------------------------------------------------------------------------
; Fortress drawing
; --------------------------------------------------------------------------------


FORTRESS1:
        defm    "  \::\:: \::\::\::  \::\:: \::      \:: \::\::\::\::\:: \::\:: \:: \::\::\::\::    \::\::\::\:: \::\:: \::\::  \::\::  "

SAVE_FORTRESS1:
        defm    "\::"

FORTRESS2:
        defm    "\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##  \##\##\##\## \## \##\##\##\##\##\##\##\## \##\##\##\##\##\##\##\##\## \##\## \::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::"

addr_fortress:
        defw    $40EE

addr_fortress_save:
        defw    $4117

fortress_col:
        defb    $13

fortress_row:
        defb    $02

fortress_rows:
        defb    $10

fortress_cols:
        defb    $05

fortress_blocks:
        defb    $29

DEAD01:
        defb    $00, $00, $00, $00, $00

#ENDASM [$4126]

    2  REM 
#ASM [$412C]

; --------------------------------------------------------------------------------
; Draw fortress at fortress_row/fortress_col
; --------------------------------------------------------------------------------


DRAW_FORTRESS:
        ld      hl, SAVE_FORTRESS1
        ld      (addr_fortress_save), hl
        ld      hl, FORTRESS1
        ld      (addr_fortress), hl
        ld      a, $14                  ; 20 rows
        ld      (fortress_rows), a
        ld      a, $09                  ; 9 columns
        ld      (fortress_cols), a
        ld      a, $39                  ; 57 blocks
        ld      (fortress_blocks), a
        call    rotate_outer_layer
        call    rotate_inner_layer
        call    rotate_inner_layer
        dec     b
        dec     b
        dec     c
        dec     c
        ld      (fortress_col), bc
        jp      return


rotate_inner_layer:
        inc     b
        inc     c
        ld      (fortress_col), bc
        ld      (addr_fortress), de
        ld      a, (fortress_blocks)
        sub     $08
        ld      (fortress_blocks), a
        ld      h, $00
        ld      l, a
        add     hl, de
        ld      (addr_fortress_save), hl
        ld      a, (fortress_rows)
        sub     $02
        ld      (fortress_rows), a
        ld      a, (fortress_cols)
        sub     $02
        ld      (fortress_cols), a
        nop
        nop
        nop
        call    rotate_outer_layer

return:
        ret


#ENDASM [$418B]

    3  REM 
#ASM [$4191]

rotate_outer_layer:
        ld      de, (addr_fortress_save); point to save block
        ld      a, (de)                 ; get block to be saved to A
        ld      hl, (addr_fortress_save); get address of save
        dec     hl                      ; minus 1=last block
        ld      b, $00                  ; BC will be number of blocks
        push    af                      ; save end block
        ld      a, (fortress_blocks)    ; get number of blocks
        ld      c, a                    ; to BC
        pop     af                      ; A is block to be saved from end
        lddr                            ; move all blocks 1 step
        ld      hl, (addr_fortress)     ; get first address
        ld      (hl), a                 ; and store saved char
        push    hl                      ; save address of fortress
        ld      bc, (fortress_col)      ; coords of fortress to BC
        call    SCR_POS                 ; screen address to HL
        ex      de, hl                  ; screen address to DE
        pop     hl                      ; HL=blocks address
        push    bc                      ; save coords
        ld      a, (fortress_rows)      ; A=number of rows
        ld      bc, $0020               ; distance between rows

rot_blocks_left_column:
        ldi                             ; copy one
        inc     bc                      ; restore BC
        dec     a                       ; count rows
        cp      $00
        jr      z, rot_block_next1      ; jump forward if end of rows
        ex      de, hl
        add     hl, bc                  ; move to next row
        ex      de, hl
        jr      rot_blocks_left_column


rot_block_next1:
        ld      bc, (fortress_cols)
        ld      b, $00                  ; BC = number of cols
        ldir                            ; copy bottom row
        ld      a, (fortress_rows)      ; A = number of rows
        ld      bc, $0022

rot_blocks_right_column:
        ldi                             ; copy block
        inc     bc                      ; restore BC
        dec     a                       ; count rows
        cp      $00
        jr      z, rot_block_next2      ; jump forward if finished
        ex      de, hl
        and     a
        sbc     hl, bc                  ; move to previous row
        ex      de, hl
        jr      rot_blocks_right_column


rot_block_next2:
        ld      a, (fortress_cols)      ; A = number of columns

rot_blocks_top_row:
        dec     de
        dec     de
        ldi                             ; copy one block
        dec     a
        cp      $00
        jr      nz, rot_blocks_top_row  ; jump back if not finished
        pop     bc
        ex      de, hl                  ; DE=blocks, HL=screen
        ret


DEAD02:
        defb    $3E, $F7, $BC, $28, $0D, $3E, $EF, $BC, $28, $08, $3E, $DF, $BC, $28, $03, $C3, $F5, $43, $AF, $B9, $CA, $F5, $43, $C3, $EA, $43

#ENDASM [$420C]

    4  REM 
#ASM [$4212]

; --------------------------------------------------------------------------------
; Compute screen position of row-col in BC to HL 
; Preserves all other registers
; --------------------------------------------------------------------------------


SCR_POS:
        push    de                      ; save registers
        push    af
        ld      hl, (D_FILE)
        inc     hl
        xor     a
        ld      de, $0021

SCR_POS_next_row:
        cp      b
        jr      z, SCR_POS_row_found
        add     hl, de
        inc     a
        jr      SCR_POS_next_row


SCR_POS_row_found:
        ld      a, c
        ld      e, $00

SCR_POS_next_col:
        cp      e
        jr      z, SCR_POS_col_found
        inc     e
        inc     hl
        jr      SCR_POS_next_col


SCR_POS_col_found:
        pop     af
        pop     de
        ret


DEAD03:
        defb    $00, $00

#ENDASM [$4232]

    5  REM 
#ASM [$4238]

erase_hit_block_from_fortress:
        ld      hl, FORTRESS1
        ld      de, (fortress_col)
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


DEAD04:
        defb    $00, $00, $00, $00, $00

#ENDASM [$426E]

    6  REM 
#ASM [$4274]

alien_col:
        defb    $16

alien_row:
        defb    $08

DEAD05:
        defb    $00

ALIEN:
        defm    "     "
        defm    "\ .\:'\::\':\. "
        defm    " \':\''\:' "
        defm    " \ '\::\'  "
        defm    " \:'\::\': "
        defm    " \ .\::\.  "
        defm    " \.: \:. "
        defm    "     "

; --------------------------------------------------------------------------------
; Draw alien at fortress_row/fortress_col
; --------------------------------------------------------------------------------


DRAW_ALIEN:
        ld      hl, (fortress_col)
        ld      a, l
        add     a, $03
        ld      (alien_col), a          ; fortress column+3 to alien column
        ld      bc, (alien_col)
        push    bc
        ld      a, (alien_direction)
        cp      $00
        jr      z, alien_downwards
        dec     b
        call    draw_alien_at_BC
        pop     bc
        ld      a, b
        cp      $06
        ret     nz
        xor     a

store_alien_pos:
        ld      (alien_direction), a
        ret


alien_downwards:
        inc     b
        call    draw_alien_at_BC
        pop     bc
        ld      a, b
        cp      $0A
        ret     nz
        ld      a, $01
        jr      store_alien_pos


DEAD06:
        defb    $00, $00

draw_alien_at_BC:
        ld      (alien_col), bc
        call    SCR_POS
        ex      de, hl                  ; DE=screen address
        ld      hl, ALIEN               ; source of alien
        ld      a, $08                  ; 8 rows

alien_each_row:
        ld      bc, $0005               ; 4 cols
        ldir                            ; copy one row
        dec     a
        cp      $00
        ret     z                       ; exit afyet 8 rows
        ex      de, hl
        ld      bc, $001C               ; advance to next row
        add     hl, bc
        ex      de, hl
        jr      alien_each_row


alien_direction:
        defb    $01

DEAD07:
        defb    $00, $00, $00, $00

#ENDASM [$42F4]

    7  REM 
#ASM [$42FA]

SHIP1:
        defm    "\.:\::\::\::\:."
        defm    "\:.\:.\.:\..\::"
        defm    " \::"

SHIP1_nr:
        defm    "\::"
        defm    "\:: "
        defm    "\:'\:'\':\''\::"
        defm    "\':\::\::\::\:'"

SHIP2:
        defm    "\.:\::\::\::\:."
        defm    "\:.\.:\..\:.\.:"
        defm    " \::"

SHIP2_nr:
        defm    "\::"
        defm    "\:: "
        defm    "\:'\':\''\:'\':"
        defm    "\':\::\::\::\:'"

SHIP3:
        defm    "\.:\::\::\::\:."
        defm    "\::\..\:.\.:\.:"
        defm    " \::"

SHIP3_nr:
        defm    "\::"
        defm    "\:: "
        defm    "\::\''\:'\':\':"
        defm    "\':\::\::\::\:'"

which_ship:
        defb    $03

ship_col:
        defb    $00

ship_row:
        defb    $09

; --------------------------------------------------------------------------------
; Draw ship at row-col ship_row/ship_col
; --------------------------------------------------------------------------------


DRAW_SHIP:
        ld      bc, (ship_col)

; --------------------------------------------------------------------------------
; Draw ship at row-col BC
; --------------------------------------------------------------------------------


DRAW_SHIP_BC:
        call    SCR_POS                 ; HL=screen position                                    
        ex      de, hl                  ; DE=screen position
        ld      hl, which_ship
        ld      a, $01
        cp      (hl)
        jr      nz, not_ship1
        inc     a
        ld      (which_ship), a
        ld      hl, SHIP1
        jr      draw_ship_hl


not_ship1:
        ld      a, $02
        cp      (hl)
        jr      nz, not_ship2
        inc     a
        ld      (which_ship), a
        ld      hl, SHIP2
        jr      draw_ship_hl


not_ship2:
        ld      a, $01
        ld      (which_ship), a
        ld      hl, SHIP3

; DE is screen address, HL is ship


draw_ship_hl:
        ld      a, $05                  ; 5 rows

draw_ship_row:
        ld      bc, $0005               ; 5 columns
        ldir                            ; copy one row
        dec     a
        cp      $00
        ret     z                       ; return when 5 rows are printed
        ex      de, hl
        ld      bc, $001C               ; DE+=33-5=28
        add     hl, bc
        ex      de, hl
        jr      draw_ship_row


DEAD08:
        defb    $00, $00, $00, $00, $00

#ENDASM [$438F]

    8  REM 
#ASM [$4395]

DEAD09:
        defb    $CD, $BB, $02, $ED, $4B, $46, $43, $3E, $FF, $BD, $28, $54, $3D, $BD, $28, $2B, $3D, $BD, $28, $27, $3D, $3D, $BD, $28, $2D, $3E, $F7, $BD, $28, $28, $3E, $EF, $BD, $28, $38, $3E, $DF, $BD, $28, $33, $3E, $F7, $94, $30, $24

; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_RIGHT:
        ld      a, (fortress_col)       ; A=fortress column
        sub     $08                     ; -8 (5 for ship + 2 for cannons + 1)
        cp      c
        jr      z, save_ship_pos        ; reached limit
        call    DELETE_SHIP
        inc     c                       ; move right
        jr      save_ship_pos


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_DOWN:
        ld      a, $10
        cp      b
        jr      z, save_ship_pos        ; reached limit
        call    DELETE_SHIP
        inc     b                       ; move down
        jr      save_ship_pos


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_UP:
        ld      a, $03
        cp      b
        jr      z, save_ship_pos        ; reached limit
        call    DELETE_SHIP
        dec     b                       ; move up
        jr      save_ship_pos


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_LEFT:
        xor     a
        cp      c
        jr      z, save_ship_pos        ; reached limit
        call    DELETE_SHIP
        dec     c                       ; move right
        jr      save_ship_pos


DEAD10:
        defb    $C5, $CD, $31, $44, $C1

save_ship_pos:
        ld      (ship_col), bc
        call    DRAW_SHIP
        ret


DEAD11:
        defb    $00, $00

#ENDASM [$43FF]

    9  REM 
#ASM [$4405]

; --------------------------------------------------------------------------------
; Delete ship before moving it
; --------------------------------------------------------------------------------


DELETE_SHIP:
        push    bc                      ; save coords
        ld      bc, (ship_col)
        call    SCR_POS                 ; HL=screen address
        ld      a, $05                  ; 5 rows
        ld      de, $001C               ; distance in screen between rows

delete_row:
        ld      b, $05                  ; 5 cols

delete_col:
        ld      (hl), $00               ; delete char
        inc     hl
        djnz    delete_col
        add     hl, de                  ; next row
        dec     a
        cp      $00
        jr      nz, delete_row
        pop     bc                      ; restore coords
        ret


DEAD12:
        defb    $00, $00, $00, $00, $00

#ENDASM [$4426]

   10  REM 
#ASM [$442C]

torpedo_running:
        defb    $00

torpedo_col:
        defb    $11

torpedo_row:
        defb    $12

topedo_pos0:
        defw    $120A

; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


FIRE_TORPEDO:
        ld      a, (torpedo_running)    ; is a torpedo on the way?
        cp      $01
        ret     z                       ; return if yes
        ld      hl, (ship_col)
        inc     h
        inc     h
        inc     l
        inc     l
        inc     l
        inc     l
        ld      (torpedo_col), hl       ; torpedo coord is mouth of ship
        ld      (topedo_pos0), hl       ; make a copy of the cooords - why?
        ld      b, h
        ld      c, l
        call    SCR_POS                 ; HL=screen position
        ld      (hl), $34               ; draw torpedo
        ld      a, $01
        ld      (torpedo_running), a    ; signal torpedo on the way
        ret


DEAD13:
        defb    $00, $00, $00, $00, $00

#ENDASM [$4458]

   11  REM 
#ASM [$445E]

WALL_HIT:
        defb    $00

ALIEN_HIT:
        defb    $00

; --------------------------------------------------------------------------------
; moves torpedo if it has been created
; --------------------------------------------------------------------------------


MOVE_TORPEDO:
        xor     a
        ld      (WALL_HIT), a
        ld      (ALIEN_HIT), a
        ld      a, (torpedo_running)
        cp      $00
        ret     z                       ; torpedo was not fired, return
        ld      bc, (torpedo_col)
        call    SCR_POS                 ; HL=torpedo coord
        ld      a, $34
        cp      (hl)                    ; is there a torpedo?
        jr      nz, topedo_erased       ; jump forward if not
        ld      (hl), $00               ; erase it

topedo_erased:
        ld      de, (fortress_col)
        ld      a, e
        dec     a
        sub     c
        jr      z, torpedo_hit          ; torpedo col == fortress col -1
        jr      c, torpedo_hit          ; torpedo col >  fortress col -1
        inc     hl                      ; move torpedo

draw_torpedo_store_pos:
        ld      (hl), $34               ; draw it
        inc     c
        ld      (torpedo_col), bc       ; store new coords
        ret


torpedo_hit:
        inc     hl
        ld      a, e
        add     a, $04                  ; A=fortress-col+4 (alien)
        sub     c
        jr      z, stop_torpedo         ; inside alien - stop torpedo
        xor     a
        cp      (hl)                    ; hit space - continue
        jr      z, draw_torpedo_store_pos
        ld      a, $80
        cp      (hl)                    ; hit black border
        jr      z, hit_fortress
        ld      a, $08
        cp      (hl)                    ; hit gray border
        jr      z, hit_fortress
        ld      a, $01
        ld      (ALIEN_HIT), a
        jr      stop_torpedo


hit_fortress:
        inc     c
        ld      (hl), $00               ; erase block
        call    erase_hit_block_from_fortress
        ld      a, $01
        ld      (WALL_HIT), a

stop_torpedo:
        xor     a
        ld      (torpedo_running), a
        ret


DEAD14:
        defb    $00, $00, $00, $00, $00, $00, $00, $00, $00

#ENDASM [$44C4]

   12  REM 
#ASM [$44CA]

BLOCK1:
        defm    " \::\::"               ; WBB block

save_block1:
        defm    "\::"

DEAD20:
        defb    $00, $00, $00

; --------------------------------------------------------------------------------
; Draw rotating border and ship
; --------------------------------------------------------------------------------


INTRO_SCREEN:
        ld      bc, $0301               ; row 3, col 1
        call    SCR_POS                 ; HL=screen address
        ex      de, hl                  ; DE=screen address
        ld      b, $0B                  ; 10 times+1 for one LDI decrementing B

draw_top_bar:
        ld      hl, BLOCK1
        ldi                             ; copy block to screen
        ldi                             ; three times
        ldi
        djnz    draw_top_bar            ; 30 chars copied (LDI decs BC, DJNZ decs B)
        ld      a, $05                  ; copy 5 blocks of 3 chars

draw_right_bar:
        ld      b, $03
        ld      hl, BLOCK1

each_right_bar_block:
        push    bc                      ; save BC
        ld      bc, $0020               ; add 32 to DE, LDI with add 1 more
        ex      de, hl
        add     hl, bc                  ; HL+=32
        pop     bc
        ex      de, hl                  ; DE+=32
        ldi                             ; DE+=33
        djnz    each_right_bar_block    ; copy 3 blocks
        dec     a                       ; times 5
        cp      $00                     ; NOTE: not needed
        jr      nz, draw_right_bar
        ld      bc, $0020               ; add 32 to DE
        ex      de, hl
        add     hl, bc
        ex      de, hl                  ; DE+=32
        ld      b, $0A                  ; 10 blocks of 3 = 30

draw_bottom_bar:
        ld      hl, BLOCK1
        ld      a, (hl)                 ; copy 3 chars
        ld      (de), a
        inc     hl
        dec     de
        ld      a, (hl)
        ld      (de), a
        inc     hl
        dec     de
        ld      a, (hl)
        ld      (de), a
        nop                             ; NOTE: not needed
        dec     de
        djnz    draw_bottom_bar         ; repeat 10 times
        ld      bc, $0020               ; subtract 32 from DE
        ex      de, hl
        and     a
        sbc     hl, bc                  ; DE-=32
        ex      de, hl
        ld      a, $05                  ; copy 5 blocks of 3 chars

draw_left_bar:
        ld      b, $03
        ld      hl, BLOCK1

each_left_bar_block:
        push    af                      ; copy one char
        ld      a, (hl)
        ld      (de), a
        pop     af
        push    bc
        ld      bc, $0021               ; subtract 33 from address
        ex      de, hl
        and     a
        sbc     hl, bc                  ; DE-=33
        pop     bc
        ex      de, hl
        inc     hl
        djnz    each_left_bar_block     ; copy 3 blocks
        dec     a
        cp      $00
        jr      nz, draw_left_bar       ; times 5

; rotate block

        ld      a, (BLOCK1)
        ld      (save_block1), a
        ld      a, (BLOCK1+1)
        ld      (BLOCK1), a
        ld      a, (BLOCK1+2)
        ld      (BLOCK1+1), a
        ld      a, (save_block1)
        ld      (BLOCK1+2), a
        ld      bc, $0B0D               ; line 11, column 13
        jp      DRAW_SHIP_BC


#ENDASM [$4559]

   13  REM 
#ASM [$455F]

ROTATED_SHIP1:
        defm    " \.:\':\:' "
        defm    "\.:\'.\:'\:.\.:"
        defm    "\:.\:' \.:\':"
        defm    "\:'\':\.:\'.\:'"
        defm    " \.:\:.\:' "

ROTATED_SHIP2:
        defm    "\.:\:: \:'\:."
        defm    "\::\.:\::\:'\::"
        defm    "\::\.: \:'\::"
        defm    "\::\.:\::\:'\::"
        defm    "\':\.: \::\:'"

ROTATED_SHIP3:
        defm    " \':\:'\:. "
        defm    "\:.\.:\':\.'\:."
        defm    "\:'\:. \':\.:"
        defm    "\':\.'\:.\:'\':"
        defm    " \':\.:\:. "

ROTATED_SHIP4:
        defm    "\.:\::\::\::\:."
        defm    "\:.\:.\:.\:.\::"
        defm    " \:: \:: "
        defm    "\::\':\':\':\':"
        defm    "\':\::\::\::\:'"

rotate_count:
        defb    $04

ROTATE_SHIP:
        ld      de, $0019               ; 5*5=25 bytes per ship
        ld      a, (rotate_count)       ; counts 1 to 4
        inc     a
        cp      $05
        jr      nz, store_count
        ld      a, $01

store_count:
        ld      (rotate_count), a
        ld      hl, $4546               ; address of ship1-25 bytes
        ld      b, a

add_25:
        add     hl, de
        djnz    add_25
        push    hl                      ; save source address of ship
        ld      bc, (ship_col)          ; get ship position
        call    SCR_POS                 ; HL = screen position
        ex      de, hl                  ; DE = screen position
        pop     hl                      ; HL = source of rotated ship
        jp      draw_ship_hl


DEAD15:
        defb    $00, $00, $00, $00, $00

#ENDASM [$45ED]

   14  REM 
#ASM [$45F3]

torpedo_path:
        defm    "        O        "

intro_torpedo_pos_lo:
        defb    $FA

intro_torpedo_pos_hi:
        defb    $45

; --------------------------------------------------------------------------------
; Fire intro torpedo
; --------------------------------------------------------------------------------


INTRO_FIRE_TORPEDO:
        ld      hl, (intro_torpedo_pos_lo)
        dec     hl                      ; move back one pos
        ld      bc, torpedo_path
        and     a
        sbc     hl, bc
        jr      nc, restore_torpedo_pos
        ld      hl, $45FB               ; reached start, go back to end
        jr      draw_torpedo_path


restore_torpedo_pos:
        add     hl, bc

draw_torpedo_path:
        push    hl
        ld      (intro_torpedo_pos_lo), hl
        ld      bc, $0D12               ; row 13, col 18
        call    SCR_POS                 ; screen pos in HL
        pop     de
        ex      de, hl                  ; DE=screen, HL=torpedo path
        ld      bc, $0008               ; 1 torpedo and 7 blanks
        ldir                            ; draw
        ret


#ENDASM [$462A]

   15  REM 
#ASM [$4630]

; --------------------------------------------------------------------------------
; Draw the 4 cannons in front of the fortress
; --------------------------------------------------------------------------------


DRAW_CANONS:
        ld      a, (fortress_col)
        sub     $02                     ; fortress column - 2
        ld      c, a
        ld      b, $05                  ; row 5
        call    draw_cannon
        nop
        call    draw_cannon
        call    draw_cannon
        nop
        call    draw_cannon
        ret


draw_cannon:
        call    SCR_POS                 ; draw cannon at roc-col BC
        ld      (hl), $81               ; top part of cannon
        ld      de, $0021
        add     hl, de                  ; add 33
        ld      (hl), $02               ; bottom part of cannon
        inc     b                       ; leave 4 rows interval
        inc     b
        inc     b
        inc     b
        ret


DEAD16:
        defb    $00, $00, $00

#ENDASM [$465A]

   16  REM 
#ASM [$4660]

bomb_col:
        defb    $00

bomb_row:
        defb    $11

bomb_running:
        defb    $00

; --------------------------------------------------------------------------------
; select bomb position from fortress and ship position
; --------------------------------------------------------------------------------


FIRE_BOMB:
        ld      a, (bomb_running)
        cp      $01
        ret     z                       ; exit if bomb is still running
        ld      a, (fortress_col)
        sub     $03
        ld      l, a                    ; L=fortress-col - 3
        ld      a, (ship_row)
        ld      h, a                    ; H=ship row
        ld      a, $05
        sub     h
        jr      c, not_cannon1
        ld      h, $05                  ; fire from row 5
        jr      do_fire_bomb


not_cannon1:
        ld      a, $09
        sub     h
        jr      c, not_cannon2
        ld      h, $09                  ; fire from row 9
        jr      do_fire_bomb


not_cannon2:
        ld      a, $0D
        sub     h
        jr      c, not_cannon3
        ld      h, $0D                  ; fire from row 13
        jr      do_fire_bomb


not_cannon3:
        ld      h, $11                  ; fire from row 17

do_fire_bomb:
        ld      (bomb_col), hl
        ld      a, $01
        ld      (bomb_running), a
        ret


#ENDASM [$4699]

   17  REM 
#ASM [$469F]

SHIP_HIT:
        defb    $00

DRAW_BOMB:
        xor     a
        ld      (SHIP_HIT), a           ; clear ship-hit
        ld      bc, (bomb_col)          ; get position of bomb
        call    SCR_POS                 ; HL = screen address of bomb
        ld      (hl), $00               ; delete bomb
        xor     a
        cp      c                       ; compare column with zero
        jr      nz, move_bomb           ; jump forward if not at col zero
        ld      (bomb_running), a       ; at col zero, clear bomb-running
        ret


move_bomb:
        dec     c                       ; move bomb left in column
        dec     hl                      ; and screen address
        cp      (hl)                    ; did we hit something?
        jr      z, bomb_no_hit          ; jump forward if not
        ld      a, $34
        cp      (hl)                    ; did we hit the torpedo?
        jr      nz, bomb_hit_ship       ; jump if not, we hit the ship

bomb_no_hit:
        ld      (hl), $04               ; draw bomb
        ld      (bomb_col), bc
        ret


bomb_hit_ship:
        inc     (hl)                    ; show rubish on hit-point
        ld      (bomb_hit_ship), a      ; NOTE: should not be here
                                        ; writes a $34 on the code address, which is inc (hl)
        ld      a, $01
        ld      (SHIP_HIT), a
        ret


DEAD17:
        defb    $00, $00, $00, $00, $00

#ENDASM [$46D5]

   18  REM 
#ASM [$46DB]

timer_fortress_move:
        defw    $0061                   ; count times until fortress moves

; --------------------------------------------------------------------------------
; advance timer and move fortress left if it reaches $C8
; --------------------------------------------------------------------------------


CHECK_FORTRESS_MOVE:
        ld      hl, (timer_fortress_move)
        inc     hl

FORTRESS_MOVE_COUNT:
        ld      de, $00C8               ; value is POKEd from BASIC
        and     a
        sbc     hl, de
        jr      z, do_fortress_move
        add     hl, de
        ld      (timer_fortress_move), hl
        ret                             ; not time yet to move


do_fortress_move:
        ld      hl, $0000
        ld      (timer_fortress_move), hl
                                        ; reset timer
        call    DELETE_TORPEDO
        ld      bc, (fortress_col)
        dec     c                       ; move fortress one position left
        ld      (fortress_col), bc
        dec     c
        dec     c
        call    SCR_POS                 ; minus 2=cannons column
        ld      a, $14                  ; 20 lines

move_next_row:
        ld      bc, $000D               ; 13 chars = fortress+cannons
        ld      d, h                    ; destintion
        ld      e, l
        inc     hl                      ; source=destination+1
        ldir                            ; move row
        dec     hl
        ld      (hl), $00               ; delete char to the right
        ld      bc, $0014               ; move to next line
        add     hl, bc
        dec     a
        cp      $00
        jr      nz, move_next_row       ; next row
        ret


#ENDASM [$471C]

   19  REM 
#ASM [$4722]

SCORE_lo:
        defb    $38

SCORE_hi:
        defb    $04

NUM_BUFFER:
        defm    " 1080"

N10000:
        defw    $2710

N1000:
        defw    $03E8

N100:
        defw    $0064

N10:
        defw    $000A

N1:
        defw    $0001

PRINT_ACC:
        defw    $0000

HAVE_DIGITS:
        defb    $01

; Note: does not work for 0, outputs all blanks


SCORE_I2A:
        xor     a
        ld      (HAVE_DIGITS), a
        ld      hl, (SCORE_lo)          ; get SCORE
        ld      (PRINT_ACC), hl         ; put in number to print
        ld      bc, NUM_BUFFER          ; output buffer
        ld      hl, N10000              ; table of divisors
        push    hl                      ; save address of table

next_divisor:
        xor     a
        pop     hl                      ; HL=next divisor address
        ld      e, (hl)
        inc     hl
        ld      d, (hl)                 ; DE=next divisor
        dec     hl                      ; restore HL
        push    hl                      ; save HL

subtract_divisor:
        ld      hl, (PRINT_ACC)         ; get accumulator
        and     a
        sbc     hl, de                  ; subtract divisor
        ld      (PRINT_ACC), hl         ; store accumulator
        jr      c, subtraction_overflowed
                                        ; jump forward if overflowed
        inc     a                       ; count one more
        jr      subtract_divisor


subtraction_overflowed:
        add     hl, de                  ; restore accumulator
        cp      $00
        jr      nz, make_digit          ; jump forward if not zero
        ld      a, (HAVE_DIGITS)        ; if this digit is 0 and have_digits is 0, output a space
        cp      $00
        ld      a, $00
        jr      z, output_digit

make_digit:
        add     a, $1C                  ; add '0'
        push    af
        ld      a, $01
        ld      (HAVE_DIGITS), a
        pop     af

output_digit:
        ld      (PRINT_ACC), hl         ; restore accumulator
        ld      (bc), a                 ; store digit
        inc     bc
        pop     hl                      ; advance in divisor table
        inc     hl
        inc     hl
        push    hl
        ld      a, e
        cp      $01                     ; check for last divisor
        jr      nz, next_divisor
        pop     hl
        ret


DEAD18:
        defb    $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

#ENDASM [$478D]

   20  REM 
#ASM [$4793]

INCR_SCORE:
        ld      a, (ALIEN_HIT)
        cp      $01
        ret     z
        ld      a, (WALL_HIT)
        cp      $00
        ret     z
        xor     a
        ld      (WALL_HIT), a
        ld      de, $0003
        ld      hl, (topedo_pos0)
        ld      h, $00
        and     a
        sbc     hl, de
        ex      de, hl
        ld      hl, (SCORE_lo)

; these 5 instructions are POKEd in BASIC to either 
; NOP or ADD HL,DE depending on the selected skill level


score_multiply:
        add     hl, de
        add     hl, de
        add     hl, de
        add     hl, de
        add     hl, de
        ld      (SCORE_lo), hl          ; save score
        call    SCORE_I2A               ; convert to digit sequence
        ld      hl, (D_FILE)            ; display
        ld      de, $0012               ; add position of score
        add     hl, de
        ex      de, hl                  ; to DE
        ld      hl, NUM_BUFFER          ; HL=number buffer
        ld      b, $05                  ; 5 digits

out_digit:
        ld      a, (hl)
        cp      $00                     ; is it a blank?
        jr      z, skip_blanks
        ldi                             ; copy digit
        inc     bc                      ; restore BC
        djnz    out_digit               ; next digit
        ret


skip_blanks:
        inc     hl
        djnz    out_digit               ; next digit
        ret


DEAD19:
        defb    $00, $00

#ENDASM [$47DB]

   21  REM 
#ASM [$47E1]

DELETE_TORPEDO:
        ld      bc, (torpedo_col)
        call    SCR_POS
        ld      a, $34                  ; does position has a torpedo?
        cp      (hl)
        ret     nz                      ; ret if not
        ld      (hl), $00               ; delete torpedo
        ret


DELETE_BOMB:
        ldi
        ld      h, b
        ld      b, (hl)
        call    SCR_POS
        ld      (hl), $00               ; delete bomb
        ret


#ENDASM [$47F9]

   22  REM 
#ASM [$47FF]

; --------------------------------------------------------------------------------
; Init restored fortress
; --------------------------------------------------------------------------------


INIT_FORTRESS:
        ld      b, $3A                  ; 58 chars black
        ld      hl, FORTRESS1

fortress_fill_outer_black:
        ld      (hl), $80
        inc     hl
        djnz    fortress_fill_outer_black
        ld      b, $32                  ; 50 chars grey

fortress_fill_grey:
        ld      (hl), $08
        inc     hl
        djnz    fortress_fill_grey
        ld      b, $2A                  ; 42 chars black

fortress_fill_inner_black:
        ld      (hl), $80
        inc     hl
        djnz    fortress_fill_inner_black
        ld      hl, $0215
        ld      (fortress_col), hl
        ld      hl, $0000
        ld      (timer_fortress_move), hl
        ld      (WALL_HIT), hl
        ld      hl, $0900
        ld      (ship_col), hl
        ld      hl, $0100
        ld      (bomb_col), hl
        xor     a
        ld      (torpedo_running), a
        ld      (bomb_running), a
        ld      (SHIP_HIT), a
        ret


DEAD21:
        defb    $00, $00

#ENDASM [$483F]

   23  REM 
#ASM [$4845]

press_any_key_msg:
        defm    "HIT ANY LETTER WHEN READY"

; --------------------------------------------------------------------------------
; Print message "Press any key"
; --------------------------------------------------------------------------------


PRESS_ANY_KEY:
        ld      bc, $1703               ; row 23, col 3
        call    SCR_POS
        ex      de, hl                  ; DE=screen address
        ld      hl, press_any_key_msg
        ld      bc, $0019
        ldir
        ret


DEAD22:
        defb    $00, $00, $00

DELETE_PRESS_ANY_KEY:
        ld      bc, $1703               ; row 23, col 3
        call    SCR_POS
        ld      b, $19

put_space:
        ld      (hl), $00
        inc     hl
        djnz    put_space
        ret


DEAD23:
        defb    $00, $00, $00

#ENDASM [$4882]

   24  REM 
#ASM [$4888]

DEAD24:
        defb    $00, $85, $01, $85, $00, $00, $02, $80, $03, $00, $00, $87, $80, $04, $00, $00, $81, $00, $82, $00, $00, $00, $00, $00, $00, $87, $07, $80, $84, $04, $00, $84, $03, $07, $00, $00, $02, $80, $01, $00, $00, $00, $00, $00, $00, $01

FLASH_ALIEN:
        ld      bc, (fortress_col)
        inc     c
        inc     c
        inc     c                       ; fortress-col+3
        ld      b, $05                  ; row=5
        call    SCR_POS                 ; HL=screen address
        ld      de, $001C               ; distance between rows
        ld      a, $0E                  ; 14 lines

flash_next_line:
        push    af                      ; save line counter
        ld      b, $05                  ; 5 columns

flash_next_col:
        ld      a, (hl)                 ; get screen char
        xor     $80                     ; invert it
        ld      (hl), a                 ; and store
        inc     hl
        djnz    flash_next_col          ; loop for all columns
        add     hl, de                  ; next row
        pop     af                      ; get row count
        dec     a
        cp      $00
        jr      nz, flash_next_line     ; loop for next row
        ret


DEAD25:
        defb    $EB, $21, $9C, $48, $C3, $77, $43, $3E, $02, $BE, $20, $1F, $3C, $32, $B5, $48, $EB, $23, $36, $05, $23, $36, $02, $23, $36, $05, $11, $1F, $00, $19, $36, $03, $23, $23, $36, $01, $11, $A3, $00, $19, $36, $80, $C9, $3E, $01, $32, $B5, $48, $EB, $23, $23, $36, $00, $23, $36, $85, $11, $21, $00, $19, $36, $03, $11, $A5, $00, $19, $36, $80, $C9, $00, $00, $00

#ENDASM [$4921]

   25  REM 
#ASM [$4927]

DEAD26:
        defb    $ED, $4B, $1C, $41, $0C, $0C, $0C, $06, $05, $ED, $43, $46, $43, $CD, $05, $44, $06, $0E, $ED, $43, $46, $43, $CD, $05, $44, $C9, $00, $00, $00, $00, $00, $00, $00, $00

#ENDASM [$4949]

   26  REM 
#ASM [$494F]

LEVEL_DELAY_lo:
        defb    $01

LEVEL_DELAY_hi:
        defb    $00

; --------------------------------------------------------------------------------
; Game loop
; --------------------------------------------------------------------------------


GAME:
        call    CHECK_FORTRESS_MOVE
        call    TEST_KEYBOARD
        call    DRAW_ALIEN
        call    DRAW_FORTRESS
        call    MOVE_TORPEDO
        call    INCR_SCORE
        ld      a, (ALIEN_HIT)
        cp      $01
        ret     z
        call    DRAW_CANONS
        call    DRAW_BOMB
        ld      a, (SHIP_HIT)
        cp      $01
        ret     z
        call    FIRE_BOMB
        ld      hl, (LEVEL_DELAY_lo)
        ld      de, $0000

game_delay:
        inc     de
        and     a
        sbc     hl, de
        jp      z, GAME
        add     hl, de
        jr      game_delay


DEAD27:
        defb    $00, $00, $00, $00, $00, $00, $00

#ENDASM [$498F]

   27  REM 
#ASM [$4995]

DEAD28:
        defb    $3E, $BF, $BD, $28, $06, $3E, $7F, $BD, $C2, $F0, $43, $3A, $1C, $41, $C3, $C5, $43, $00, $00, $00, $00, $00, $00, $00

#ENDASM [$49AD]

   28  REM 
#ASM [$49B3]

CLEAR_GAME_AREA:
        ld      bc, $0200               ; row 2 column 0
        call    SCR_POS                 ; HL = screen address
        ld      a, $14                  ; 20 rows

clear_next_row:
        ld      b, $20                  ; 32 chars

L49BD:
        ld      (hl), $00               ; clear char
        inc     hl
        djnz    L49BD                   ; loop for columns
        inc     hl
        dec     a
        cp      $00
        jr      nz, clear_next_row      ; loop for rows
        ret


#ENDASM [$49C9]

   29  REM 
#ASM [$49CF]

pressed_key:
        defw    $FBDF

; --------------------------------------------------------------------------------
; Test keyboard and do actions
; --------------------------------------------------------------------------------


TEST_KEYBOARD:
        call    KEYBOARD                ; scan keyboard, H=column, L=half-row

L49D4:
        ld      bc, (ship_col)
        ld      (pressed_key), hl
        ld      de, $FFFF
        and     a
        sbc     hl, de
        ret     z                       ; no key pressed, return
        ld      hl, (pressed_key)
        bit     4, l                    ; check top right half-row
        call    z, FIRE_TORPEDO         ; key pressed - fire torpedo
        ld      hl, (pressed_key)
        ld      bc, (ship_col)
        bit     5, l                    ; check 2nd right half-row
        call    z, FIRE_TORPEDO         ; key pressed - fire torpedo
        ld      hl, (pressed_key)
        ld      bc, (ship_col)
        bit     3, l                    ; check top left half-row
        jp      z, MOVE_SHIP_UP         ; key pressed - move ship up
        bit     2, l                    ; check second left half-row
        jp      z, MOVE_SHIP_UP         ; key pressed - move ship up
        bit     0, l                    ; check bottom left half-row
        jp      z, MOVE_SHIP_DOWN       ; key pressed - move ship down
        bit     1, l                    ; check third left half-row
        jp      z, MOVE_SHIP_DOWN       ; key pressed - move ship down
        bit     6, l                    ; check thrird right half-row
        jr      z, check_left_right     ; key pressed - move ship left or right
        bit     7, l                    ; check bottom right half-row
        ret     nz                      ; not pressed - return

check_left_right:
        nop
        nop
        nop
        nop
        nop
        bit     5, h                    ; check move ship left
        jp      z, MOVE_SHIP_LEFT
        bit     4, h                    ; check move ship left
        jp      z, MOVE_SHIP_LEFT
        bit     3, h                    ; check move ship left
        jp      z, MOVE_SHIP_LEFT
        bit     2, h                    ; check move ship right
        jp      z, MOVE_SHIP_RIGHT
        bit     1, h                    ; check move ship right
        jp      z, MOVE_SHIP_RIGHT
        ret


DEAD29:
        defb    $00, $00, $00, $00, $00, $00

#ENDASM [$4A3D]


#: --------------------------------------------------------------------------------
#: Start of introdution
#: --------------------------------------------------------------------------------


@Line_StartIntro:
   30  CLS 

#: init torpedo position

   34  POKE &intro_torpedo_pos_lo,252
   35  POKE &intro_torpedo_pos_hi,69

#: show welcome

   40  PRINT  AT 5,11;"WELCOME TO";
   50  PRINT  AT 7,5;"\.'\''\'. \''\:'\' \ '\':\'' \.'\''\'. \.'\''\'. \: \ .\' ";
   60  PRINT  AT 8,5;"\:.\..\.:  \:   \ :  \:.\..\.: \:    \:'\'.";
   70  PRINT  AT 9,5;"\:  \ :  \:   \ :  \:  \ : \'.\..\.' \:  \'.";\00
   80  PRINT  AT 17,3;"COPR., PAUL CARLSON, 1982";
   90  PRINT  AT 21,0;"DO YOU WANT INSTRUCTIONS? (Y/N)";

#: clear ship numbers

  100  POKE &SHIP1_nr,128
  110  POKE &SHIP2_nr,128
  120  POKE &SHIP3_nr,128

#: --------------------------------------------------------------------------------
#: Intro wait key
#: --------------------------------------------------------------------------------


@Line_IntroWaitKey:
  140  LET L= USR &INTRO_SCREEN
  150  LET L= USR &INTRO_FIRE_TORPEDO
  155  IF  INKEY$ ="" THEN  GOTO @Line_IntroWaitKey
  160  IF  INKEY$ ="N" THEN  GOTO @Line_StartGame
  170  IF  INKEY$ <>"Y" THEN  GOTO @Line_IntroWaitKey
  180  GOSUB @Line_Instructions

#: --------------------------------------------------------------------------------
#: Start of new game
#: --------------------------------------------------------------------------------


@Line_StartGame:
  190  GOSUB @Line_SelectLevel

#: W is the ship number init to 0, incremented below to start at 1

  195  LET W=28
  200  LET S=0
  202  LET R=200

#: 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT

  205  POKE 18146,R
  210  POKE &SCORE_lo,0
  220  POKE &SCORE_hi,0
  230  LET L= USR &INIT_FORTRESS
  250  PRINT  AT 0,10;"SCORE: 0"

#: --------------------------------------------------------------------------------
#: Start of new ship
#: --------------------------------------------------------------------------------


@Line_StartNewShip:
  260  LET W=W+1

#: if incremented ship is >5, exit

  270  IF W=34 THEN  GOTO @Line_GameOver

#: write ship number in the three ship drawings

  280  POKE &SHIP1_nr,W
  290  POKE &SHIP2_nr,W
  300  POKE &SHIP3_nr,W

@Line_StartLevelWaitKey:
  310  LET L= USR &DRAW_CANONS
  320  LET L= USR &PRESS_ANY_KEY

@Line_LevelWaitKey:
  340  LET L= USR &DRAW_FORTRESS
  350  LET L= USR &DRAW_ALIEN
  360  LET L= USR &DRAW_SHIP
  370  IF  INKEY$ ="" THEN  GOTO @Line_LevelWaitKey
  380  LET L= USR &DELETE_PRESS_ANY_KEY

#: --------------------------------------------------------------------------------
#: Run the game
#: --------------------------------------------------------------------------------

  390  LET L= USR &GAME
  400  LET L= USR &DELETE_TORPEDO
  410  LET L= USR &DELETE_BOMB
  420  IF  PEEK &ALIEN_HIT=1 THEN  GOTO @Line_AlienHit

#: --------------------------------------------------------------------------------
#: Ship was hit
#: --------------------------------------------------------------------------------

  430  FOR N=1 TO 32
  440  LET L= USR &ROTATE_SHIP
  450  LET L=L+N
  460  NEXT N
  470  LET L= USR &DELETE_SHIP

#: init next ship

  475  POKE &bomb_col,0
  480  POKE &torpedo_running,0
  485  POKE &bomb_running,0
  490  POKE &ship_col,0
  500  POKE &ship_row,9
  505  POKE &SHIP_HIT,0
  510  GOTO @Line_StartNewShip

#: --------------------------------------------------------------------------------
#: Alien was hit
#: --------------------------------------------------------------------------------


@Line_AlienHit:
  520  FOR N=1 TO 50
  540  LET L= USR &FLASH_ALIEN
  545  LET L=L+0
  547  LET L=L+0
  550  NEXT N
  560  LET S= PEEK &SCORE_lo+256* PEEK &SCORE_hi
  570  LET S=S+F*(( PEEK &topedo_pos0)-3)
  575  PRINT  AT 0,17;S
  580  POKE &SCORE_hi, INT (S/256)
  590  POKE &SCORE_lo,(S-256* PEEK &SCORE_hi)
  592  IF R>20 THEN  LET R=R-20

#: 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT

  595  POKE 18146,R
  600  LET L= USR &CLEAR_GAME_AREA
  620  LET L= USR &INIT_FORTRESS
  630  GOTO @Line_StartLevelWaitKey

#: --------------------------------------------------------------------------------
#: Game Over
#: --------------------------------------------------------------------------------


@Line_GameOver:
  640  LET L= USR &CLEAR_GAME_AREA
  650  PRINT  AT 0,4;"FINAL";
  660  PRINT  AT 10,13;"\:'\''\''\''\''\':"
  670  PRINT  AT 11,13;"\: GAME\ :"
  680  PRINT  AT 12,13;"\: OVER\ :"
  690  PRINT  AT 13,13;"\:.\..\..\..\..\.:"
  700  PRINT  AT 16,6;"PLAY AGAIN? (Y OR N)"

@Line__720:
  720  IF  INKEY$ ="N" THEN  STOP 
  730  IF  INKEY$ ="Y" THEN  GOTO @Line_StartGame
  740  GOTO @Line__720

#: --------------------------------------------------------------------------------
#: Show intructions
#: --------------------------------------------------------------------------------


@Line_Instructions:
 1000  CLS 
 1010  PRINT  AT 0,10;"YOUR GOAL:"
 1020  PRINT  AT 2,0;"TO SCORE AS MANY POINTS AS YOU   CAN BY DESTROYING THE ALIEN AND BLOCKS OF HIS FORTRESS."
 1030  PRINT  AT 6,0;"YOU HAVE FIVE SHIPS TO DESTROY   THE ALIEN AS MANY TIMES AS YOU  CAN.  WARNING - THE ALIEN GUNS  ARE INDESTRUCTIBLE."
 1040  PRINT  AT 11,0;"POINTS ARE AWARDED BASED ON      SKILL LEVEL AND THE DISTANCE    YOUR SHIP IS FROM THE LEFT EDGE OF THE SCREEN WHEN IT FIRES."
 1050  PRINT  AT 16,0;"DESTROYING THE ALIEN IS WORTH    FIFTY TIMES THE POINTS AWARDED  FOR DESTROYING A BLOCK OF HIS   FORTRESS."
 1060  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."

@Line_IntructionsWaitKey1:
 1070  IF  INKEY$ ="" THEN  GOTO @Line_IntructionsWaitKey1
 1080  CLS 
 1090  PRINT  AT 0,3;"YOUR SHIPS CONTROL PANEL:"
 1100  PRINT  AT 3,2;"\:'\''\''\''\''\''\''\''\''\''\''\':    \:'\''\''\''\''\''\''\''\''\''\''\':    \: 1 2 3 4 5 \ :    \: 6 7 8 9 0 \ :    \:  Q W E R T\ :    \:  Y U I O P\ :    \:.\..\..\..\..\..\..\..\..\..\..\.:    \:.\..\..\..\..\..\..\..\..\..\..\.:    MOVE SHIP UP    FIRE TORPEDO"\00
 1110  PRINT  AT 11,0;"\:'\''\''\''\''\''\''\''\''\''\':  \:'\''\''\''\''\''\''\': \:'\''\''\''\''\''\''\''\''\':\: A S D F G\ :  \:  H J K\ : \:  L ENTER\ :\:  Z X C V \ :  \: B N M \ : \: . SPACE \ :\:.\..\..\..\..\..\..\..\..\..\.:  \:.\..\..\..\..\..\..\.: \:.\..\..\..\..\..\..\..\..\.: MOVE SHIP     MOVE      MOVE      DOWN        SHIP      SHIP                  LEFT      RIGHT"
 1120  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."

@Line_IntructionsWaitKey2:
 1130  IF  INKEY$ ="" THEN  GOTO @Line_IntructionsWaitKey2
 1140  RETURN 

#: --------------------------------------------------------------------------------
#: Select skill level
#: --------------------------------------------------------------------------------


@Line_SelectLevel:
 2000  CLS 
 2010  PRINT  AT 2,1;"SKILL LEVELS:"
 2020  PRINT  AT 4,15;"1 = SLOW"
 2030  PRINT  AT 6,15;"2 = AVERAGE"
 2040  PRINT  AT 8,15;"3 = FAST"
 2050  PRINT  AT 10,15;"4 = VERY FAST"
 2060  PRINT  AT 12,15;"5 = SUICIDAL"
 2070  PRINT  AT 16,6;"PRESS SKILL LEVEL..."

@Line_WaitSkillSelection:
 2080  IF  INKEY$ ="" THEN  GOTO @Line_WaitSkillSelection
 2090  LET V= CODE  INKEY$ -28
 2100  IF V<1 OR V>5 THEN  GOTO @Line_WaitSkillSelection

#: selected skill is in V

 2105  CLS 
 2110  POKE &LEVEL_DELAY_lo,1
 2120  POKE &LEVEL_DELAY_hi,5-V
 2130  LET F=50*V

#: 5 instructions in score_multiply (18353+1..28353+5) are replaced by 
#: NOP (0) or ADD HL,DE (25=$19) depending on the selected level 
#: 1 ADDs for level 1, 5 ADDs for level 5

 2150  FOR N=1 TO 5
 2160  LET A=18353+N
 2170  POKE A,0
 2180  IF N<=V THEN  POKE A,25
 2190  NEXT N
 2200  RETURN 

#: --------------------------------------------------------------------------------
#: Save and autostart routine
#: --------------------------------------------------------------------------------

 3000  SAVE "ATTAC%K"
 3100  GOTO @Line_StartIntro

#: Line_StartIntro          =     30
#: Line_IntroWaitKey        =    140
#: Line_StartGame           =    190
#: Line_StartNewShip        =    260
#: Line_StartLevelWaitKey   =    310
#: Line_LevelWaitKey        =    340
#: Line_AlienHit            =    520
#: Line_GameOver            =    640
#: Line__720                =    720
#: Line_Instructions        =   1000
#: Line_IntructionsWaitKey1 =   1070
#: Line_IntructionsWaitKey2 =   1130
#: Line_SelectLevel         =   2000
#: Line_WaitSkillSelection  =   2080

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
#: DB_ST                    = $4027 = 16423
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
#: FORTRESS1                = $4082 = 16514
#: SAVE_FORTRESS1           = $40BB = 16571
#: FORTRESS2                = $40BC = 16572
#: addr_fortress            = $4118 = 16664
#: addr_fortress_save       = $411A = 16666
#: fortress_col             = $411C = 16668
#: fortress_row             = $411D = 16669
#: fortress_rows            = $411E = 16670
#: fortress_cols            = $411F = 16671
#: fortress_blocks          = $4120 = 16672
#: DEAD01                   = $4121 = 16673
#: DRAW_FORTRESS            = $412C = 16684
#: rotate_inner_layer       = $415B = 16731
#: return                   = $418A = 16778
#: rotate_outer_layer       = $4191 = 16785
#: rot_blocks_left_column   = $41B9 = 16825
#: rot_block_next1          = $41C6 = 16838
#: rot_blocks_right_column  = $41D4 = 16852
#: rot_block_next2          = $41E3 = 16867
#: rot_blocks_top_row       = $41E6 = 16870
#: DEAD02                   = $41F2 = 16882
#: SCR_POS                  = $4212 = 16914
#: SCR_POS_next_row         = $421C = 16924
#: SCR_POS_row_found        = $4223 = 16931
#: SCR_POS_next_col         = $4226 = 16934
#: SCR_POS_col_found        = $422D = 16941
#: DEAD03                   = $4230 = 16944
#: erase_hit_block_from_fortress = $4238 = 16952
#: L4252                    = $4252 = 16978
#: L425C                    = $425C = 16988
#: DEAD04                   = $4269 = 17001
#: alien_col                = $4274 = 17012
#: alien_row                = $4275 = 17013
#: DEAD05                   = $4276 = 17014
#: ALIEN                    = $4277 = 17015
#: DRAW_ALIEN               = $429F = 17055
#: store_alien_pos          = $42BE = 17086
#: alien_downwards          = $42C2 = 17090
#: DEAD06                   = $42CF = 17103
#: draw_alien_at_BC         = $42D1 = 17105
#: alien_each_row           = $42DE = 17118
#: alien_direction          = $42EF = 17135
#: DEAD07                   = $42F0 = 17136
#: SHIP1                    = $42FA = 17146
#: SHIP1_nr                 = $4306 = 17158
#: SHIP2                    = $4313 = 17171
#: SHIP2_nr                 = $431F = 17183
#: SHIP3                    = $432C = 17196
#: SHIP3_nr                 = $4338 = 17208
#: which_ship               = $4345 = 17221
#: ship_col                 = $4346 = 17222
#: ship_row                 = $4347 = 17223
#: DRAW_SHIP                = $4348 = 17224
#: DRAW_SHIP_BC             = $434C = 17228
#: not_ship1                = $4361 = 17249
#: not_ship2                = $436F = 17263
#: draw_ship_hl             = $4377 = 17271
#: draw_ship_row            = $4379 = 17273
#: DEAD08                   = $438A = 17290
#: DEAD09                   = $4395 = 17301
#: MOVE_SHIP_RIGHT          = $43C2 = 17346
#: MOVE_SHIP_DOWN           = $43D0 = 17360
#: MOVE_SHIP_UP             = $43DB = 17371
#: MOVE_SHIP_LEFT           = $43E6 = 17382
#: DEAD10                   = $43F0 = 17392
#: save_ship_pos            = $43F5 = 17397
#: DEAD11                   = $43FD = 17405
#: DELETE_SHIP              = $4405 = 17413
#: delete_row               = $4412 = 17426
#: delete_col               = $4414 = 17428
#: DEAD12                   = $4421 = 17441
#: torpedo_running          = $442C = 17452
#: torpedo_col              = $442D = 17453
#: torpedo_row              = $442E = 17454
#: topedo_pos0              = $442F = 17455
#: FIRE_TORPEDO             = $4431 = 17457
#: DEAD13                   = $4453 = 17491
#: WALL_HIT                 = $445E = 17502
#: ALIEN_HIT                = $445F = 17503
#: MOVE_TORPEDO             = $4460 = 17504
#: topedo_erased            = $447B = 17531
#: draw_torpedo_store_pos   = $4487 = 17543
#: torpedo_hit              = $448F = 17551
#: hit_fortress             = $44AB = 17579
#: stop_torpedo             = $44B6 = 17590
#: DEAD14                   = $44BB = 17595
#: BLOCK1                   = $44CA = 17610
#: save_block1              = $44CD = 17613
#: DEAD20                   = $44CE = 17614
#: INTRO_SCREEN             = $44D1 = 17617
#: draw_top_bar             = $44DA = 17626
#: draw_right_bar           = $44E7 = 17639
#: each_right_bar_block     = $44EC = 17644
#: draw_bottom_bar          = $4505 = 17669
#: draw_left_bar            = $4520 = 17696
#: each_left_bar_block      = $4525 = 17701
#: ROTATED_SHIP1            = $455F = 17759
#: ROTATED_SHIP2            = $4578 = 17784
#: ROTATED_SHIP3            = $4591 = 17809
#: ROTATED_SHIP4            = $45AA = 17834
#: rotate_count             = $45C3 = 17859
#: ROTATE_SHIP              = $45C4 = 17860
#: store_count              = $45D1 = 17873
#: add_25                   = $45D8 = 17880
#: DEAD15                   = $45E8 = 17896
#: torpedo_path             = $45F3 = 17907
#: intro_torpedo_pos_lo     = $4604 = 17924
#: intro_torpedo_pos_hi     = $4605 = 17925
#: INTRO_FIRE_TORPEDO       = $4606 = 17926
#: restore_torpedo_pos      = $4617 = 17943
#: draw_torpedo_path        = $4618 = 17944
#: DRAW_CANONS              = $4630 = 17968
#: draw_cannon              = $4647 = 17991
#: DEAD16                   = $4657 = 18007
#: bomb_col                 = $4660 = 18016
#: bomb_row                 = $4661 = 18017
#: bomb_running             = $4662 = 18018
#: FIRE_BOMB                = $4663 = 18019
#: not_cannon1              = $467C = 18044
#: not_cannon2              = $4685 = 18053
#: not_cannon3              = $468E = 18062
#: do_fire_bomb             = $4690 = 18064
#: SHIP_HIT                 = $469F = 18079
#: DRAW_BOMB                = $46A0 = 18080
#: move_bomb                = $46B5 = 18101
#: bomb_no_hit              = $46BF = 18111
#: bomb_hit_ship            = $46C6 = 18118
#: DEAD17                   = $46D0 = 18128
#: timer_fortress_move      = $46DB = 18139
#: CHECK_FORTRESS_MOVE      = $46DD = 18141
#: FORTRESS_MOVE_COUNT      = $46E1 = 18145
#: do_fortress_move         = $46EE = 18158
#: move_next_row            = $4707 = 18183
#: SCORE_lo                 = $4722 = 18210
#: SCORE_hi                 = $4723 = 18211
#: NUM_BUFFER               = $4724 = 18212
#: N10000                   = $4729 = 18217
#: N1000                    = $472B = 18219
#: N100                     = $472D = 18221
#: N10                      = $472F = 18223
#: N1                       = $4731 = 18225
#: PRINT_ACC                = $4733 = 18227
#: HAVE_DIGITS              = $4735 = 18229
#: SCORE_I2A                = $4736 = 18230
#: next_divisor             = $4747 = 18247
#: subtract_divisor         = $474E = 18254
#: subtraction_overflowed   = $475C = 18268
#: make_digit               = $476A = 18282
#: output_digit             = $4773 = 18291
#: DEAD18                   = $4783 = 18307
#: INCR_SCORE               = $4793 = 18323
#: score_multiply           = $47B2 = 18354
#: out_digit                = $47CA = 18378
#: skip_blanks              = $47D5 = 18389
#: DEAD19                   = $47D9 = 18393
#: DELETE_TORPEDO           = $47E1 = 18401
#: DELETE_BOMB              = $47EF = 18415
#: INIT_FORTRESS            = $47FF = 18431
#: fortress_fill_outer_black = $4804 = 18436
#: fortress_fill_grey       = $480B = 18443
#: fortress_fill_inner_black = $4812 = 18450
#: DEAD21                   = $483D = 18493
#: press_any_key_msg        = $4845 = 18501
#: PRESS_ANY_KEY            = $485E = 18526
#: DEAD22                   = $486E = 18542
#: DELETE_PRESS_ANY_KEY     = $4871 = 18545
#: put_space                = $4879 = 18553
#: DEAD23                   = $487F = 18559
#: DEAD24                   = $4888 = 18568
#: FLASH_ALIEN              = $48B6 = 18614
#: flash_next_line          = $48C7 = 18631
#: flash_next_col           = $48CA = 18634
#: DEAD25                   = $48D9 = 18649
#: DEAD26                   = $4927 = 18727
#: LEVEL_DELAY_lo           = $494F = 18767
#: LEVEL_DELAY_hi           = $4950 = 18768
#: GAME                     = $4951 = 18769
#: game_delay               = $497E = 18814
#: DEAD27                   = $4988 = 18824
#: DEAD28                   = $4995 = 18837
#: CLEAR_GAME_AREA          = $49B3 = 18867
#: clear_next_row           = $49BB = 18875
#: L49BD                    = $49BD = 18877
#: pressed_key              = $49CF = 18895
#: TEST_KEYBOARD            = $49D1 = 18897
#: L49D4                    = $49D4 = 18900
#: check_left_right         = $4A18 = 18968
#: DEAD29                   = $4A37 = 18999

#: [$4000] $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#: [$4040] $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$bbb
#: [$4080] bbTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$40C0] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$4100] TTTTTTTTTTTTTTTTTTTTTTTTWWWWBBBBBBBBBBbbbbbbCCCCCCCCCCCCCCCCCCCC
#: [$4140] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4180] CCCCCCCCCCCbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$41C0] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBBBBBBBBBB
#: [$4200] BBBBBBBBBBBBbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBbbbbbbCCCCCCCC
#: [$4240] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbBBBTTTTTTTTT
#: [$4280] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$42C0] CCCCCCCCCCCCCCCBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbTTTTTT
#: [$4300] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$4340] TTTTTBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4380] CCCCCCCCCCBBBBBbbbbbbBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
#: [$43C0] BBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBCCCCCCCCBBb
#: [$4400] bbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbBBBWWCCCCCCCCCCCCCCC
#: [$4440] CCCCCCCCCCCCCCCCCCCBBBBBbbbbbbBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4480] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBB
#: [$44C0] BBBBbbbbbbTTTTBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4500] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4540] CCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$4580] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$45C0] TTTBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbTTTTTTTTTTTTT
#: [$4600] TTTTBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbCCCCCCCCCCCCCCCC
#: [$4640] CCCCCCCCCCCCCCCCCCCCCCCBBBbbbbbbBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4680] CCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$46C0] CCCCCCCCCCCCCCCCBBBBBbbbbbbWWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4700] CCCCCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbBBTTTTTWWWWWWWWWWWWBCCCCCCCCCC
#: [$4740] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4780] CCCBBBBBBBBBBbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$47C0] CCCCCCCCCCCCCCCCCCCCCCCCCBBbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCbbbbbbC
#: [$4800] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBb
#: [$4840] bbbbbTTTTTTTTTTTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCBBBCCCCCCCCCCCCCCB
#: [$4880] BBbbbbbbBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCC
#: [$48C0] CCCCCCCCCCCCCCCCCCCCCCCCCBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
#: [$4900] BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBbbbbbbBBBBBBBBBBBBBBBBBBBBBBBBB
#: [$4940] BBBBBBBBBbbbbbbBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4980] CCCCCCCCBBBBBBBbbbbbbBBBBBBBBBBBBBBBBBBBBBBBBbbbbbbCCCCCCCCCCCCC
#: [$49C0] CCCCCCCCCbbbbbbWWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4A00] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBBbbb
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

