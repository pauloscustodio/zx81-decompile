#ASM

KEYBOARD    equ $02BB
D_FILE      equ $400C
LAST_K		equ $4025

		defb $76, $76			; hide REM line

CHEAT:	defb 0					; set to 1 to fire only bottom canon

FORTRESS1:
        defb $00, $00, $80, $80, $00, $80, $80, $80, $00, $00, $80, $80, $00, $80, $00, $00
        defb $00, $00, $00, $00, $80, $00, $80, $80, $80, $80, $80, $00, $80, $80, $00, $80
        defb $00, $80, $80, $80, $80, $00, $00, $00, $00, $80, $80, $80, $80, $00, $80, $80
        defb $00, $80, $80, $00, $00, $80, $80, $00, $00

SAVE_FORTRESS1:
        defb $80

FORTRESS2:
        defb $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
        defb $08, $08, $08, $00, $00, $08, $08, $08, $08, $00, $08, $00, $08, $08, $08, $08
        defb $08, $08, $08, $08, $00, $08, $08, $08, $08, $08, $08, $08, $08, $08, $00, $08
        defb $08, $00, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80
        defb $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80
        defb $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80

addr_fortress:
        defw $40EE

addr_fortress_save:
        defw $4117

fortress_col:
        defb $13

fortress_row:
        defb $02

fortress_rows:
        defb $10

fortress_cols:
        defb $05

fortress_blocks:
        defb $29


;--------------------------------------------------------------------------------
; Draw fortress at fortress_row/fortress_col
;--------------------------------------------------------------------------------

DRAW_FORTRESS:
        ld hl, SAVE_FORTRESS1
        ld (addr_fortress_save), hl
        ld hl, FORTRESS1
        ld (addr_fortress), hl
        ld a, $14               ; 20 rows
        ld (fortress_rows), a
        ld a, $09               ; 9 columns
        ld (fortress_cols), a
        ld a, $39               ; 57 blocks
        ld (fortress_blocks), a
        call rotate_outer_layer
        call rotate_inner_layer
        call rotate_inner_layer
        dec b
        dec b
        dec c
        dec c
        ld (fortress_col), bc
        jp L_418A


rotate_inner_layer:
        inc b
        inc c
        ld (fortress_col), bc
        ld (addr_fortress), de
        ld a, (fortress_blocks)
        sub $08
        ld (fortress_blocks), a
        ld h, $00
        ld l, a
        add hl, de
        ld (addr_fortress_save), hl
        ld a, (fortress_rows)
        sub $02
        ld (fortress_rows), a
        ld a, (fortress_cols)
        sub $02
        ld (fortress_cols), a
        nop
        nop
        nop
        call rotate_outer_layer

L_418A:
        ret


rotate_outer_layer:
        ld de, (addr_fortress_save)
                                ; point to save block
        ld a, (de)              ; get block to be saved to A
        ld hl, (addr_fortress_save)
                                ; get address of save
        dec hl                  ; minus 1=last block
        ld b, $00               ; BC will be number of blocks
        push af                 ; save end block
        ld a, (fortress_blocks) ; get number of blocks
        ld c, a                 ; to BC
        pop af                  ; A is block to be saved from end
        lddr                    ; move all blocks 1 step
        ld hl, (addr_fortress)  ; get first address
        ld (hl), a              ; and store saved char
        push hl                 ; save address of fortress
        ld bc, (fortress_col)   ; coords of fortress to BC
        call SCR_POS            ; screen address to HL
        ex de, hl               ; screen address to DE
        pop hl                  ; HL=blocks address
        push bc                 ; save coords
        ld a, (fortress_rows)   ; A=number of rows
        ld bc, $0020            ; distance between rows

rot_blocks_left_column:
        ldi                     ; copy one
        inc bc                  ; restore BC
        dec a                   ; count rows
        cp $00
        jr z, rot_block_next1   ; jump forward if end of rows
        ex de, hl
        add hl, bc              ; move to next row
        ex de, hl
        jr rot_blocks_left_column


rot_block_next1:
        ld bc, (fortress_cols)
        ld b, $00               ; BC = number of cols
        ldir                    ; copy bottom row
        ld a, (fortress_rows)   ; A = number of rows
        ld bc, $0022

rot_blocks_right_column:
        ldi                     ; copy block
        inc bc                  ; restore BC
        dec a                   ; count rows
        cp $00
        jr z, rot_block_next2   ; jump forward if finished
        ex de, hl
        and a
        sbc hl, bc              ; move to previous row
        ex de, hl
        jr rot_blocks_right_column


rot_block_next2:
        ld a, (fortress_cols)   ; A = number of columns

rot_blocks_top_row:
        dec de
        dec de
        ldi                     ; copy one block
        dec a
        cp $00
        jr nz, rot_blocks_top_row
                                ; jump back if not finished
        pop bc
        ex de, hl               ; DE=blocks, HL=screen
        ret

;--------------------------------------------------------------------------------
; Compute screen position of row-col in BC to HL
; Preserves all other registers
;--------------------------------------------------------------------------------

SCR_POS:
        push de
        push af
        ld hl, (D_FILE)
        inc hl
        xor a
        ld de, $0021

SCR_POS_next_row:
        cp b
        jr z, SCR_POS_row_found
        add hl, de
        inc a
        jr SCR_POS_next_row


SCR_POS_row_found:
        ld a, c
        ld e, $00

SCR_POS_next_col:
        cp e
        jr z, SCR_POS_col_found
        inc e
        inc hl
        jr SCR_POS_next_col


