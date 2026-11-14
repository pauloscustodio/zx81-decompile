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

#[$407D]
    1  REM 
#ASM [$4082]

; --------------------------------------------------------------------------------
; Fortress drawing
; --------------------------------------------------------------------------------


FORTRESS1:                              ;[$4082]
        defm    "  \::\:: \::\::\::  \::\:: \::      \:: \::\::\::\::\:: \::\:: \:: \::\::\::\::    \::\::\::\:: \::\:: \::\::  \::\::  "
                                        ;[$4082]

SAVE_FORTRESS1:                         ;[$40BB]
        defm    "\::"                   ;[$40BB]

FORTRESS2:                              ;[$40BC]
        defm    "\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##\##  \##\##\##\## \## \##\##\##\##\##\##\##\## \##\##\##\##\##\##\##\##\## \##\## \::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::\::"
                                        ;[$40BC]

SAVE_FORTRESS2:                         ;[$4117]
        defm    "\::"                   ;[$4117]

addr_fortress:                          ;[$4118]
        defw    $40EE                   ;[$4118]

addr_fortress_save:                     ;[$411A]
        defw    SAVE_FORTRESS2          ;[$411A]

fortress_col:                           ;[$411C]
        defb    $13                     ;[$411C]

fortress_row:                           ;[$411D]
        defb    $02                     ;[$411D]

fortress_rows:                          ;[$411E]
        defb    $10                     ;[$411E]

fortress_cols:                          ;[$411F]
        defb    $05                     ;[$411F]

fortress_blocks:                        ;[$4120]
        defb    $29                     ;[$4120]

DEAD01:                                 ;[$4121]
        defb    $00, $00, $00, $00, $00 ;[$4121]

#ENDASM [$4126]

#[$4127]
    2  REM 
#ASM [$412C]

; --------------------------------------------------------------------------------
; Draw fortress at fortress_row/fortress_col
; --------------------------------------------------------------------------------


DRAW_FORTRESS:                          ;[$412C]
        ld      hl, SAVE_FORTRESS1      ;[$412C]
        ld      (addr_fortress_save), hl;[$412F]
        ld      hl, FORTRESS1           ;[$4132]
        ld      (addr_fortress), hl     ;[$4135]
        ld      a, $14                  ;[$4138]; 20 rows
        ld      (fortress_rows), a      ;[$413A]
        ld      a, $09                  ;[$413D]; 9 columns
        ld      (fortress_cols), a      ;[$413F]
        ld      a, $39                  ;[$4142]; 57 blocks
        ld      (fortress_blocks), a    ;[$4144]
        call    rotate_outer_layer      ;[$4147]
        call    rotate_inner_layer      ;[$414A]
        call    rotate_inner_layer      ;[$414D]
        dec     b                       ;[$4150]
        dec     b                       ;[$4151]
        dec     c                       ;[$4152]
        dec     c                       ;[$4153]
        ld      (fortress_col), bc      ;[$4154]
        jp      return                  ;[$4158]


rotate_inner_layer:                     ;[$415B]
        inc     b                       ;[$415B]
        inc     c                       ;[$415C]
        ld      (fortress_col), bc      ;[$415D]
        ld      (addr_fortress), de     ;[$4161]
        ld      a, (fortress_blocks)    ;[$4165]
        sub     $08                     ;[$4168]
        ld      (fortress_blocks), a    ;[$416A]
        ld      h, $00                  ;[$416D]
        ld      l, a                    ;[$416F]
        add     hl, de                  ;[$4170]
        ld      (addr_fortress_save), hl;[$4171]
        ld      a, (fortress_rows)      ;[$4174]
        sub     $02                     ;[$4177]
        ld      (fortress_rows), a      ;[$4179]
        ld      a, (fortress_cols)      ;[$417C]
        sub     $02                     ;[$417F]
        ld      (fortress_cols), a      ;[$4181]
        nop                             ;[$4184]
        nop                             ;[$4185]
        nop                             ;[$4186]
        call    rotate_outer_layer      ;[$4187]

return:                                 ;[$418A]
        ret                             ;[$418A]


#ENDASM [$418B]

#[$418C]
    3  REM 
#ASM [$4191]

rotate_outer_layer:                     ;[$4191]
        ld      de, (addr_fortress_save);[$4191]; point to save block
        ld      a, (de)                 ;[$4195]; get block to be saved to A
        ld      hl, (addr_fortress_save);[$4196]; get address of save
        dec     hl                      ;[$4199]; minus 1=last block
        ld      b, $00                  ;[$419A]; BC will be number of blocks
        push    af                      ;[$419C]; save end block
        ld      a, (fortress_blocks)    ;[$419D]; get number of blocks
        ld      c, a                    ;[$41A0]; to BC
        pop     af                      ;[$41A1]; A is block to be saved from end
        lddr                            ;[$41A2]; move all blocks 1 step
        ld      hl, (addr_fortress)     ;[$41A4]; get first address
        ld      (hl), a                 ;[$41A7]; and store saved char
        push    hl                      ;[$41A8]; save address of fortress
        ld      bc, (fortress_col)      ;[$41A9]; coords of fortress to BC
        call    SCR_POS                 ;[$41AD]; screen address to HL
        ex      de, hl                  ;[$41B0]; screen address to DE
        pop     hl                      ;[$41B1]; HL=blocks address
        push    bc                      ;[$41B2]; save coords
        ld      a, (fortress_rows)      ;[$41B3]; A=number of rows
        ld      bc, $0020               ;[$41B6]; distance between rows

rot_blocks_left_column:                 ;[$41B9]
        ldi                             ;[$41B9]; copy one
        inc     bc                      ;[$41BB]; restore BC
        dec     a                       ;[$41BC]; count rows
        cp      $00                     ;[$41BD]
        jr      z, rot_block_next1      ;[$41BF]; jump forward if end of rows
        ex      de, hl                  ;[$41C1]
        add     hl, bc                  ;[$41C2]; move to next row
        ex      de, hl                  ;[$41C3]
        jr      rot_blocks_left_column  ;[$41C4]


rot_block_next1:                        ;[$41C6]
        ld      bc, (fortress_cols)     ;[$41C6]
        ld      b, $00                  ;[$41CA]; BC = number of cols
        ldir                            ;[$41CC]; copy bottom row
        ld      a, (fortress_rows)      ;[$41CE]; A = number of rows
        ld      bc, $0022               ;[$41D1]

rot_blocks_right_column:                ;[$41D4]
        ldi                             ;[$41D4]; copy block
        inc     bc                      ;[$41D6]; restore BC
        dec     a                       ;[$41D7]; count rows
        cp      $00                     ;[$41D8]
        jr      z, rot_block_next2      ;[$41DA]; jump forward if finished
        ex      de, hl                  ;[$41DC]
        and     a                       ;[$41DD]
        sbc     hl, bc                  ;[$41DE]; move to previous row
        ex      de, hl                  ;[$41E0]
        jr      rot_blocks_right_column ;[$41E1]


rot_block_next2:                        ;[$41E3]
        ld      a, (fortress_cols)      ;[$41E3]; A = number of columns

rot_blocks_top_row:                     ;[$41E6]
        dec     de                      ;[$41E6]
        dec     de                      ;[$41E7]
        ldi                             ;[$41E8]; copy one block
        dec     a                       ;[$41EA]
        cp      $00                     ;[$41EB]
        jr      nz, rot_blocks_top_row  ;[$41ED]; jump back if not finished
        pop     bc                      ;[$41EF]
        ex      de, hl                  ;[$41F0]; DE=blocks, HL=screen
        ret                             ;[$41F1]


DEAD02:                                 ;[$41F2]
        ld      a, $F7                  ;[$41F2]
        cp      h                       ;[$41F4]
        jr      z, DEAD30               ;[$41F5]
        ld      a, $EF                  ;[$41F7]
        cp      h                       ;[$41F9]
        jr      z, DEAD30               ;[$41FA]
        ld      a, $DF                  ;[$41FC]
        cp      h                       ;[$41FE]
        jr      z, DEAD30               ;[$41FF]
        jp      save_ship_pos           ;[$4201]


DEAD30:                                 ;[$4204]
        xor     a                       ;[$4204]
        cp      c                       ;[$4205]
        jp      z, save_ship_pos        ;[$4206]
        jp      DEAD31                  ;[$4209]


#ENDASM [$420C]

#[$420D]
    4  REM 
#ASM [$4212]

; --------------------------------------------------------------------------------
; Compute screen position of row-col in BC to HL 
; Preserves all other registers
; --------------------------------------------------------------------------------


SCR_POS:                                ;[$4212]
        push    de                      ;[$4212]; save registers
        push    af                      ;[$4213]
        ld      hl, (D_FILE)            ;[$4214]
        inc     hl                      ;[$4217]
        xor     a                       ;[$4218]
        ld      de, $0021               ;[$4219]

SCR_POS_next_row:                       ;[$421C]
        cp      b                       ;[$421C]
        jr      z, SCR_POS_row_found    ;[$421D]
        add     hl, de                  ;[$421F]
        inc     a                       ;[$4220]
        jr      SCR_POS_next_row        ;[$4221]


SCR_POS_row_found:                      ;[$4223]
        ld      a, c                    ;[$4223]
        ld      e, $00                  ;[$4224]

SCR_POS_next_col:                       ;[$4226]
        cp      e                       ;[$4226]
        jr      z, SCR_POS_col_found    ;[$4227]
        inc     e                       ;[$4229]
        inc     hl                      ;[$422A]
        jr      SCR_POS_next_col        ;[$422B]


SCR_POS_col_found:                      ;[$422D]
        pop     af                      ;[$422D]
        pop     de                      ;[$422E]
        ret                             ;[$422F]


DEAD03:                                 ;[$4230]
        defb    $00, $00                ;[$4230]

#ENDASM [$4232]

#[$4233]
    5  REM 
#ASM [$4238]