SCR_POS_col_found:
        pop af
        pop de
        ret

erase_hit_block_from_fortress:
        ld hl, FORTRESS1
        ld de, (fortress_col)
        push de
        ld a, b
        sub d
        ld d, $00
        ld e, a
        add hl, de
        pop de
        ld a, c
        sub e
        cp $01
        jr z, L_4252
        jr nc, L_425C
        ld (hl), $00
        ret


L_4252:
        ld d, $00
        ld a, $39
        nop
        ld e, a
        add hl, de
        ld (hl), $00
        ret


L_425C:
        ld d, $00
        ld a, $39
        nop
        add a, a
        sub $08
        ld e, a
        add hl, de
        ld (hl), $00
        ret

alien_col:
        defb $16

alien_row:
        defb $08

ALIEN:
        defb $00, $00, $00, $00, $00
        defb $87, $07, $80, $84, $04
        defb $00, $84, $03, $07, $00
        defb $00, $02, $80, $01, $00
        defb $00, $07, $80, $84, $00
        defb $00, $87, $80, $04, $00
        defb $00, $81, $00, $82, $00
        defb $00, $00, $00, $00, $00
;--------------------------------------------------------------------------------
; Draw alien at fortress_row/fortress_col
;--------------------------------------------------------------------------------

DRAW_ALIEN:
        ld hl, (fortress_col)
        ld a, l
        add a, $03
        ld (alien_col), a       ; fortress column+3 to alien column
        ld bc, (alien_col)
        push bc
        ld a, (alien_direction)
        cp $00
        jr z, alien_downwards
        dec b
        call draw_alien_at_BC
        pop bc
        ld a, b
        cp $06
        ret nz
        xor a

store_alien_pos:
        ld (alien_direction), a
        ret


alien_downwards:
        inc b
        call draw_alien_at_BC
        pop bc
        ld a, b
        cp $0A
        ret nz
        ld a, $01
        jr store_alien_pos

draw_alien_at_BC:
        ld (alien_col), bc
        call SCR_POS
        ex de, hl               ; DE=screen address
        ld hl, ALIEN            ; source of alien
        ld a, $08               ; 8 rows

alien_each_row:
        ld bc, $0005            ; 4 cols
        ldir                    ; copu one row
        dec a
        cp $00
        ret z                   ; exit afyet 8 rows
        ex de, hl
        ld bc, $001C            ; advance to next row
        add hl, bc
        ex de, hl
        jr alien_each_row


alien_direction:
        defb $01

SHIP1:
        defb $81, $80, $80, $80, $82
        defb $82, $82, $81, $83, $80
        defb $00, $80, $80, $80, $00
        defb $07, $07, $84, $03, $80
        defb $84, $80, $80, $80, $07

SHIP2:
        defb $81, $80, $80, $80, $82
        defb $82, $81, $83, $82, $81
        defb $00, $80, $80, $80, $00
        defb $07, $84, $03, $07, $84
        defb $84, $80, $80, $80, $07

SHIP3:
        defb $81, $80, $80, $80, $82
        defb $80, $83, $82, $81, $81
        defb $00, $80, $80, $80, $00
        defb $80, $03, $07, $84, $84
        defb $84, $80, $80, $80, $07

which_ship:
        defb $03

ship_col:
        defb $00

ship_row:
        defb $09
;--------------------------------------------------------------------------------
; Draw ship at row-col ship_row/ship_col
;--------------------------------------------------------------------------------

DRAW_SHIP:
        ld bc, (ship_col)
;--------------------------------------------------------------------------------
; Draw ship at row-col BC
;--------------------------------------------------------------------------------

DRAW_SHIP_BC:
        call SCR_POS            ; HL=screen position
        ex de, hl               ; DE=screen position
        ld hl, which_ship
        ld a, $01
        cp (hl)
        jr nz, not_ship1
        inc a
        ld (which_ship), a
        ld hl, SHIP1
        jr draw_ship_hl


not_ship1:
        ld a, $02
        cp (hl)
        jr nz, not_ship2
        inc a
        ld (which_ship), a
        ld hl, SHIP2
        jr draw_ship_hl


not_ship2:
        ld a, $01
        ld (which_ship), a
        ld hl, SHIP3
; DE is screen address, HL is ship

draw_ship_hl:
        ld a, $05               ; 5 rows

draw_ship_row:
        ld bc, $0005            ; 5 columns
        ldir                    ; copy one row
        dec a
        cp $00
        ret z                   ; return when 5 rows are printed
        ex de, hl
        ld bc, $001C            ; DE+=33-5=28
        add hl, bc
        ex de, hl
        jr draw_ship_row


;--------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
;--------------------------------------------------------------------------------

MOVE_SHIP_RIGHT:
        ld a, (fortress_col)    ; A=fortress column
        sub $08                 ; -8 (5 for ship + 2 for cannons + 1)
        cp c
        jr z, save_ship_pos     ; reached limit
        call DELETE_SHIP
        inc c                   ; move right
        jr save_ship_pos

;--------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
;--------------------------------------------------------------------------------

MOVE_SHIP_DOWN:
        ld a, $10
        cp b
        jr z, save_ship_pos     ; reached limit
        call DELETE_SHIP
        inc b                   ; move down
        jr save_ship_pos

;--------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
;--------------------------------------------------------------------------------

MOVE_SHIP_UP:
        ld a, $03
        cp b
        jr z, save_ship_pos     ; reached limit
        call DELETE_SHIP
        dec b                   ; move up
        jr save_ship_pos

;--------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
;--------------------------------------------------------------------------------

MOVE_SHIP_LEFT:
        xor a
        cp c
        jr z, save_ship_pos     ; reached limit
        call DELETE_SHIP
        dec c                   ; move right
        jr save_ship_pos

save_ship_pos:
        ld (ship_col), bc
        call DRAW_SHIP
        ret

;--------------------------------------------------------------------------------
; Delete ship before moving it
;--------------------------------------------------------------------------------

DELETE_SHIP:
        push bc                 ; save coords
        ld bc, (ship_col)
        call SCR_POS            ; HL=screen address
        ld a, $05               ; 5 rows
        ld de, $001C            ; distance in screen between rows

delete_row:
        ld b, $05               ; 5 cols

delete_col:
        ld (hl), $00            ; delete char
        inc hl
        djnz delete_col
        add hl, de              ; next row
        dec a
        cp $00
        jr nz, delete_row
        pop bc                  ; restore coords
        ret

torpedo_running:
        defb $00

torpedo_col:
        defb $11

torpedo_row:
        defb $12

topedo_pos0:
        defw $120A
;--------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
;--------------------------------------------------------------------------------

FIRE_TORPEDO:
        ld a, (torpedo_running) ; is a torpedo on the way?
        cp $01
        ret z                   ; return if yes
        ld hl, (ship_col)
        inc h
        inc h
        inc l
        inc l
        inc l
        inc l
        ld (torpedo_col), hl    ; torpedo coord is mouth of ship
        ld (topedo_pos0), hl    ; make a copy of the cooords - why?
        ld b, h
        ld c, l
        call SCR_POS            ; HL=screen position
        ld (hl), $34            ; draw torpedo
        ld a, $01
        ld (torpedo_running), a ; signal torpedo on the way
        ret

WALL_HIT:
        defb $00

ALIEN_HIT:
        defb $00
;--------------------------------------------------------------------------------
; moves torpedo if it has been created
;--------------------------------------------------------------------------------

MOVE_TORPEDO:
        xor a
        ld (WALL_HIT), a
        ld (ALIEN_HIT), a
        ld a, (torpedo_running)
        cp $00
        ret z                   ; torpedo was not fired, return
        ld bc, (torpedo_col)
        call SCR_POS            ; HL=torpedo coord
        ld a, $34
        cp (hl)                 ; is there a torpedo?
        jr nz, topedo_erased    ; jump forward if not
        ld (hl), $00            ; erase it

topedo_erased:
        ld de, (fortress_col)
        ld a, e
        dec a
        sub c
        jr z, torpedo_hit       ; torpedo col == fortress col -1
        jr c, torpedo_hit       ; torpedo col >  fortress col -1
        inc hl                  ; move torpedo

draw_torpedo_store_pos:
        ld (hl), $34            ; draw it
        inc c
        ld (torpedo_col), bc    ; store new coords
        ret


torpedo_hit:
        inc hl
        ld a, e
        add a, $04              ; A=fortress-col+4 (alien)
        sub c
        jr z, stop_torpedo      ; inside alien - stop torpedo
        xor a
        cp (hl)                 ; hit space - continue
        jr z, draw_torpedo_store_pos
        ld a, $80
        cp (hl)                 ; hit black border
        jr z, hit_fortress
        ld a, $08
        cp (hl)                 ; hit gray border
        jr z, hit_fortress
        ld a, $01
        ld (ALIEN_HIT), a
        jr stop_torpedo


hit_fortress:
        inc c
        ld (hl), $00            ; erase block
        call erase_hit_block_from_fortress
        ld a, $01
        ld (WALL_HIT), a

stop_torpedo:
        xor a
        ld (torpedo_running), a
        ret

BLOCK1:
        defb $00, $80, $80      ; WBB block

save_block1:
        defb $80

;--------------------------------------------------------------------------------
; Draw rotating border and ship
;--------------------------------------------------------------------------------

INTRO_SCR:
        ld bc, $0301            ; row 3, col 1
        call SCR_POS            ; HL=screen address
        ex de, hl               ; DE=screen address
        ld b, $0B               ; 10 times+1 for one LDI decrementing B

draw_top_bar:
        ld hl, BLOCK1
        ldi                     ; copy block to screen
        ldi                     ; three times
        ldi
        djnz draw_top_bar       ; 30 chars copied (LDI decs BC, DJNZ decs B)
        ld a, $05               ; copy 5 blocks of 3 chars

draw_right_bar:
        ld b, $03
        ld hl, BLOCK1

each_right_bar_block:
        push bc                 ; save BC
        ld bc, $0020            ; add 32 to DE, LDI with add 1 more
        ex de, hl
        add hl, bc              ; HL+=32
        pop bc
        ex de, hl               ; DE+=32
        ldi                     ; DE+=33
        djnz each_right_bar_block
                                ; copy 3 blocks
        dec a                   ; times 5
        cp $00                  ; NOTE: not needed
        jr nz, draw_right_bar
        ld bc, $0020            ; add 32 to DE
        ex de, hl
        add hl, bc
        ex de, hl               ; DE+=32
        ld b, $0A               ; 10 blocks of 3 = 30

draw_bottom_bar:
        ld hl, BLOCK1
        ld a, (hl)              ; copy 3 chars
        ld (de), a
        inc hl
        dec de
        ld a, (hl)
        ld (de), a
        inc hl
        dec de
        ld a, (hl)
        ld (de), a
        nop                     ; NOTE: not needed
        dec de
        djnz draw_bottom_bar    ; repeat 10 times
        ld bc, $0020            ; subtract 32 from DE
        ex de, hl
        and a
        sbc hl, bc              ; DE-=32
        ex de, hl
        ld a, $05               ; copy 5 blocks of 3 chars