erase_hit_block_from_fortress:          ;[$4238]
        ld      hl, FORTRESS1           ;[$4238]
        ld      de, (fortress_col)      ;[$423B]
        push    de                      ;[$423F]
        ld      a, b                    ;[$4240]
        sub     d                       ;[$4241]
        ld      d, $00                  ;[$4242]
        ld      e, a                    ;[$4244]
        add     hl, de                  ;[$4245]
        pop     de                      ;[$4246]
        ld      a, c                    ;[$4247]
        sub     e                       ;[$4248]
        cp      $01                     ;[$4249]
        jr      z, L4252                ;[$424B]
        jr      nc, L425C               ;[$424D]
        ld      (hl), $00               ;[$424F]
        ret                             ;[$4251]


L4252:                                  ;[$4252]
        ld      d, $00                  ;[$4252]
        ld      a, $39                  ;[$4254]
        nop                             ;[$4256]
        ld      e, a                    ;[$4257]
        add     hl, de                  ;[$4258]
        ld      (hl), $00               ;[$4259]
        ret                             ;[$425B]


L425C:                                  ;[$425C]
        ld      d, $00                  ;[$425C]
        ld      a, $39                  ;[$425E]
        nop                             ;[$4260]
        add     a, a                    ;[$4261]
        sub     $08                     ;[$4262]
        ld      e, a                    ;[$4264]
        add     hl, de                  ;[$4265]
        ld      (hl), $00               ;[$4266]
        ret                             ;[$4268]


DEAD04:                                 ;[$4269]
        defb    $00, $00, $00, $00, $00 ;[$4269]

#ENDASM [$426E]

#[$426F]
    6  REM 
#ASM [$4274]

alien_col:                              ;[$4274]
        defb    $16                     ;[$4274]

alien_row:                              ;[$4275]
        defb    $08                     ;[$4275]

DEAD05:                                 ;[$4276]
        defb    $00                     ;[$4276]

ALIEN:                                  ;[$4277]
        defm    "     "                 ;[$4277]
        defm    "\ .\:'\::\':\. "       ;[$427C]
        defm    " \':\''\:' "           ;[$4281]
        defm    " \ '\::\'  "           ;[$4286]
        defm    " \:'\::\': "           ;[$428B]
        defm    " \ .\::\.  "           ;[$4290]
        defm    " \.: \:. "             ;[$4295]
        defm    "     "                 ;[$429A]

; --------------------------------------------------------------------------------
; Draw alien at fortress_row/fortress_col
; --------------------------------------------------------------------------------


DRAW_ALIEN:                             ;[$429F]
        ld      hl, (fortress_col)      ;[$429F]
        ld      a, l                    ;[$42A2]
        add     a, $03                  ;[$42A3]
        ld      (alien_col), a          ;[$42A5]; fortress column+3 to alien column
        ld      bc, (alien_col)         ;[$42A8]
        push    bc                      ;[$42AC]
        ld      a, (alien_direction)    ;[$42AD]
        cp      $00                     ;[$42B0]
        jr      z, alien_downwards      ;[$42B2]
        dec     b                       ;[$42B4]
        call    draw_alien_at_BC        ;[$42B5]
        pop     bc                      ;[$42B8]
        ld      a, b                    ;[$42B9]
        cp      $06                     ;[$42BA]
        ret     nz                      ;[$42BC]
        xor     a                       ;[$42BD]

store_alien_pos:                        ;[$42BE]
        ld      (alien_direction), a    ;[$42BE]
        ret                             ;[$42C1]


alien_downwards:                        ;[$42C2]
        inc     b                       ;[$42C2]
        call    draw_alien_at_BC        ;[$42C3]
        pop     bc                      ;[$42C6]
        ld      a, b                    ;[$42C7]
        cp      $0A                     ;[$42C8]
        ret     nz                      ;[$42CA]
        ld      a, $01                  ;[$42CB]
        jr      store_alien_pos         ;[$42CD]


DEAD06:                                 ;[$42CF]
        defb    $00, $00                ;[$42CF]

draw_alien_at_BC:                       ;[$42D1]
        ld      (alien_col), bc         ;[$42D1]
        call    SCR_POS                 ;[$42D5]
        ex      de, hl                  ;[$42D8]; DE=screen address
        ld      hl, ALIEN               ;[$42D9]; source of alien
        ld      a, $08                  ;[$42DC]; 8 rows

alien_each_row:                         ;[$42DE]
        ld      bc, $0005               ;[$42DE]; 4 cols
        ldir                            ;[$42E1]; copy one row
        dec     a                       ;[$42E3]
        cp      $00                     ;[$42E4]
        ret     z                       ;[$42E6]; exit afyet 8 rows
        ex      de, hl                  ;[$42E7]
        ld      bc, $001C               ;[$42E8]; advance to next row
        add     hl, bc                  ;[$42EB]
        ex      de, hl                  ;[$42EC]
        jr      alien_each_row          ;[$42ED]


alien_direction:                        ;[$42EF]
        defb    $01                     ;[$42EF]

DEAD07:                                 ;[$42F0]
        defb    $00, $00, $00, $00      ;[$42F0]

#ENDASM [$42F4]

#[$42F5]
    7  REM 
#ASM [$42FA]

SHIP1:                                  ;[$42FA]
        defm    "\.:\::\::\::\:."       ;[$42FA]
        defm    "\:.\:.\.:\..\::"       ;[$42FF]
        defm    " \::"                  ;[$4304]

SHIP1_nr:                               ;[$4306]
        defm    "\::"                   ;[$4306]
        defm    "\:: "                  ;[$4307]
        defm    "\:'\:'\':\''\::"       ;[$4309]
        defm    "\':\::\::\::\:'"       ;[$430E]

SHIP2:                                  ;[$4313]
        defm    "\.:\::\::\::\:."       ;[$4313]
        defm    "\:.\.:\..\:.\.:"       ;[$4318]
        defm    " \::"                  ;[$431D]

SHIP2_nr:                               ;[$431F]
        defm    "\::"                   ;[$431F]
        defm    "\:: "                  ;[$4320]
        defm    "\:'\':\''\:'\':"       ;[$4322]
        defm    "\':\::\::\::\:'"       ;[$4327]

SHIP3:                                  ;[$432C]
        defm    "\.:\::\::\::\:."       ;[$432C]
        defm    "\::\..\:.\.:\.:"       ;[$4331]
        defm    " \::"                  ;[$4336]

SHIP3_nr:                               ;[$4338]
        defm    "\::"                   ;[$4338]
        defm    "\:: "                  ;[$4339]
        defm    "\::\''\:'\':\':"       ;[$433B]
        defm    "\':\::\::\::\:'"       ;[$4340]

which_ship:                             ;[$4345]
        defb    $03                     ;[$4345]

ship_col:                               ;[$4346]
        defb    $00                     ;[$4346]

ship_row:                               ;[$4347]
        defb    $09                     ;[$4347]

; --------------------------------------------------------------------------------
; Draw ship at row-col ship_row/ship_col
; --------------------------------------------------------------------------------


DRAW_SHIP:                              ;[$4348]
        ld      bc, (ship_col)          ;[$4348]

; --------------------------------------------------------------------------------
; Draw ship at row-col BC
; --------------------------------------------------------------------------------


DRAW_SHIP_BC:                           ;[$434C]
        call    SCR_POS                 ;[$434C]; HL=screen position                                    
        ex      de, hl                  ;[$434F]; DE=screen position
        ld      hl, which_ship          ;[$4350]
        ld      a, $01                  ;[$4353]
        cp      (hl)                    ;[$4355]
        jr      nz, not_ship1           ;[$4356]
        inc     a                       ;[$4358]
        ld      (which_ship), a         ;[$4359]
        ld      hl, SHIP1               ;[$435C]
        jr      draw_ship_hl            ;[$435F]


not_ship1:                              ;[$4361]
        ld      a, $02                  ;[$4361]
        cp      (hl)                    ;[$4363]
        jr      nz, not_ship2           ;[$4364]
        inc     a                       ;[$4366]
        ld      (which_ship), a         ;[$4367]
        ld      hl, SHIP2               ;[$436A]
        jr      draw_ship_hl            ;[$436D]


not_ship2:                              ;[$436F]
        ld      a, $01                  ;[$436F]
        ld      (which_ship), a         ;[$4371]
        ld      hl, SHIP3               ;[$4374]

; DE is screen address, HL is ship


draw_ship_hl:                           ;[$4377]
        ld      a, $05                  ;[$4377]; 5 rows

draw_ship_row:                          ;[$4379]
        ld      bc, $0005               ;[$4379]; 5 columns
        ldir                            ;[$437C]; copy one row
        dec     a                       ;[$437E]
        cp      $00                     ;[$437F]
        ret     z                       ;[$4381]; return when 5 rows are printed
        ex      de, hl                  ;[$4382]
        ld      bc, $001C               ;[$4383]; DE+=33-5=28
        add     hl, bc                  ;[$4386]
        ex      de, hl                  ;[$4387]
        jr      draw_ship_row           ;[$4388]


DEAD08:                                 ;[$438A]
        defb    $00, $00, $00, $00, $00 ;[$438A]

#ENDASM [$438F]

#[$4390]
    8  REM 
#ASM [$4395]

DEAD09:                                 ;[$4395]
        call    KEYBOARD                ;[$4395]
        ld      bc, (ship_col)          ;[$4398]
        ld      a, $FF                  ;[$439C]
        cp      l                       ;[$439E]
        jr      z, save_ship_pos        ;[$439F]
        dec     a                       ;[$43A1]
        cp      l                       ;[$43A2]
        jr      z, MOVE_SHIP_DOWN       ;[$43A3]
        dec     a                       ;[$43A5]
        cp      l                       ;[$43A6]
        jr      z, MOVE_SHIP_DOWN       ;[$43A7]
        dec     a                       ;[$43A9]
        dec     a                       ;[$43AA]
        cp      l                       ;[$43AB]
        jr      z, MOVE_SHIP_UP         ;[$43AC]
        ld      a, $F7                  ;[$43AE]
        cp      l                       ;[$43B0]
        jr      z, MOVE_SHIP_UP         ;[$43B1]
        ld      a, $EF                  ;[$43B3]
        cp      l                       ;[$43B5]
        jr      z, DEAD10               ;[$43B6]
        ld      a, $DF                  ;[$43B8]
        cp      l                       ;[$43BA]
        jr      z, DEAD10               ;[$43BB]
        ld      a, $F7                  ;[$43BD]
        sub     h                       ;[$43BF]
        jr      nc, MOVE_SHIP_LEFT      ;[$43C0]

; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_RIGHT:                        ;[$43C2]
        ld      a, (fortress_col)       ;[$43C2]; A=fortress column
        sub     $08                     ;[$43C5]; -8 (5 for ship + 2 for cannons + 1)
        cp      c                       ;[$43C7]
        jr      z, save_ship_pos        ;[$43C8]; reached limit
        call    DELETE_SHIP             ;[$43CA]
        inc     c                       ;[$43CD]; move right
        jr      save_ship_pos           ;[$43CE]


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_DOWN:                         ;[$43D0]
        ld      a, $10                  ;[$43D0]
        cp      b                       ;[$43D2]
        jr      z, save_ship_pos        ;[$43D3]; reached limit
        call    DELETE_SHIP             ;[$43D5]
        inc     b                       ;[$43D8]; move down
        jr      save_ship_pos           ;[$43D9]


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_UP:                           ;[$43DB]
        ld      a, $03                  ;[$43DB]
        cp      b                       ;[$43DD]
        jr      z, save_ship_pos        ;[$43DE]; reached limit
        call    DELETE_SHIP             ;[$43E0]
        dec     b                       ;[$43E3]; move up
        jr      save_ship_pos           ;[$43E4]


; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


MOVE_SHIP_LEFT:                         ;[$43E6]
        xor     a                       ;[$43E6]
        cp      c                       ;[$43E7]
        jr      z, save_ship_pos        ;[$43E8]; reached limit

DEAD31:                                 ;[$43EA]
        call    DELETE_SHIP             ;[$43EA]
        dec     c                       ;[$43ED]; move right
        jr      save_ship_pos           ;[$43EE]


DEAD10:                                 ;[$43F0]
        push    bc                      ;[$43F0]
        call    FIRE_TORPEDO            ;[$43F1]
        pop     bc                      ;[$43F4]

save_ship_pos:                          ;[$43F5]
        ld      (ship_col), bc          ;[$43F5]
        call    DRAW_SHIP               ;[$43F9]
        ret                             ;[$43FC]


DEAD11:                                 ;[$43FD]
        defb    $00, $00                ;[$43FD]

#ENDASM [$43FF]

#[$4400]
    9  REM 
#ASM [$4405]

; --------------------------------------------------------------------------------
; Delete ship before moving it
; --------------------------------------------------------------------------------


DELETE_SHIP:                            ;[$4405]
        push    bc                      ;[$4405]; save coords
        ld      bc, (ship_col)          ;[$4406]
        call    SCR_POS                 ;[$440A]; HL=screen address
        ld      a, $05                  ;[$440D]; 5 rows
        ld      de, $001C               ;[$440F]; distance in screen between rows

delete_row:                             ;[$4412]
        ld      b, $05                  ;[$4412]; 5 cols

delete_col:                             ;[$4414]
        ld      (hl), $00               ;[$4414]; delete char
        inc     hl                      ;[$4416]
        djnz    delete_col              ;[$4417]
        add     hl, de                  ;[$4419]; next row
        dec     a                       ;[$441A]
        cp      $00                     ;[$441B]
        jr      nz, delete_row          ;[$441D]
        pop     bc                      ;[$441F]; restore coords
        ret                             ;[$4420]


DEAD12:                                 ;[$4421]
        defb    $00, $00, $00, $00, $00 ;[$4421]

#ENDASM [$4426]

#[$4427]
   10  REM 
#ASM [$442C]

torpedo_running:                        ;[$442C]
        defb    $00                     ;[$442C]

torpedo_col:                            ;[$442D]
        defb    $11                     ;[$442D]

torpedo_row:                            ;[$442E]
        defb    $12                     ;[$442E]

topedo_pos0:                            ;[$442F]
        defw    $120A                   ;[$442F]

; --------------------------------------------------------------------------------
; Called in response to key, BC has ship coords
; --------------------------------------------------------------------------------


FIRE_TORPEDO:                           ;[$4431]
        ld      a, (torpedo_running)    ;[$4431]; is a torpedo on the way?
        cp      $01                     ;[$4434]
        ret     z                       ;[$4436]; return if yes
        ld      hl, (ship_col)          ;[$4437]
        inc     h                       ;[$443A]
        inc     h                       ;[$443B]
        inc     l                       ;[$443C]
        inc     l                       ;[$443D]
        inc     l                       ;[$443E]
        inc     l                       ;[$443F]
        ld      (torpedo_col), hl       ;[$4440]; torpedo coord is mouth of ship
        ld      (topedo_pos0), hl       ;[$4443]; make a copy of the cooords - why?
        ld      b, h                    ;[$4446]
        ld      c, l                    ;[$4447]
        call    SCR_POS                 ;[$4448]; HL=screen position
        ld      (hl), $34               ;[$444B]; draw torpedo
        ld      a, $01                  ;[$444D]
        ld      (torpedo_running), a    ;[$444F]; signal torpedo on the way
        ret                             ;[$4452]


DEAD13:                                 ;[$4453]
        defb    $00, $00, $00, $00, $00 ;[$4453]

#ENDASM [$4458]

#[$4459]
   11  REM 
#ASM [$445E]

WALL_HIT:                               ;[$445E]
        defb    $00                     ;[$445E]

ALIEN_HIT:                              ;[$445F]
        defb    $00                     ;[$445F]

; --------------------------------------------------------------------------------
; moves torpedo if it has been created
; --------------------------------------------------------------------------------


MOVE_TORPEDO:                           ;[$4460]
        xor     a                       ;[$4460]
        ld      (WALL_HIT), a           ;[$4461]
        ld      (ALIEN_HIT), a          ;[$4464]
        ld      a, (torpedo_running)    ;[$4467]
        cp      $00                     ;[$446A]
        ret     z                       ;[$446C]; torpedo was not fired, return
        ld      bc, (torpedo_col)       ;[$446D]
        call    SCR_POS                 ;[$4471]; HL=torpedo coord
        ld      a, $34                  ;[$4474]
        cp      (hl)                    ;[$4476]; is there a torpedo?
        jr      nz, topedo_erased       ;[$4477]; jump forward if not
        ld      (hl), $00               ;[$4479]; erase it

topedo_erased:                          ;[$447B]
        ld      de, (fortress_col)      ;[$447B]
        ld      a, e                    ;[$447F]
        dec     a                       ;[$4480]
        sub     c                       ;[$4481]
        jr      z, torpedo_hit          ;[$4482]; torpedo col == fortress col -1
        jr      c, torpedo_hit          ;[$4484]; torpedo col >  fortress col -1
        inc     hl                      ;[$4486]; move torpedo

draw_torpedo_store_pos:                 ;[$4487]
        ld      (hl), $34               ;[$4487]; draw it
        inc     c                       ;[$4489]
        ld      (torpedo_col), bc       ;[$448A]; store new coords
        ret                             ;[$448E]


torpedo_hit:                            ;[$448F]
        inc     hl                      ;[$448F]
        ld      a, e                    ;[$4490]
        add     a, $04                  ;[$4491]; A=fortress-col+4 (alien)
        sub     c                       ;[$4493]
        jr      z, stop_torpedo         ;[$4494]; inside alien - stop torpedo
        xor     a                       ;[$4496]
        cp      (hl)                    ;[$4497]; hit space - continue
        jr      z, draw_torpedo_store_pos
                                        ;[$4498]
        ld      a, $80                  ;[$449A]
        cp      (hl)                    ;[$449C]; hit black border
        jr      z, hit_fortress         ;[$449D]
        ld      a, $08                  ;[$449F]
        cp      (hl)                    ;[$44A1]; hit gray border
        jr      z, hit_fortress         ;[$44A2]
        ld      a, $01                  ;[$44A4]
        ld      (ALIEN_HIT), a          ;[$44A6]
        jr      stop_torpedo            ;[$44A9]


hit_fortress:                           ;[$44AB]
        inc     c                       ;[$44AB]
        ld      (hl), $00               ;[$44AC]; erase block
        call    erase_hit_block_from_fortress
                                        ;[$44AE]
        ld      a, $01                  ;[$44B1]
        ld      (WALL_HIT), a           ;[$44B3]

stop_torpedo:                           ;[$44B6]
        xor     a                       ;[$44B6]
        ld      (torpedo_running), a    ;[$44B7]
        ret                             ;[$44BA]


DEAD14:                                 ;[$44BB]
        defb    $00, $00, $00, $00, $00, $00, $00, $00, $00
                                        ;[$44BB]

#ENDASM [$44C4]

#[$44C5]
   12  REM 
#ASM [$44CA]

BLOCK1:                                 ;[$44CA]
        defm    " \::\::"               ;[$44CA]; WBB block

save_block1:                            ;[$44CD]
        defm    "\::"                   ;[$44CD]

DEAD20:                                 ;[$44CE]
        defb    $00, $00, $00           ;[$44CE]

; --------------------------------------------------------------------------------
; Draw rotating border and ship
; --------------------------------------------------------------------------------


INTRO_SCREEN:                           ;[$44D1]
        ld      bc, $0301               ;[$44D1]; row 3, col 1
        call    SCR_POS                 ;[$44D4]; HL=screen address
        ex      de, hl                  ;[$44D7]; DE=screen address
        ld      b, $0B                  ;[$44D8]; 10 times+1 for one LDI decrementing B