draw_left_bar:
        ld b, $03
        ld hl, BLOCK1

each_left_bar_block:
        push af                 ; copy one char
        ld a, (hl)
        ld (de), a
        pop af
        push bc
        ld bc, $0021            ; subtract 33 from address
        ex de, hl
        and a
        sbc hl, bc              ; DE-=33
        pop bc
        ex de, hl
        inc hl
        djnz each_left_bar_block
                                ; copy 3 blocks
        dec a
        cp $00
        jr nz, draw_left_bar    ; times 5
; rotate block
        ld a, (BLOCK1)
        ld (save_block1), a
        ld a, (BLOCK1+1)
        ld (BLOCK1), a
        ld a, (BLOCK1+2)
        ld (BLOCK1+1), a
        ld a, (save_block1)
        ld (BLOCK1+2), a
        ld bc, $0B0D            ; line 11, column 13
        jp DRAW_SHIP_BC


ROTATED_SHIP1:
        defb $00, $81, $84, $07, $00
        defb $81, $86, $07, $82, $81
        defb $82, $07, $00, $81, $84
        defb $07, $84, $81, $86, $07
        defb $00, $81, $82, $07, $00

ROTATED_SHIP2:
        defb $81, $80, $00, $07, $82
        defb $80, $81, $80, $07, $80
        defb $80, $81, $00, $07, $80
        defb $80, $81, $80, $07, $80
        defb $84, $81, $00, $80, $07

ROTATED_SHIP3:
        defb $00, $84, $07, $82, $00
        defb $82, $81, $84, $06, $82
        defb $07, $82, $00, $84, $81
        defb $84, $06, $82, $07, $84
        defb $00, $84, $81, $82, $00

ROTATED_SHIP4:
        defb $81, $80, $80, $80, $82
        defb $82, $82, $82, $82, $80
        defb $00, $80, $00, $80, $00
        defb $80, $84, $84, $84, $84
        defb $84, $80, $80, $80, $07

rotate_count:
        defb $04

ROTATE_SHIP:
        ld de, 5*5              ; 5*5=25 bytes per ship
        ld a, (rotate_count)    ; counts 1 to 4
        inc a
        cp $05
        jr nz, store_count
        ld a, $01

store_count:
        ld (rotate_count), a
        ld hl, ROTATED_SHIP1-5*5 ; address of ship1-25 bytes
        ld b, a

add_25:
        add hl, de
        djnz add_25
        push hl                 ; save source address of ship
        ld bc, (ship_col)       ; get ship position
        call SCR_POS            ; HL = screen position
        ex de, hl               ; DE = screen position
        pop hl                  ; HL = source of rotated ship
        jp draw_ship_hl


torpedo_path:
        defb $00, $00, $00, $00, $00, $00, $00, $00, $34, $00, $00, $00, $00, $00, $00, $00
        defb $00

intro_torpedo_pos:
        defw $45FA
;--------------------------------------------------------------------------------
; Fire intro torpedo
;--------------------------------------------------------------------------------

INTRO_FIRE_TORPEDO:
        ld hl, (intro_torpedo_pos)
        dec hl                  ; move back one pos
        ld bc, torpedo_path
        and a
        sbc hl, bc
        jr nc, restore_torpedo_pos
        ld hl, torpedo_path+8	; reached start, go back to end
        jr draw_torpedo_path


restore_torpedo_pos:
        add hl, bc

draw_torpedo_path:
        push hl
        ld (intro_torpedo_pos), hl
        ld bc, $0D12            ; row 13, col 18
        call SCR_POS            ; screen pos in HL
        pop de
        ex de, hl               ; DE=screen, HL=torpedo path
        ld bc, $0008            ; 1 torpedo and 7 blanks
        ldir                    ; draw
        ret


;--------------------------------------------------------------------------------
; Draw the 4 cannons in front of the fortress
;--------------------------------------------------------------------------------

DRAW_CANONS:
        ld a, (fortress_col)
        sub $02                 ; fortress column - 2
        ld c, a
        ld b, $05               ; row 5
        call draw_cannon
        nop
        call draw_cannon
        call draw_cannon
        nop
        call draw_cannon
        ret


draw_cannon:
        call SCR_POS            ; draw cannon at roc-col BC
        ld (hl), $81            ; top part of cannon
        ld de, $0021
        add hl, de              ; add 33
        ld (hl), $02            ; bottom part of cannon
        inc b                   ; leave 4 rows interval
        inc b
        inc b
        inc b
        ret


bomb_col:
        defb $00

bomb_row:
        defb $11

bomb_running:
        defb $00
;--------------------------------------------------------------------------------
; select bomb position from fortress and ship position
;--------------------------------------------------------------------------------

FIRE_BOMB:
        ld a, (bomb_running)
        cp $01
        ret z                   ; exit if bomb is still running
        ld a, (fortress_col)
        sub $03
        ld l, a                 ; L=fortress-col - 3
        ld a, (ship_row)
        ld h, a                 ; H=ship row
        ld a, $05
        sub h
        jr c, not_cannon1
        ld h, $05               ; fire from row 5
        jr do_fire_bomb


not_cannon1:
        ld a, $09
        sub h
        jr c, not_cannon2
        ld h, $09               ; fire from row 9
        jr do_fire_bomb


not_cannon2:
        ld a, $0D
        sub h
        jr c, not_cannon3
        ld h, $0D               ; fire from row 13
        jr do_fire_bomb


not_cannon3:
        ld h, $11               ; fire from row 17

do_fire_bomb:
		ld a, (CHEAT)
		and a
		jr z, continue_fire_bomb
		
        ld h, $11               ; fire from row 17 if cheating
		
continue_fire_bomb:
        ld (bomb_col), hl
        ld a, $01
        ld (bomb_running), a
        ret


SHIP_HIT:
        defb $00

DRAW_BOMB:
        xor a
        ld (SHIP_HIT), a        ; clear ship-hit
        ld bc, (bomb_col)       ; get position of bomb
        call SCR_POS            ; HL = screen address of bomb
        ld (hl), $00            ; delete bomb
        xor a
        cp c                    ; compare column with zero
        jr nz, move_bomb        ; jump forward if not at col zero
        ld (bomb_running), a    ; at col zero, clear bomb-running
        ret


move_bomb:
        dec c                   ; move bomb left in column
        dec hl                  ; and screen address
        cp (hl)                 ; did we hit something?
        jr z, bomb_no_hit       ; jump forward if not
        ld a, $34
        cp (hl)                 ; did we hit the torpedo?
        jr nz, bomb_hit_ship    ; jump if not, we hit the ship

bomb_no_hit:
        ld (hl), $04            ; draw bomb
        ld (bomb_col), bc
        ret


bomb_hit_ship:
        inc (hl)                ; show rubish on hit-point
        ld (bomb_hit_ship), a   ; NOTE: should not be here
                                ; writes a $34 on the code address, which is inc (hl)
        ld a, $01
        ld (SHIP_HIT), a
        ret


timer_fortress_move:
        defw $0061              ; count times until fortress moves
;--------------------------------------------------------------------------------
; advance timer and move fortress left if it reaches $C8
;--------------------------------------------------------------------------------

CHECK_FORTRESS_MOVE:
        ld hl, (timer_fortress_move)
        inc hl

FORTRESS_MOVE_COUNT:
        ld de, $00C8
        and a
        sbc hl, de
        jr z, do_fortress_move
        add hl, de
        ld (timer_fortress_move), hl
        ret                     ; not time yet to move


do_fortress_move:
        ld hl, $0000
        ld (timer_fortress_move), hl
                                ; reset timer
        call DELETE_TORPEDO
        ld bc, (fortress_col)
        dec c                   ; move fortress one position left
        ld (fortress_col), bc
        dec c
        dec c
        call SCR_POS            ; minus 2=cannons column
        ld a, $14               ; 20 lines

move_next_row:
        ld bc, $000D            ; 13 chars = fortress+cannons
        ld d, h                 ; destintion
        ld e, l
        inc hl                  ; source=destination+1
        ldir                    ; move row
        dec hl
        ld (hl), $00            ; delete char to the right
        ld bc, $0014            ; move to next line
        add hl, bc
        dec a
        cp $00
        jr nz, move_next_row    ; next row
        ret


SCORE:
        defw $0438

NUM_BUFFER:
        defb $00, $1D, $1C, $24, $1C

N10000:
        defw $2710

N1000:
        defw $03E8

N100:
        defw $0064

N10:
        defw $000A

N1:
        defw $0001

PRINT_ACC:
        defw $0000

HAVE_DIGITS:
        defb $01
; Note: does not work for 0, outputs all blanks

SCORE_I2A:
        xor a
        ld (HAVE_DIGITS), a
        ld hl, (SCORE)          ; get SCORE
        ld (PRINT_ACC), hl      ; put in number to print
        ld bc, NUM_BUFFER       ; output buffer
        ld hl, N10000           ; table of divisors
        push hl                 ; save address of table

next_divisor:
        xor a
        pop hl                  ; HL=next divisor address
        ld e, (hl)
        inc hl
        ld d, (hl)              ; DE=next divisor
        dec hl                  ; restore HL
        push hl                 ; save HL

subtract_divisor:
        ld hl, (PRINT_ACC)      ; get accumulator
        and a
        sbc hl, de              ; subtract divisor
        ld (PRINT_ACC), hl      ; store accumulator
        jr c, subtraction_overflowed
                                ; jump forward if overflowed
        inc a                   ; count one more
        jr subtract_divisor


subtraction_overflowed:
        add hl, de              ; restore accumulator
        cp $00
        jr nz, make_digit       ; jump forward if not zero
        ld a, (HAVE_DIGITS)     ; if this digit is 0 and have_digits is 0, output a space
        cp $00
        ld a, $00
        jr z, output_digit

make_digit:
        add a, $1C              ; add '0'
        push af
        ld a, $01
        ld (HAVE_DIGITS), a
        pop af

output_digit:
        ld (PRINT_ACC), hl      ; restore accumulator
        ld (bc), a              ; store digit
        inc bc
        pop hl                  ; advance in divisor table
        inc hl
        inc hl
        push hl
        ld a, e
        cp $01                  ; check for last divisor
        jr nz, next_divisor
        pop hl
        ret


INCR_SCORE:
        ld a, (ALIEN_HIT)
        cp $01
        ret z
        ld a, (WALL_HIT)
        cp $00
        ret z
        xor a
        ld (WALL_HIT), a
        ld de, $0003
        ld hl, (topedo_pos0)
        ld h, $00
        and a
        sbc hl, de
        ex de, hl
        ld hl, (SCORE)

score_multiply:
        add hl, de
        add hl, de
        add hl, de
        add hl, de
        add hl, de
        ld (SCORE), hl          ; save score
        call SCORE_I2A          ; convert to digit sequence
        ld hl, (D_FILE)         ; display
        ld de, $0012            ; add position of score
        add hl, de
        ex de, hl               ; to DE
        ld hl, NUM_BUFFER       ; HL=number buffer
        ld b, $05               ; 5 digits