draw_top_bar:                           ;[$44DA]
        ld      hl, BLOCK1              ;[$44DA]
        ldi                             ;[$44DD]; copy block to screen
        ldi                             ;[$44DF]; three times
        ldi                             ;[$44E1]
        djnz    draw_top_bar            ;[$44E3]; 30 chars copied (LDI decs BC, DJNZ decs B)
        ld      a, $05                  ;[$44E5]; copy 5 blocks of 3 chars

draw_right_bar:                         ;[$44E7]
        ld      b, $03                  ;[$44E7]
        ld      hl, BLOCK1              ;[$44E9]

each_right_bar_block:                   ;[$44EC]
        push    bc                      ;[$44EC]; save BC
        ld      bc, $0020               ;[$44ED]; add 32 to DE, LDI with add 1 more
        ex      de, hl                  ;[$44F0]
        add     hl, bc                  ;[$44F1]; HL+=32
        pop     bc                      ;[$44F2]
        ex      de, hl                  ;[$44F3]; DE+=32
        ldi                             ;[$44F4]; DE+=33
        djnz    each_right_bar_block    ;[$44F6]; copy 3 blocks
        dec     a                       ;[$44F8]; times 5
        cp      $00                     ;[$44F9]; NOTE: not needed
        jr      nz, draw_right_bar      ;[$44FB]
        ld      bc, $0020               ;[$44FD]; add 32 to DE
        ex      de, hl                  ;[$4500]
        add     hl, bc                  ;[$4501]
        ex      de, hl                  ;[$4502]; DE+=32
        ld      b, $0A                  ;[$4503]; 10 blocks of 3 = 30

draw_bottom_bar:                        ;[$4505]
        ld      hl, BLOCK1              ;[$4505]
        ld      a, (hl)                 ;[$4508]; copy 3 chars
        ld      (de), a                 ;[$4509]
        inc     hl                      ;[$450A]
        dec     de                      ;[$450B]
        ld      a, (hl)                 ;[$450C]
        ld      (de), a                 ;[$450D]
        inc     hl                      ;[$450E]
        dec     de                      ;[$450F]
        ld      a, (hl)                 ;[$4510]
        ld      (de), a                 ;[$4511]
        nop                             ;[$4512]; NOTE: not needed
        dec     de                      ;[$4513]
        djnz    draw_bottom_bar         ;[$4514]; repeat 10 times
        ld      bc, $0020               ;[$4516]; subtract 32 from DE
        ex      de, hl                  ;[$4519]
        and     a                       ;[$451A]
        sbc     hl, bc                  ;[$451B]; DE-=32
        ex      de, hl                  ;[$451D]
        ld      a, $05                  ;[$451E]; copy 5 blocks of 3 chars

draw_left_bar:                          ;[$4520]
        ld      b, $03                  ;[$4520]
        ld      hl, BLOCK1              ;[$4522]

each_left_bar_block:                    ;[$4525]
        push    af                      ;[$4525]; copy one char
        ld      a, (hl)                 ;[$4526]
        ld      (de), a                 ;[$4527]
        pop     af                      ;[$4528]
        push    bc                      ;[$4529]
        ld      bc, $0021               ;[$452A]; subtract 33 from address
        ex      de, hl                  ;[$452D]
        and     a                       ;[$452E]
        sbc     hl, bc                  ;[$452F]; DE-=33
        pop     bc                      ;[$4531]
        ex      de, hl                  ;[$4532]
        inc     hl                      ;[$4533]
        djnz    each_left_bar_block     ;[$4534]; copy 3 blocks
        dec     a                       ;[$4536]
        cp      $00                     ;[$4537]
        jr      nz, draw_left_bar       ;[$4539]; times 5

; rotate block

        ld      a, (BLOCK1)             ;[$453B]
        ld      (save_block1), a        ;[$453E]
        ld      a, (BLOCK1+1)           ;[$4541]
        ld      (BLOCK1), a             ;[$4544]
        ld      a, (BLOCK1+2)           ;[$4547]
        ld      (BLOCK1+1), a           ;[$454A]
        ld      a, (save_block1)        ;[$454D]
        ld      (BLOCK1+2), a           ;[$4550]
        ld      bc, $0B0D               ;[$4553]; line 11, column 13
        jp      DRAW_SHIP_BC            ;[$4556]


#ENDASM [$4559]

#[$455A]
   13  REM 
#ASM [$455F]

ROTATED_SHIP1:                          ;[$455F]
        defm    " \.:\':\:' "           ;[$455F]
        defm    "\.:\'.\:'\:.\.:"       ;[$4564]
        defm    "\:.\:' \.:\':"         ;[$4569]
        defm    "\:'\':\.:\'.\:'"       ;[$456E]
        defm    " \.:\:.\:' "           ;[$4573]

ROTATED_SHIP2:                          ;[$4578]
        defm    "\.:\:: \:'\:."         ;[$4578]
        defm    "\::\.:\::\:'\::"       ;[$457D]
        defm    "\::\.: \:'\::"         ;[$4582]
        defm    "\::\.:\::\:'\::"       ;[$4587]
        defm    "\':\.: \::\:'"         ;[$458C]

ROTATED_SHIP3:                          ;[$4591]
        defm    " \':\:'\:. "           ;[$4591]
        defm    "\:.\.:\':\.'\:."       ;[$4596]
        defm    "\:'\:. \':\.:"         ;[$459B]
        defm    "\':\.'\:.\:'\':"       ;[$45A0]
        defm    " \':\.:\:. "           ;[$45A5]

ROTATED_SHIP4:                          ;[$45AA]
        defm    "\.:\::\::\::\:."       ;[$45AA]
        defm    "\:.\:.\:.\:.\::"       ;[$45AF]
        defm    " \:: \:: "             ;[$45B4]
        defm    "\::\':\':\':\':"       ;[$45B9]
        defm    "\':\::\::\::\:'"       ;[$45BE]

rotate_count:                           ;[$45C3]
        defb    $04                     ;[$45C3]

ROTATE_SHIP:                            ;[$45C4]
        ld      de, $0019               ;[$45C4]; 5*5=25 bytes per ship
        ld      a, (rotate_count)       ;[$45C7]; counts 1 to 4
        inc     a                       ;[$45CA]
        cp      $05                     ;[$45CB]
        jr      nz, store_count         ;[$45CD]
        ld      a, $01                  ;[$45CF]

store_count:                            ;[$45D1]
        ld      (rotate_count), a       ;[$45D1]
        ld      hl, $4546               ;[$45D4]; address of ship1-25 bytes
        ld      b, a                    ;[$45D7]

add_25:                                 ;[$45D8]
        add     hl, de                  ;[$45D8]
        djnz    add_25                  ;[$45D9]
        push    hl                      ;[$45DB]; save source address of ship
        ld      bc, (ship_col)          ;[$45DC]; get ship position
        call    SCR_POS                 ;[$45E0]; HL = screen position
        ex      de, hl                  ;[$45E3]; DE = screen position
        pop     hl                      ;[$45E4]; HL = source of rotated ship
        jp      draw_ship_hl            ;[$45E5]


DEAD15:                                 ;[$45E8]
        defb    $00, $00, $00, $00, $00 ;[$45E8]

#ENDASM [$45ED]

#[$45EE]
   14  REM 
#ASM [$45F3]

torpedo_path:                           ;[$45F3]
        defm    "        O        "     ;[$45F3]

intro_torpedo_pos_lo:                   ;[$4604]
        defb    $FA                     ;[$4604]

intro_torpedo_pos_hi:                   ;[$4605]
        defb    $45                     ;[$4605]

; --------------------------------------------------------------------------------
; Fire intro torpedo
; --------------------------------------------------------------------------------


INTRO_FIRE_TORPEDO:                     ;[$4606]
        ld      hl, (intro_torpedo_pos_lo)
                                        ;[$4606]
        dec     hl                      ;[$4609]; move back one pos
        ld      bc, torpedo_path        ;[$460A]
        and     a                       ;[$460D]
        sbc     hl, bc                  ;[$460E]
        jr      nc, restore_torpedo_pos ;[$4610]
        ld      hl, $45FB               ;[$4612]; reached start, go back to end
        jr      draw_torpedo_path       ;[$4615]


restore_torpedo_pos:                    ;[$4617]
        add     hl, bc                  ;[$4617]

draw_torpedo_path:                      ;[$4618]
        push    hl                      ;[$4618]
        ld      (intro_torpedo_pos_lo), hl
                                        ;[$4619]
        ld      bc, $0D12               ;[$461C]; row 13, col 18
        call    SCR_POS                 ;[$461F]; screen pos in HL
        pop     de                      ;[$4622]
        ex      de, hl                  ;[$4623]; DE=screen, HL=torpedo path
        ld      bc, $0008               ;[$4624]; 1 torpedo and 7 blanks
        ldir                            ;[$4627]; draw
        ret                             ;[$4629]


#ENDASM [$462A]

#[$462B]
   15  REM 
#ASM [$4630]

; --------------------------------------------------------------------------------
; Draw the 4 cannons in front of the fortress
; --------------------------------------------------------------------------------


DRAW_CANONS:                            ;[$4630]
        ld      a, (fortress_col)       ;[$4630]
        sub     $02                     ;[$4633]; fortress column - 2
        ld      c, a                    ;[$4635]
        ld      b, $05                  ;[$4636]; row 5
        call    draw_cannon             ;[$4638]
        nop                             ;[$463B]
        call    draw_cannon             ;[$463C]
        call    draw_cannon             ;[$463F]
        nop                             ;[$4642]
        call    draw_cannon             ;[$4643]
        ret                             ;[$4646]


draw_cannon:                            ;[$4647]
        call    SCR_POS                 ;[$4647]; draw cannon at roc-col BC
        ld      (hl), $81               ;[$464A]; top part of cannon
        ld      de, $0021               ;[$464C]
        add     hl, de                  ;[$464F]; add 33
        ld      (hl), $02               ;[$4650]; bottom part of cannon
        inc     b                       ;[$4652]; leave 4 rows interval
        inc     b                       ;[$4653]
        inc     b                       ;[$4654]
        inc     b                       ;[$4655]
        ret                             ;[$4656]