out_digit:
        ld a, (hl)
        cp $00                  ; is it a blank?
        jr z, skip_blanks
        ldi                     ; copy digit
        inc bc                  ; restore BC
        djnz out_digit          ; next digit
        ret


skip_blanks:
        inc hl
        djnz out_digit          ; next digit
        ret


DELETE_TORPEDO:
        ld bc, (torpedo_col)
        call SCR_POS
        ld a, $34               ; does position has a torpedo?
        cp (hl)
        ret nz                  ; ret if not
        ld (hl), $00            ; delete torpedo
        ret


DELETE_BOMB:
        ld bc, (bomb_col)
        call SCR_POS
        ld (hl), $00            ; delete bomb
        ret


;--------------------------------------------------------------------------------
; Init restored fortress
;--------------------------------------------------------------------------------

INIT_FORTRESS:
        ld b, $3A               ; 58 chars black
        ld hl, FORTRESS1

fortress_fill_outer_black:
        ld (hl), $80
        inc hl
        djnz fortress_fill_outer_black
        ld b, $32               ; 50 chars grey

fortress_fill_grey:
        ld (hl), $08
        inc hl
        djnz fortress_fill_grey
        ld b, $2A               ; 42 chars black

fortress_fill_inner_black:
        ld (hl), $80
        inc hl
        djnz fortress_fill_inner_black
        ld hl, $0215
        ld (fortress_col), hl
        ld hl, $0000
        ld (timer_fortress_move), hl
        ld (WALL_HIT), hl
        ld hl, $0900
        ld (ship_col), hl
        ld hl, $0100
        ld (bomb_col), hl
        xor a
        ld (torpedo_running), a
        ld (bomb_running), a
        ld (SHIP_HIT), a
        ret


press_any_key_msg:
        defb $2D, $2E, $39, $00, $26, $33, $3E, $00, $31, $2A, $39, $39, $2A, $37, $00, $3C
        defb $2D, $2A, $33, $00, $37, $2A, $26, $29, $3E
;--------------------------------------------------------------------------------
; Print message "Press any key"
;--------------------------------------------------------------------------------

PRESS_ANY_KEY:
        ld bc, $1703            ; row 23, col 3
        call SCR_POS
        ex de, hl               ; DE=screen address
        ld hl, press_any_key_msg
        ld bc, $0019
        ldir
        ret

DELETE_PRESS_ANY_KEY:
        ld bc, $1703            ; row 23, col 3
        call SCR_POS
        ld b, $19

put_space:
        ld (hl), $00
        inc hl
        djnz put_space
        ret

FLASH_ALIEN:
        ld bc, (fortress_col)
        inc c
        inc c
        inc c                   ; fortress-col+3
        ld b, $05               ; row=5
        call SCR_POS            ; HL=screen address
        ld de, $001C            ; distance between rows
        ld a, $0E               ; 14 lines

flash_next_line:
        push af                 ; save line counter
        ld b, $05               ; 5 columns

flash_next_col:
        ld a, (hl)              ; get screen char
        xor $80                 ; invert it
        ld (hl), a              ; and store
        inc hl
        djnz flash_next_col     ; loop for all columns
        add hl, de              ; next row
        pop af                  ; get row count
        dec a
        cp $00
        jr nz, flash_next_line  ; loop for next row
        ret

LEVEL_DELAY:
        defw $0001
;--------------------------------------------------------------------------------
; Game loop
;--------------------------------------------------------------------------------

GAME:
        call CHECK_FORTRESS_MOVE
        call TEST_KEYBOARD
        call DRAW_ALIEN
        call DRAW_FORTRESS
        call MOVE_TORPEDO
        call INCR_SCORE
        ld a, (ALIEN_HIT)
        cp $01
        ret z
        call DRAW_CANONS
        call DRAW_BOMB
        ld a, (SHIP_HIT)
        cp $01
        ret z
        call FIRE_BOMB
        ld hl, (LEVEL_DELAY)
        ld de, $0000

game_delay:
        inc de
        and a
        sbc hl, de
        jp z, GAME
        add hl, de
        jr game_delay

CLEAR_GAME_AREA:
        ld bc, $0200            ; row 2 column 0
        call SCR_POS            ; HL = screen address
        ld a, $14               ; 20 rows

clear_next_row:
        ld b, $20               ; 32 chars

L_49BD:
        ld (hl), $00            ; clear char
        inc hl
        djnz L_49BD             ; loop for columns
        inc hl
        dec a
        cp $00
        jr nz, clear_next_row   ; loop for rows
        ret

;--------------------------------------------------------------------------------
; Test keyboard and do actions
;--------------------------------------------------------------------------------

TEST_KEYBOARD:
        ld hl,(LAST_K) 			; keyboard scan, H=column, L=half-row
        ld bc, (ship_col)

		ld a, (CHEAT)
		and a
		call nz, FIRE_TORPEDO
		
        ld de, $FFFF
        and a
        sbc hl, de
        ret z                   ; no key pressed, return

; detect arrow keys
        ld hl, (LAST_K)
        bit 4, l                ; check top right half-row
        call z, FIRE_TORPEDO    ; key pressed - fire torpedo

        ld hl, (LAST_K)
        ld bc, (ship_col)
		
		; top arrow
		bit 4, l
		jr nz, next_arrow1
		bit 4, h
		jp z, MOVE_SHIP_UP