DEAD16:                                 ;[$4657]
        defb    $00, $00, $00           ;[$4657]

#ENDASM [$465A]

#[$465B]
   16  REM 
#ASM [$4660]

bomb_col:                               ;[$4660]
        defb    $00                     ;[$4660]

bomb_row:                               ;[$4661]
        defb    $11                     ;[$4661]

bomb_running:                           ;[$4662]
        defb    $00                     ;[$4662]

; --------------------------------------------------------------------------------
; select bomb position from fortress and ship position
; --------------------------------------------------------------------------------


FIRE_BOMB:                              ;[$4663]
        ld      a, (bomb_running)       ;[$4663]
        cp      $01                     ;[$4666]
        ret     z                       ;[$4668]; exit if bomb is still running
        ld      a, (fortress_col)       ;[$4669]
        sub     $03                     ;[$466C]
        ld      l, a                    ;[$466E]; L=fortress-col - 3
        ld      a, (ship_row)           ;[$466F]
        ld      h, a                    ;[$4672]; H=ship row
        ld      a, $05                  ;[$4673]
        sub     h                       ;[$4675]
        jr      c, not_cannon1          ;[$4676]
        ld      h, $05                  ;[$4678]; fire from row 5
        jr      do_fire_bomb            ;[$467A]


not_cannon1:                            ;[$467C]
        ld      a, $09                  ;[$467C]
        sub     h                       ;[$467E]
        jr      c, not_cannon2          ;[$467F]
        ld      h, $09                  ;[$4681]; fire from row 9
        jr      do_fire_bomb            ;[$4683]


not_cannon2:                            ;[$4685]
        ld      a, $0D                  ;[$4685]
        sub     h                       ;[$4687]
        jr      c, not_cannon3          ;[$4688]
        ld      h, $0D                  ;[$468A]; fire from row 13
        jr      do_fire_bomb            ;[$468C]


not_cannon3:                            ;[$468E]
        ld      h, $11                  ;[$468E]; fire from row 17

do_fire_bomb:                           ;[$4690]
        ld      (bomb_col), hl          ;[$4690]
        ld      a, $01                  ;[$4693]
        ld      (bomb_running), a       ;[$4695]
        ret                             ;[$4698]


#ENDASM [$4699]

#[$469A]
   17  REM 
#ASM [$469F]

SHIP_HIT:                               ;[$469F]
        defb    $00                     ;[$469F]

DRAW_BOMB:                              ;[$46A0]
        xor     a                       ;[$46A0]
        ld      (SHIP_HIT), a           ;[$46A1]; clear ship-hit
        ld      bc, (bomb_col)          ;[$46A4]; get position of bomb
        call    SCR_POS                 ;[$46A8]; HL = screen address of bomb
        ld      (hl), $00               ;[$46AB]; delete bomb
        xor     a                       ;[$46AD]
        cp      c                       ;[$46AE]; compare column with zero
        jr      nz, move_bomb           ;[$46AF]; jump forward if not at col zero
        ld      (bomb_running), a       ;[$46B1]; at col zero, clear bomb-running
        ret                             ;[$46B4]


move_bomb:                              ;[$46B5]
        dec     c                       ;[$46B5]; move bomb left in column
        dec     hl                      ;[$46B6]; and screen address
        cp      (hl)                    ;[$46B7]; did we hit something?
        jr      z, bomb_no_hit          ;[$46B8]; jump forward if not
        ld      a, $34                  ;[$46BA]
        cp      (hl)                    ;[$46BC]; did we hit the torpedo?
        jr      nz, bomb_hit_ship       ;[$46BD]; jump if not, we hit the ship

bomb_no_hit:                            ;[$46BF]
        ld      (hl), $04               ;[$46BF]; draw bomb
        ld      (bomb_col), bc          ;[$46C1]
        ret                             ;[$46C5]


bomb_hit_ship:                          ;[$46C6]
        inc     (hl)                    ;[$46C6]; show rubish on hit-point
        ld      (bomb_hit_ship), a      ;[$46C7]; NOTE: should not be here
                                        ; writes a $34 on the code address, which is inc (hl)
        ld      a, $01                  ;[$46CA]
        ld      (SHIP_HIT), a           ;[$46CC]
        ret                             ;[$46CF]


DEAD17:                                 ;[$46D0]
        defb    $00, $00, $00, $00, $00 ;[$46D0]

#ENDASM [$46D5]

#[$46D6]
   18  REM 
#ASM [$46DB]

timer_fortress_move:                    ;[$46DB]
        defw    $0061                   ;[$46DB]; count times until fortress moves

; --------------------------------------------------------------------------------
; advance timer and move fortress left if it reaches $C8
; --------------------------------------------------------------------------------


CHECK_FORTRESS_MOVE:                    ;[$46DD]
        ld      hl, (timer_fortress_move)
                                        ;[$46DD]
        inc     hl                      ;[$46E0]

FORTRESS_MOVE_COUNT:                    ;[$46E1]
        ld      de, $00C8               ;[$46E1]; value is POKEd from BASIC
        and     a                       ;[$46E4]
        sbc     hl, de                  ;[$46E5]
        jr      z, do_fortress_move     ;[$46E7]
        add     hl, de                  ;[$46E9]
        ld      (timer_fortress_move), hl
                                        ;[$46EA]
        ret                             ;[$46ED]; not time yet to move


do_fortress_move:                       ;[$46EE]
        ld      hl, $0000               ;[$46EE]
        ld      (timer_fortress_move), hl
                                        ;[$46F1]; reset timer
        call    DELETE_TORPEDO          ;[$46F4]
        ld      bc, (fortress_col)      ;[$46F7]
        dec     c                       ;[$46FB]; move fortress one position left
        ld      (fortress_col), bc      ;[$46FC]
        dec     c                       ;[$4700]
        dec     c                       ;[$4701]
        call    SCR_POS                 ;[$4702]; minus 2=cannons column
        ld      a, $14                  ;[$4705]; 20 lines

move_next_row:                          ;[$4707]
        ld      bc, $000D               ;[$4707]; 13 chars = fortress+cannons
        ld      d, h                    ;[$470A]; destintion
        ld      e, l                    ;[$470B]
        inc     hl                      ;[$470C]; source=destination+1
        ldir                            ;[$470D]; move row
        dec     hl                      ;[$470F]
        ld      (hl), $00               ;[$4710]; delete char to the right
        ld      bc, $0014               ;[$4712]; move to next line
        add     hl, bc                  ;[$4715]
        dec     a                       ;[$4716]
        cp      $00                     ;[$4717]
        jr      nz, move_next_row       ;[$4719]; next row
        ret                             ;[$471B]


#ENDASM [$471C]

#[$471D]
   19  REM 
#ASM [$4722]

SCORE_lo:                               ;[$4722]
        defb    $38                     ;[$4722]

SCORE_hi:                               ;[$4723]
        defb    $04                     ;[$4723]

NUM_BUFFER:                             ;[$4724]
        defm    " 1080"                 ;[$4724]

N10000:                                 ;[$4729]
        defw    $2710                   ;[$4729]

N1000:                                  ;[$472B]
        defw    $03E8                   ;[$472B]

N100:                                   ;[$472D]
        defw    $0064                   ;[$472D]

N10:                                    ;[$472F]
        defw    $000A                   ;[$472F]

N1:                                     ;[$4731]
        defw    $0001                   ;[$4731]

PRINT_ACC:                              ;[$4733]
        defw    $0000                   ;[$4733]

HAVE_DIGITS:                            ;[$4735]
        defb    $01                     ;[$4735]

; Note: does not work for 0, outputs all blanks


SCORE_I2A:                              ;[$4736]
        xor     a                       ;[$4736]
        ld      (HAVE_DIGITS), a        ;[$4737]
        ld      hl, (SCORE_lo)          ;[$473A]; get SCORE
        ld      (PRINT_ACC), hl         ;[$473D]; put in number to print
        ld      bc, NUM_BUFFER          ;[$4740]; output buffer
        ld      hl, N10000              ;[$4743]; table of divisors
        push    hl                      ;[$4746]; save address of table

next_divisor:                           ;[$4747]
        xor     a                       ;[$4747]
        pop     hl                      ;[$4748]; HL=next divisor address
        ld      e, (hl)                 ;[$4749]
        inc     hl                      ;[$474A]
        ld      d, (hl)                 ;[$474B]; DE=next divisor
        dec     hl                      ;[$474C]; restore HL
        push    hl                      ;[$474D]; save HL

subtract_divisor:                       ;[$474E]
        ld      hl, (PRINT_ACC)         ;[$474E]; get accumulator
        and     a                       ;[$4751]
        sbc     hl, de                  ;[$4752]; subtract divisor
        ld      (PRINT_ACC), hl         ;[$4754]; store accumulator
        jr      c, subtraction_overflowed
                                        ;[$4757]; jump forward if overflowed
        inc     a                       ;[$4759]; count one more
        jr      subtract_divisor        ;[$475A]


subtraction_overflowed:                 ;[$475C]
        add     hl, de                  ;[$475C]; restore accumulator
        cp      $00                     ;[$475D]
        jr      nz, make_digit          ;[$475F]; jump forward if not zero
        ld      a, (HAVE_DIGITS)        ;[$4761]; if this digit is 0 and have_digits is 0, output a space
        cp      $00                     ;[$4764]
        ld      a, $00                  ;[$4766]
        jr      z, output_digit         ;[$4768]

make_digit:                             ;[$476A]
        add     a, $1C                  ;[$476A]; add '0'
        push    af                      ;[$476C]
        ld      a, $01                  ;[$476D]
        ld      (HAVE_DIGITS), a        ;[$476F]
        pop     af                      ;[$4772]

output_digit:                           ;[$4773]
        ld      (PRINT_ACC), hl         ;[$4773]; restore accumulator
        ld      (bc), a                 ;[$4776]; store digit
        inc     bc                      ;[$4777]
        pop     hl                      ;[$4778]; advance in divisor table
        inc     hl                      ;[$4779]
        inc     hl                      ;[$477A]
        push    hl                      ;[$477B]
        ld      a, e                    ;[$477C]
        cp      $01                     ;[$477D]; check for last divisor
        jr      nz, next_divisor        ;[$477F]
        pop     hl                      ;[$4781]
        ret                             ;[$4782]


DEAD18:                                 ;[$4783]
        defb    $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
                                        ;[$4783]

#ENDASM [$478D]

#[$478E]
   20  REM 
#ASM [$4793]

INCR_SCORE:                             ;[$4793]
        ld      a, (ALIEN_HIT)          ;[$4793]
        cp      $01                     ;[$4796]
        ret     z                       ;[$4798]
        ld      a, (WALL_HIT)           ;[$4799]
        cp      $00                     ;[$479C]
        ret     z                       ;[$479E]
        xor     a                       ;[$479F]
        ld      (WALL_HIT), a           ;[$47A0]
        ld      de, $0003               ;[$47A3]
        ld      hl, (topedo_pos0)       ;[$47A6]
        ld      h, $00                  ;[$47A9]
        and     a                       ;[$47AB]
        sbc     hl, de                  ;[$47AC]
        ex      de, hl                  ;[$47AE]
        ld      hl, (SCORE_lo)          ;[$47AF]

; these 5 instructions are POKEd in BASIC to either 
; NOP or ADD HL,DE depending on the selected skill level


score_multiply:                         ;[$47B2]
        add     hl, de                  ;[$47B2]
        add     hl, de                  ;[$47B3]
        add     hl, de                  ;[$47B4]
        add     hl, de                  ;[$47B5]
        add     hl, de                  ;[$47B6]
        ld      (SCORE_lo), hl          ;[$47B7]; save score
        call    SCORE_I2A               ;[$47BA]; convert to digit sequence
        ld      hl, (D_FILE)            ;[$47BD]; display
        ld      de, $0012               ;[$47C0]; add position of score
        add     hl, de                  ;[$47C3]
        ex      de, hl                  ;[$47C4]; to DE
        ld      hl, NUM_BUFFER          ;[$47C5]; HL=number buffer
        ld      b, $05                  ;[$47C8]; 5 digits

out_digit:                              ;[$47CA]
        ld      a, (hl)                 ;[$47CA]
        cp      $00                     ;[$47CB]; is it a blank?
        jr      z, skip_blanks          ;[$47CD]
        ldi                             ;[$47CF]; copy digit
        inc     bc                      ;[$47D1]; restore BC
        djnz    out_digit               ;[$47D2]; next digit
        ret                             ;[$47D4]


skip_blanks:                            ;[$47D5]
        inc     hl                      ;[$47D5]
        djnz    out_digit               ;[$47D6]; next digit
        ret                             ;[$47D8]


DEAD19:                                 ;[$47D9]
        defb    $00, $00                ;[$47D9]

#ENDASM [$47DB]

#[$47DC]
   21  REM 
#ASM [$47E1]

DELETE_TORPEDO:                         ;[$47E1]
        ld      bc, (torpedo_col)       ;[$47E1]
        call    SCR_POS                 ;[$47E5]
        ld      a, $34                  ;[$47E8]; does position has a torpedo?
        cp      (hl)                    ;[$47EA]
        ret     nz                      ;[$47EB]; ret if not
        ld      (hl), $00               ;[$47EC]; delete torpedo
        ret                             ;[$47EE]


DELETE_BOMB:                            ;[$47EF]
        ldi                             ;[$47EF]
        ld      h, b                    ;[$47F1]
        ld      b, (hl)                 ;[$47F2]
        call    SCR_POS                 ;[$47F3]
        ld      (hl), $00               ;[$47F6]; delete bomb
        ret                             ;[$47F8]


#ENDASM [$47F9]

#[$47FA]
   22  REM 
#ASM [$47FF]

; --------------------------------------------------------------------------------
; Init restored fortress
; --------------------------------------------------------------------------------


INIT_FORTRESS:                          ;[$47FF]
        ld      b, $3A                  ;[$47FF]; 58 chars black
        ld      hl, FORTRESS1           ;[$4801]

fortress_fill_outer_black:              ;[$4804]
        ld      (hl), $80               ;[$4804]
        inc     hl                      ;[$4806]
        djnz    fortress_fill_outer_black
                                        ;[$4807]
        ld      b, $32                  ;[$4809]; 50 chars grey

fortress_fill_grey:                     ;[$480B]
        ld      (hl), $08               ;[$480B]
        inc     hl                      ;[$480D]
        djnz    fortress_fill_grey      ;[$480E]
        ld      b, $2A                  ;[$4810]; 42 chars black

fortress_fill_inner_black:              ;[$4812]
        ld      (hl), $80               ;[$4812]
        inc     hl                      ;[$4814]
        djnz    fortress_fill_inner_black
                                        ;[$4815]
        ld      hl, $0215               ;[$4817]
        ld      (fortress_col), hl      ;[$481A]
        ld      hl, $0000               ;[$481D]
        ld      (timer_fortress_move), hl
                                        ;[$4820]
        ld      (WALL_HIT), hl          ;[$4823]
        ld      hl, $0900               ;[$4826]
        ld      (ship_col), hl          ;[$4829]
        ld      hl, $0100               ;[$482C]
        ld      (bomb_col), hl          ;[$482F]
        xor     a                       ;[$4832]
        ld      (torpedo_running), a    ;[$4833]
        ld      (bomb_running), a       ;[$4836]
        ld      (SHIP_HIT), a           ;[$4839]
        ret                             ;[$483C]


DEAD21:                                 ;[$483D]
        defb    $00, $00                ;[$483D]

#ENDASM [$483F]

#[$4840]
   23  REM 
#ASM [$4845]

press_any_key_msg:                      ;[$4845]
        defm    "HIT ANY LETTER WHEN READY"
                                        ;[$4845]

; --------------------------------------------------------------------------------
; Print message "Press any key"
; --------------------------------------------------------------------------------


PRESS_ANY_KEY:                          ;[$485E]
        ld      bc, $1703               ;[$485E]; row 23, col 3
        call    SCR_POS                 ;[$4861]
        ex      de, hl                  ;[$4864]; DE=screen address
        ld      hl, press_any_key_msg   ;[$4865]
        ld      bc, $0019               ;[$4868]
        ldir                            ;[$486B]
        ret                             ;[$486D]


DEAD22:                                 ;[$486E]
        defb    $00, $00, $00           ;[$486E]

DELETE_PRESS_ANY_KEY:                   ;[$4871]
        ld      bc, $1703               ;[$4871]; row 23, col 3
        call    SCR_POS                 ;[$4874]
        ld      b, $19                  ;[$4877]

put_space:                              ;[$4879]
        ld      (hl), $00               ;[$4879]
        inc     hl                      ;[$487B]
        djnz    put_space               ;[$487C]
        ret                             ;[$487E]


DEAD23:                                 ;[$487F]
        defb    $00, $00, $00           ;[$487F]

#ENDASM [$4882]

#[$4883]
   24  REM 
#ASM [$4888]

DEAD24:                                 ;[$4888]
        defb    $00, $85, $01, $85, $00, $00, $02, $80, $03, $00, $00, $87, $80, $04, $00, $00, $81, $00, $82, $00, $00, $00, $00, $00, $00, $87, $07, $80, $84, $04, $00, $84, $03, $07, $00, $00, $02, $80, $01, $00, $00, $00, $00, $00, $00, $01
                                        ;[$4888]

FLASH_ALIEN:                            ;[$48B6]
        ld      bc, (fortress_col)      ;[$48B6]
        inc     c                       ;[$48BA]
        inc     c                       ;[$48BB]
        inc     c                       ;[$48BC]; fortress-col+3
        ld      b, $05                  ;[$48BD]; row=5
        call    SCR_POS                 ;[$48BF]; HL=screen address
        ld      de, $001C               ;[$48C2]; distance between rows
        ld      a, $0E                  ;[$48C5]; 14 lines

flash_next_line:                        ;[$48C7]
        push    af                      ;[$48C7]; save line counter
        ld      b, $05                  ;[$48C8]; 5 columns

flash_next_col:                         ;[$48CA]
        ld      a, (hl)                 ;[$48CA]; get screen char
        xor     $80                     ;[$48CB]; invert it
        ld      (hl), a                 ;[$48CD]; and store
        inc     hl                      ;[$48CE]
        djnz    flash_next_col          ;[$48CF]; loop for all columns
        add     hl, de                  ;[$48D1]; next row
        pop     af                      ;[$48D2]; get row count
        dec     a                       ;[$48D3]
        cp      $00                     ;[$48D4]
        jr      nz, flash_next_line     ;[$48D6]; loop for next row
        ret                             ;[$48D8]


DEAD25:                                 ;[$48D9]
        defb    $EB, $21, $9C, $48, $C3, $77, $43, $3E, $02, $BE, $20, $1F, $3C, $32, $B5, $48, $EB, $23, $36, $05, $23, $36, $02, $23, $36, $05, $11, $1F, $00, $19, $36, $03, $23, $23, $36, $01, $11, $A3, $00, $19, $36, $80, $C9, $3E, $01, $32, $B5, $48, $EB, $23, $23, $36, $00, $23, $36, $85, $11, $21, $00, $19, $36, $03, $11, $A5, $00, $19, $36, $80, $C9, $00, $00, $00
                                        ;[$48D9]

#ENDASM [$4921]

#[$4922]
   25  REM 
#ASM [$4927]

DEAD26:                                 ;[$4927]
        defb    $ED, $4B, $1C, $41, $0C, $0C, $0C, $06, $05, $ED, $43, $46, $43, $CD, $05, $44, $06, $0E, $ED, $43, $46, $43, $CD, $05, $44, $C9, $00, $00, $00, $00, $00, $00, $00, $00
                                        ;[$4927]