next_arrow1:
		; bottom arrow
		bit 4, l
		jr nz, next_arrow2
		bit 5, h
		jp z, MOVE_SHIP_DOWN
next_arrow2:
		; left arrow
		bit 3, l
		jr nz, next_arrow3
		bit 5, h
		jp z, MOVE_SHIP_LEFT
next_arrow3:
		; right arrow
		bit 4, l
		jr nz, next_arrow4
		bit 3, h
		jp z, MOVE_SHIP_RIGHT
		
next_arrow4:		
        bit 5, l                ; check 2nd right half-row
        call z, FIRE_TORPEDO    ; key pressed - fire torpedo
        ld hl, (LAST_K)
        ld bc, (ship_col)
        bit 3, l                ; check top left half-row
        jp z, MOVE_SHIP_UP      ; key pressed - move ship up
        bit 2, l                ; check second left half-row
        jp z, MOVE_SHIP_UP      ; key pressed - move ship up
        bit 0, l                ; check bottom left half-row
        jp z, MOVE_SHIP_DOWN    ; key pressed - move ship down
        bit 1, l                ; check third left half-row
        jp z, MOVE_SHIP_DOWN    ; key pressed - move ship down
        bit 6, l                ; check thrird right half-row
        jr z, check_left_right  ; key pressed - move ship left or right
        bit 7, l                ; check bottom right half-row
        ret nz                  ; not pressed - return

check_left_right:
        nop
        nop
        nop
        nop
        nop
        bit 5, h                ; check move ship left
        jp z, MOVE_SHIP_LEFT
        bit 4, h                ; check move ship left
        jp z, MOVE_SHIP_LEFT
        bit 3, h                ; check move ship left
        jp z, MOVE_SHIP_LEFT
        bit 2, h                ; check move ship right
        jp z, MOVE_SHIP_RIGHT
        bit 1, h                ; check move ship right
        jp z, MOVE_SHIP_RIGHT
        ret

#ENDASM

   30  CLS 
# init torpedo position
   34  POKE &intro_torpedo_pos,(&torpedo_path+9)-256*INT((&torpedo_path+9)/256)
   35  POKE &intro_torpedo_pos+1,INT((&torpedo_path+9)/256)

   40  PRINT  AT 5,11;"WELCOME TO";
   50  PRINT  AT 7,5;"\.'\''\'. \''\:'\' \ '\':\'' \.'\''\'. \.'\''\'. \: \ .\' ";
   60  PRINT  AT 8,5;"\:.\..\.:  \:   \ :  \:.\..\.: \:    \:'\'.";
   70  PRINT  AT 9,5;"\:  \ :  \:   \ :  \:  \ : \'.\..\.' \:  \'.";
   80  PRINT  AT 17,3;"COPR., PAUL CARLSON, 1982";
   90  PRINT  AT 21,0;"DO YOU WANT INSTRUCTIONS? (Y/N)";

# close als ships windows
  100  POKE &SHIP1+12,128
  110  POKE &SHIP2+12,128
  120  POKE &SHIP3+12,128

@WAIT_INTRO:
  140  LET L= USR &INTRO_SCR
  150  LET L= USR &INTRO_FIRE_TORPEDO
  155  IF  INKEY$ ="" THEN  GOTO @WAIT_INTRO
  160  IF  INKEY$ ="N" THEN  GOTO @DO_GAME
  170  IF  INKEY$ <>"Y" THEN  GOTO @WAIT_INTRO

  180  GOSUB @INSTRUCTIONS

@DO_GAME:
  190  GOSUB @INSERT_LEVEL
  195  LET W=CODE "0"            # W: number of ship
  200  LET S=0
  202  LET R=200
  205  POKE &FORTRESS_MOVE_COUNT+1,R
  210  POKE &SCORE,0
  220  POKE &SCORE+1,0
  230  LET L= USR &INIT_FORTRESS
  250  PRINT  AT 0,10;"SCORE: 0"

# increment ship number, exit if too many lives
@NXT_LIVE:
  260  LET W=W+1
  270  IF W=CODE "6" THEN  GOTO @END_GAME

  # write ship number in windows
  280  POKE &SHIP1+12,W
  290  POKE &SHIP2+12,W
  300  POKE &SHIP3+12,W

@NXT_LEVEL:
  310  LET L= USR &DRAW_CANONS
  320  LET L= USR &PRESS_ANY_KEY
@WAIT_GAME_START:
  340  LET L= USR &DRAW_FORTRESS
  350  LET L= USR &DRAW_ALIEN
  360  LET L= USR &DRAW_SHIP
  370  IF  INKEY$ ="" THEN  GOTO @WAIT_GAME_START

  380  LET L= USR &DELETE_PRESS_ANY_KEY
  390  LET L= USR &GAME
  400  LET L= USR &DELETE_TORPEDO
  410  LET L= USR &DELETE_BOMB
  420  IF  PEEK &ALIEN_HIT=1 THEN  GOTO @HIT_ALIEN
  430  FOR N=1 TO 32
  440  LET L= USR &ROTATE_SHIP
  450  LET L=L+N
  460  NEXT N
  470  LET L= USR &DELETE_SHIP
  475  POKE &bomb_col,0
  480  POKE &torpedo_running,0
  485  POKE &bomb_running,0
  490  POKE &ship_col,0
  500  POKE &ship_row,9
  505  POKE &SHIP_HIT,0
  510  GOTO @NXT_LIVE