#ENDASM [$4949]

#[$494A]
   26  REM 
#ASM [$494F]

LEVEL_DELAY_lo:                         ;[$494F]
        defb    $01                     ;[$494F]

LEVEL_DELAY_hi:                         ;[$4950]
        defb    $00                     ;[$4950]

; --------------------------------------------------------------------------------
; Game loop
; --------------------------------------------------------------------------------


GAME:                                   ;[$4951]
        call    CHECK_FORTRESS_MOVE     ;[$4951]
        call    TEST_KEYBOARD           ;[$4954]
        call    DRAW_ALIEN              ;[$4957]
        call    DRAW_FORTRESS           ;[$495A]
        call    MOVE_TORPEDO            ;[$495D]
        call    INCR_SCORE              ;[$4960]
        ld      a, (ALIEN_HIT)          ;[$4963]
        cp      $01                     ;[$4966]
        ret     z                       ;[$4968]
        call    DRAW_CANONS             ;[$4969]
        call    DRAW_BOMB               ;[$496C]
        ld      a, (SHIP_HIT)           ;[$496F]
        cp      $01                     ;[$4972]
        ret     z                       ;[$4974]
        call    FIRE_BOMB               ;[$4975]
        ld      hl, (LEVEL_DELAY_lo)    ;[$4978]
        ld      de, $0000               ;[$497B]

game_delay:                             ;[$497E]
        inc     de                      ;[$497E]
        and     a                       ;[$497F]
        sbc     hl, de                  ;[$4980]
        jp      z, GAME                 ;[$4982]
        add     hl, de                  ;[$4985]
        jr      game_delay              ;[$4986]


DEAD27:                                 ;[$4988]
        defb    $00, $00, $00, $00, $00, $00, $00
                                        ;[$4988]

#ENDASM [$498F]

#[$4990]
   27  REM 
#ASM [$4995]

DEAD28:                                 ;[$4995]
        defb    $3E, $BF, $BD, $28, $06, $3E, $7F, $BD, $C2, $F0, $43, $3A, $1C, $41, $C3, $C5, $43, $00, $00, $00, $00, $00, $00, $00
                                        ;[$4995]

#ENDASM [$49AD]

#[$49AE]
   28  REM 
#ASM [$49B3]

CLEAR_GAME_AREA:                        ;[$49B3]
        ld      bc, $0200               ;[$49B3]; row 2 column 0
        call    SCR_POS                 ;[$49B6]; HL = screen address
        ld      a, $14                  ;[$49B9]; 20 rows

clear_next_row:                         ;[$49BB]
        ld      b, $20                  ;[$49BB]; 32 chars

L49BD:                                  ;[$49BD]
        ld      (hl), $00               ;[$49BD]; clear char
        inc     hl                      ;[$49BF]
        djnz    L49BD                   ;[$49C0]; loop for columns
        inc     hl                      ;[$49C2]
        dec     a                       ;[$49C3]
        cp      $00                     ;[$49C4]
        jr      nz, clear_next_row      ;[$49C6]; loop for rows
        ret                             ;[$49C8]


#ENDASM [$49C9]

#[$49CA]
   29  REM 
#ASM [$49CF]

pressed_key:                            ;[$49CF]
        defw    $FBDF                   ;[$49CF]

; --------------------------------------------------------------------------------
; Test keyboard and do actions
; --------------------------------------------------------------------------------


TEST_KEYBOARD:                          ;[$49D1]
        call    KEYBOARD                ;[$49D1]; scan keyboard, H=column, L=half-row

L49D4:                                  ;[$49D4]
        ld      bc, (ship_col)          ;[$49D4]
        ld      (pressed_key), hl       ;[$49D8]
        ld      de, $FFFF               ;[$49DB]
        and     a                       ;[$49DE]
        sbc     hl, de                  ;[$49DF]
        ret     z                       ;[$49E1]; no key pressed, return
        ld      hl, (pressed_key)       ;[$49E2]
        bit     4, l                    ;[$49E5]; check top right half-row
        call    z, FIRE_TORPEDO         ;[$49E7]; key pressed - fire torpedo
        ld      hl, (pressed_key)       ;[$49EA]
        ld      bc, (ship_col)          ;[$49ED]
        bit     5, l                    ;[$49F1]; check 2nd right half-row
        call    z, FIRE_TORPEDO         ;[$49F3]; key pressed - fire torpedo
        ld      hl, (pressed_key)       ;[$49F6]
        ld      bc, (ship_col)          ;[$49F9]
        bit     3, l                    ;[$49FD]; check top left half-row
        jp      z, MOVE_SHIP_UP         ;[$49FF]; key pressed - move ship up
        bit     2, l                    ;[$4A02]; check second left half-row
        jp      z, MOVE_SHIP_UP         ;[$4A04]; key pressed - move ship up
        bit     0, l                    ;[$4A07]; check bottom left half-row
        jp      z, MOVE_SHIP_DOWN       ;[$4A09]; key pressed - move ship down
        bit     1, l                    ;[$4A0C]; check third left half-row
        jp      z, MOVE_SHIP_DOWN       ;[$4A0E]; key pressed - move ship down
        bit     6, l                    ;[$4A11]; check thrird right half-row
        jr      z, check_left_right     ;[$4A13]; key pressed - move ship left or right
        bit     7, l                    ;[$4A15]; check bottom right half-row
        ret     nz                      ;[$4A17]; not pressed - return

check_left_right:                       ;[$4A18]
        nop                             ;[$4A18]
        nop                             ;[$4A19]
        nop                             ;[$4A1A]
        nop                             ;[$4A1B]
        nop                             ;[$4A1C]
        bit     5, h                    ;[$4A1D]; check move ship left
        jp      z, MOVE_SHIP_LEFT       ;[$4A1F]
        bit     4, h                    ;[$4A22]; check move ship left
        jp      z, MOVE_SHIP_LEFT       ;[$4A24]
        bit     3, h                    ;[$4A27]; check move ship left
        jp      z, MOVE_SHIP_LEFT       ;[$4A29]
        bit     2, h                    ;[$4A2C]; check move ship right
        jp      z, MOVE_SHIP_RIGHT      ;[$4A2E]
        bit     1, h                    ;[$4A31]; check move ship right
        jp      z, MOVE_SHIP_RIGHT      ;[$4A33]
        ret                             ;[$4A36]


DEAD29:                                 ;[$4A37]
        defb    $00, $00, $00, $00, $00, $00
                                        ;[$4A37]

#ENDASM [$4A3D]

#[$4A3E]

#: --------------------------------------------------------------------------------
#: Start of introdution
#: --------------------------------------------------------------------------------


@Line_StartIntro:
   30  CLS 
#[$4A44]

#: init torpedo position

   34  POKE &intro_torpedo_pos_lo,252
#[$4A5F]
   35  POKE &intro_torpedo_pos_hi,69
#[$4A79]

#: show welcome

   40  PRINT  AT 5,11;"WELCOME TO";
#[$4A9E]
   50  PRINT  AT 7,5;"\.'\''\'. \''\:'\' \ '\':\'' \.'\''\'. \.'\''\'. \: \ .\' ";
#[$4ACE]
   60  PRINT  AT 8,5;"\:.\..\.:  \:   \ :  \:.\..\.: \:    \:'\'.";
#[$4AFD]
   70  PRINT  AT 9,5;"\:  \ :  \:   \ :  \:  \ : \'.\..\.' \:  \'.";\00
#[$4B2E]
   80  PRINT  AT 17,3;"COPR., PAUL CARLSON, 1982";
#[$4B62]
   90  PRINT  AT 21,0;"DO YOU WANT INSTRUCTIONS? (Y/N)";
#[$4B9C]

#: clear ship numbers

  100  POKE &SHIP1_nr,128
#[$4BB7]
  110  POKE &SHIP2_nr,128
#[$4BD2]
  120  POKE &SHIP3_nr,128
#[$4BED]

#: --------------------------------------------------------------------------------
#: Intro wait key
#: --------------------------------------------------------------------------------


@Line_IntroWaitKey:
  140  LET L= USR &INTRO_SCREEN
#[$4C01]
  150  LET L= USR &INTRO_FIRE_TORPEDO
#[$4C15]
  155  IF  INKEY$ ="" THEN  GOTO @Line_IntroWaitKey
#[$4C2A]
  160  IF  INKEY$ ="N" THEN  GOTO @Line_StartGame
#[$4C40]
  170  IF  INKEY$ <>"Y" THEN  GOTO @Line_IntroWaitKey
#[$4C56]
  180  GOSUB @Line_Instructions
#[$4C66]

#: --------------------------------------------------------------------------------
#: Start of new game
#: --------------------------------------------------------------------------------


@Line_StartGame:
  190  GOSUB @Line_SelectLevel
#[$4C76]

#: W is the ship number init to 0, incremented below to start at 1

  195  LET W=28
#[$4C86]
  200  LET S=0
#[$4C95]
  202  LET R=200
#[$4CA6]

#: 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT

  205  POKE 18146,R
#[$4CB9]
  210  POKE &SCORE_lo,0
#[$4CD2]
  220  POKE &SCORE_hi,0
#[$4CEB]
  230  LET L= USR &INIT_FORTRESS
#[$4CFF]
  250  PRINT  AT 0,10;"SCORE: 0"
#[$4D21]

#: --------------------------------------------------------------------------------
#: Start of new ship
#: --------------------------------------------------------------------------------


@Line_StartNewShip:
  260  LET W=W+1
#[$4D32]

#: if incremented ship is >5, exit

  270  IF W=34 THEN  GOTO @Line_GameOver
#[$4D4D]

#: write ship number in the three ship drawings

  280  POKE &SHIP1_nr,W
#[$4D60]
  290  POKE &SHIP2_nr,W
#[$4D73]
  300  POKE &SHIP3_nr,W
#[$4D86]

@Line_StartLevelWaitKey:
  310  LET L= USR &DRAW_CANONS
#[$4D9A]
  320  LET L= USR &PRESS_ANY_KEY
#[$4DAE]

@Line_LevelWaitKey:
  340  LET L= USR &DRAW_FORTRESS
#[$4DC2]
  350  LET L= USR &DRAW_ALIEN
#[$4DD6]
  360  LET L= USR &DRAW_SHIP
#[$4DEA]
  370  IF  INKEY$ ="" THEN  GOTO @Line_LevelWaitKey
#[$4DFF]
  380  LET L= USR &DELETE_PRESS_ANY_KEY
#[$4E13]

#: --------------------------------------------------------------------------------
#: Run the game
#: --------------------------------------------------------------------------------

  390  LET L= USR &GAME
#[$4E27]
  400  LET L= USR &DELETE_TORPEDO
#[$4E3B]
  410  LET L= USR &DELETE_BOMB
#[$4E4F]
  420  IF  PEEK &ALIEN_HIT=1 THEN  GOTO @Line_AlienHit
#[$4E74]

#: --------------------------------------------------------------------------------
#: Ship was hit
#: --------------------------------------------------------------------------------

  430  FOR N=1 TO 32
#[$4E8C]
  440  LET L= USR &ROTATE_SHIP
#[$4EA0]
  450  LET L=L+N
#[$4EAB]
  460  NEXT N
#[$4EB2]
  470  LET L= USR &DELETE_SHIP
#[$4EC6]

#: init next ship

  475  POKE &bomb_col,0
#[$4EDF]
  480  POKE &torpedo_running,0
#[$4EF8]
  485  POKE &bomb_running,0
#[$4F11]
  490  POKE &ship_col,0
#[$4F2A]
  500  POKE &ship_row,9
#[$4F43]
  505  POKE &SHIP_HIT,0
#[$4F5C]
  510  GOTO @Line_StartNewShip
#[$4F6B]

#: --------------------------------------------------------------------------------
#: Alien was hit
#: --------------------------------------------------------------------------------


@Line_AlienHit:
  520  FOR N=1 TO 50
#[$4F83]
  540  LET L= USR &FLASH_ALIEN
#[$4F97]
  545  LET L=L+0
#[$4FA8]
  547  LET L=L+0
#[$4FB9]
  550  NEXT N
#[$4FC0]
  560  LET S= PEEK &SCORE_lo+256* PEEK &SCORE_hi
#[$4FEB]
  570  LET S=S+F*(( PEEK &topedo_pos0)-3)
#[$500F]
  575  PRINT  AT 0,17;S
#[$5028]
  580  POKE &SCORE_hi, INT (S/256)
#[$5048]
  590  POKE &SCORE_lo,(S-256* PEEK &SCORE_hi)
#[$5074]
  592  IF R>20 THEN  LET R=R-20
#[$5092]

#: 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT

  595  POKE 18146,R
#[$50A5]
  600  LET L= USR &CLEAR_GAME_AREA
#[$50B9]
  620  LET L= USR &INIT_FORTRESS
#[$50CD]
  630  GOTO @Line_StartLevelWaitKey
#[$50DC]

#: --------------------------------------------------------------------------------
#: Game Over
#: --------------------------------------------------------------------------------


@Line_GameOver:
  640  LET L= USR &CLEAR_GAME_AREA
#[$50F0]
  650  PRINT  AT 0,4;"FINAL";
#[$510F]
  660  PRINT  AT 10,13;"\:'\''\''\''\''\':"
#[$5130]
  670  PRINT  AT 11,13;"\: GAME\ :"
#[$5151]
  680  PRINT  AT 12,13;"\: OVER\ :"
#[$5172]
  690  PRINT  AT 13,13;"\:.\..\..\..\..\.:"
#[$5193]
  700  PRINT  AT 16,6;"PLAY AGAIN? (Y OR N)"
#[$51C1]

@Line__720:
  720  IF  INKEY$ ="N" THEN  STOP 
#[$51CE]
  730  IF  INKEY$ ="Y" THEN  GOTO @Line_StartGame
#[$51E4]
  740  GOTO @Line__720
#[$51F3]

#: --------------------------------------------------------------------------------
#: Show intructions
#: --------------------------------------------------------------------------------


@Line_Instructions:
 1000  CLS 
#[$51F9]
 1010  PRINT  AT 0,10;"YOUR GOAL:"
#[$521D]
 1020  PRINT  AT 2,0;"TO SCORE AS MANY POINTS AS YOU   CAN BY DESTROYING THE ALIEN AND BLOCKS OF HIS FORTRESS."
#[$528E]
 1030  PRINT  AT 6,0;"YOU HAVE FIVE SHIPS TO DESTROY   THE ALIEN AS MANY TIMES AS YOU  CAN.  WARNING - THE ALIEN GUNS  ARE INDESTRUCTIBLE."
#[$531B]
 1040  PRINT  AT 11,0;"POINTS ARE AWARDED BASED ON      SKILL LEVEL AND THE DISTANCE    YOUR SHIP IS FROM THE LEFT EDGE OF THE SCREEN WHEN IT FIRES."
#[$53B2]
 1050  PRINT  AT 16,0;"DESTROYING THE ALIEN IS WORTH    FIFTY TIMES THE POINTS AWARDED  FOR DESTROYING A BLOCK OF HIS   FORTRESS."
#[$5436]
 1060  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."
#[$546D]

@Line_IntructionsWaitKey1:
 1070  IF  INKEY$ ="" THEN  GOTO @Line_IntructionsWaitKey1
#[$5483]
 1080  CLS 
#[$5489]
 1090  PRINT  AT 0,3;"YOUR SHIPS CONTROL PANEL:"
#[$54BB]
 1100  PRINT  AT 3,2;"\:'\''\''\''\''\''\''\''\''\''\''\':    \:'\''\''\''\''\''\''\''\''\''\''\':    \: 1 2 3 4 5 \ :    \: 6 7 8 9 0 \ :    \:  Q W E R T\ :    \:  Y U I O P\ :    \:.\..\..\..\..\..\..\..\..\..\..\.:    \:.\..\..\..\..\..\..\..\..\..\..\.:    MOVE SHIP UP    FIRE TORPEDO"\00
#[$5571]
 1110  PRINT  AT 11,0;"\:'\''\''\''\''\''\''\''\''\''\':  \:'\''\''\''\''\''\''\': \:'\''\''\''\''\''\''\''\''\':\: A S D F G\ :  \:  H J K\ : \:  L ENTER\ :\:  Z X C V \ :  \: B N M \ : \: . SPACE \ :\:.\..\..\..\..\..\..\..\..\..\.:  \:.\..\..\..\..\..\..\.: \:.\..\..\..\..\..\..\..\..\.: MOVE SHIP     MOVE      MOVE      DOWN        SHIP      SHIP                  LEFT      RIGHT"
#[$5669]
 1120  PRINT  AT 21,1;"HIT ANY LETTER TO CONTINUE..."
#[$56A0]

@Line_IntructionsWaitKey2:
 1130  IF  INKEY$ ="" THEN  GOTO @Line_IntructionsWaitKey2
#[$56B6]
 1140  RETURN 
#[$56BC]

#: --------------------------------------------------------------------------------
#: Select skill level
#: --------------------------------------------------------------------------------


@Line_SelectLevel:
 2000  CLS 
#[$56C2]
 2010  PRINT  AT 2,1;"SKILL LEVELS:"
#[$56E8]
 2020  PRINT  AT 4,15;"1 = SLOW"
#[$570A]
 2030  PRINT  AT 6,15;"2 = AVERAGE"
#[$572F]
 2040  PRINT  AT 8,15;"3 = FAST"
#[$5751]
 2050  PRINT  AT 10,15;"4 = VERY FAST"
#[$5779]
 2060  PRINT  AT 12,15;"5 = SUICIDAL"
#[$57A0]
 2070  PRINT  AT 16,6;"PRESS SKILL LEVEL..."
#[$57CE]

@Line_WaitSkillSelection:
 2080  IF  INKEY$ ="" THEN  GOTO @Line_WaitSkillSelection
#[$57E4]
 2090  LET V= CODE  INKEY$ -28
#[$57F7]
 2100  IF V<1 OR V>5 THEN  GOTO @Line_WaitSkillSelection
#[$581C]

#: selected skill is in V

 2105  CLS 
#[$5822]
 2110  POKE &LEVEL_DELAY_lo,1
#[$583B]
 2120  POKE &LEVEL_DELAY_hi,5-V
#[$5856]
 2130  LET F=50*V
#[$5868]

#: 5 instructions in score_multiply (18353+1..28353+5) are replaced by 
#: NOP (0) or ADD HL,DE (25=$19) depending on the selected level 
#: 1 ADDs for level 1, 5 ADDs for level 5

 2150  FOR N=1 TO 5
#[$587F]
 2160  LET A=18353+N
#[$5894]
 2170  POKE A,0
#[$58A3]
 2180  IF N<=V THEN  POKE A,25
#[$58B8]
 2190  NEXT N
#[$58BF]
 2200  RETURN 
#[$58C5]

#: --------------------------------------------------------------------------------
#: Save and autostart routine
#: --------------------------------------------------------------------------------

 3000  SAVE "ATTAC%K"
#[$58D3]
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
#: SAVE_FORTRESS2           = $4117 = 16663
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
#: DEAD30                   = $4204 = 16900
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
#: DEAD31                   = $43EA = 17386
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
#: [$41C0] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4200] CCCCCCCCCCCCbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBbbbbbbCCCCCCCC
#: [$4240] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbBBBTTTTTTTTT
#: [$4280] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$42C0] CCCCCCCCCCCCCCCBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBBBBbbbbbbTTTTTT
#: [$4300] TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#: [$4340] TTTTTBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$4380] CCCCCCCCCCBBBBBbbbbbbCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#: [$43C0] CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBBb
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