@HIT_ALIEN:
  520  FOR N=1 TO 50
  540  LET L= USR &FLASH_ALIEN
  545  LET L=L+0
  547  LET L=L+0
  550  NEXT N
  560  LET S= PEEK &SCORE+256* PEEK (&SCORE+1)
  570  LET S=S+F*(( PEEK 17455)-3)
  575  PRINT  AT 0,17;S
  580  POKE &SCORE+1, INT (S/256)
  590  POKE &SCORE,(S-256* PEEK (&SCORE+1)))
  592  IF R>20 THEN  LET R=R-20
  595  POKE &FORTRESS_MOVE_COUNT+1,R
  600  LET L= USR &CLEAR_GAME_AREA
  620  LET L= USR &INIT_FORTRESS
  630  GOTO @NXT_LEVEL

@END_GAME:
  640  LET L= USR &CLEAR_GAME_AREA
  650  PRINT  AT 0,4;"FINAL";
  660  PRINT  AT 10,13;"\:'\''\''\''\''\':"
  670  PRINT  AT 11,13;"\: GAME\ :"
  680  PRINT  AT 12,13;"\: OVER\ :"
  690  PRINT  AT 13,13;"\:.\..\..\..\..\.:"
  700  PRINT  AT 16,6;"PLAY AGAIN? (Y OR N)"
  
@WAIT_KEY1:
  720  IF  INKEY$ ="N" THEN  STOP 
  730  IF  INKEY$ ="Y" THEN  GOTO @DO_GAME
  740  GOTO @WAIT_KEY1
 
@INSTRUCTIONS:
 1000  CLS 
 1010  PRINT  AT 0,10;"YOUR GOAL:"
 1020  PRINT  AT 2,0;"TO SCORE AS MANY POINTS AS YOU   CAN BY DESTROYING THE ALIEN AND BLOCKS OF HIS FORTRESS."
 1030  PRINT  AT 6,0;"YOU HAVE FIVE SHIPS TO DESTROY   THE ALIEN AS MANY TIMES AS YOU  CAN.  WARNING - THE ALIEN GUNS  ARE INDESTRUCTIBLE."
 1040  PRINT  AT 11,0;"POINTS ARE AWARDED BASED ON      SKILL LEVEL AND THE DISTANCE    YOUR SHIP IS FROM THE LEFT EDGE OF THE SCREEN WHEN IT FIRES."
 1050  PRINT  AT 16,0;"DESTROYING THE ALIEN IS WORTH    FIFTY TIMES THE POINTS AWARDED  FOR DESTROYING A BLOCK OF HIS   FORTRESS."
 1060  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."
 
@WAIT_KEY2:
 1070  IF  INKEY$ ="" THEN  GOTO @WAIT_KEY2

 1080  CLS 
 1090  PRINT  AT 0,3;"YOUR SHIPS CONTROL PANEL:"
 1100  PRINT  AT 3,2;"\:'\''\''\''\''\''\''\''\''\''\''\':    \:'\''\''\''\''\''\''\''\''\''\''\':    \: 1 2 3 4 5 \ :    \: 6 7 8 9 0 \ :    \:  Q W E R T\ :    \:  Y U I O P\ :    \:.\..\..\..\..\..\..\..\..\..\..\.:    \:.\..\..\..\..\..\..\..\..\..\..\.:    MOVE SHIP UP    FIRE TORPEDO"
 1110  PRINT  AT 11,0;"\:'\''\''\''\''\''\''\''\''\''\':  \:'\''\''\''\''\''\''\': \:'\''\''\''\''\''\''\''\''\':\: A S D F G\ :  \:  H J K\ : \:  L ENTER\ :\:  Z X C V \ :  \: B N M \ : \: . SPACE \ :\:.\..\..\..\..\..\..\..\..\..\.:  \:.\..\..\..\..\..\..\.: \:.\..\..\..\..\..\..\..\..\.: MOVE SHIP     MOVE      MOVE      DOWN        SHIP      SHIP                  LEFT      RIGHT"
 1120  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."
 
@WAIT_KEY3:
 1130  IF  INKEY$ ="" THEN  GOTO @WAIT_KEY3

 1140  RETURN 

@INSERT_LEVEL:
 2000  CLS 
 2010  PRINT  AT 2,1;"SKILL LEVELS:"
 2020  PRINT  AT 4,15;"1 = SLOW"
 2030  PRINT  AT 6,15;"2 = AVERAGE"
 2040  PRINT  AT 8,15;"3 = FAST"
 2050  PRINT  AT 10,15;"4 = VERY FAST"
 2060  PRINT  AT 12,15;"5 = SUICIDAL"
 2070  PRINT  AT 16,6;"PRESS SKILL LEVEL..."
 
@WAIT_LEVEL:
 2080  IF  INKEY$ ="" THEN  GOTO @WAIT_LEVEL
 2090  LET V= CODE  INKEY$ -28
 2095  IF V=0 THEN POKE &CHEAT, 1
 2100  IF V<1 OR V>5 THEN  GOTO @WAIT_LEVEL
 
# V has level 1 to 5
 2105  CLS 

# store 256*(5-level)+1 at DELAY_LEVEL
 2110  POKE &LEVEL_DELAY,1
 2120  POKE &LEVEL_DELAY+1,5-V

 2130  LET F=50*V
 2150  FOR N=1 TO 5
 2160  LET A=&score_multiply-1+N
 2170  POKE A,0                     # 0=nop
 2180  IF N<=V THEN  POKE A,25      # 25=add hl,de
 2190  NEXT N
 2200  RETURN 

 3000  SAVE "ATTAC%K"
 3100  GOTO 30

#AUTOSTART=1
#FAST=0
