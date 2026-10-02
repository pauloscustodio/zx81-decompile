;------------------------------------------------------------------------------
; CPU::Z80::Disassembler control file
;------------------------------------------------------------------------------

4009 :F ../t/FortressOfZorlac.p

; ROM routines
02BB := KEYBOARD

; system variables
400C := D_FILE

; first REM line
4081 :B 
     :#; REM of start of assembly

; last REM line
4A3E :B
     :#; REM of end of assembly

; compute screen address
4212 :C SCR_POS
     :#;--------------------------------------------------------------------------------
     :#; Compute screen position of row-col in BC to HL 
     :#; Preserves all other registers
     :#;--------------------------------------------------------------------------------
421C :C SCR_POS_next_row
4223 :C SCR_POS_row_found
4226 :C SCR_POS_next_col
422D :C SCR_POS_col_found

4082-40BA						:B FORTRESS1
40BB							:B SAVE_FORTRESS1
40BC-4117                       :B FORTRESS2

4118 EE40                       :W addr_fortress
411A 1741                       :W addr_fortress_save
411C 13                         :B fortress_col
411D 02                         :B fortress_row
411E 10                         :B fortress_rows
411F 05                         :B fortress_cols
4120 29                         :B fortress_blocks
4121 00         nop
4122 00         nop
4123 00         nop
4124 00         nop
4125 00         nop
4126 76         halt
4127 00         nop
4128 02         ld (bc), a
4129 61         ld h, c
412A 00         nop
412B EA
412C 21BB40                     :C DRAW_FORTRESS
                                :#;--------------------------------------------------------------------------------
                                :#; Draw fortress at fortress_row/fortress_col
                                :#;--------------------------------------------------------------------------------
412F 221A41     ld ($411A), hl
4132 218240     ld hl, $4082
4135 221841     ld ($4118), hl
4138 3E14       ld a, $14				:; 20 rows
413A 321E41     ld ($411E), a
413D 3E09       ld a, $09				:; 9 columns
413F 321F41     ld ($411F), a
4142 3E39       ld a, $39				:; 57 blocks
4144 322041     ld ($4120), a
4147 CD9141     call $4191
414A CD5B41     call $415B
414D CD5B41     call $415B
4150 05         dec b
4151 05         dec b
4152 0D         dec c
4153 0D         dec c
4154 ED431C41   ld ($411C), bc
4158 C38A41     jp $418A
415B 04         inc b					:C rotate_inner_layer
415C 0C         inc c
415D ED431C41   ld ($411C), bc
4161 ED531841   ld ($4118), de
4165 3A2041     ld a, ($4120)
4168 D608       sub $08
416A 322041     ld ($4120), a
416D 2600       ld h, $00
416F 6F         ld l, a
4170 19         add hl, de
4171 221A41     ld ($411A), hl
4174 3A1E41     ld a, ($411E)
4177 D602       sub $02
4179 321E41     ld ($411E), a
417C 3A1F41     ld a, ($411F)
417F D602       sub $02
4181 321F41     ld ($411F), a
4184 00         nop
4185 00         nop
4186 00         nop
4187 CD9141     call $4191
418A C9         ret							:C return
418B 76         halt
418C 00         nop
418D 03         inc bc
418E 7D         ld a, l
418F 00         nop
4190 EA
4191 ED5B1A41   ld de, ($411A)				:C rotate_outer_layer
											:; point to save block
4195 1A         ld a, (de)					:; get block to be saved to A
4196 2A1A41     ld hl, ($411A)				:; get address of save
4199 2B         dec hl						:; minus 1=last block
419A 0600       ld b, $00					:; BC will be number of blocks
419C F5         push af						:; save end block
419D 3A2041     ld a, ($4120)				:; get number of blocks
41A0 4F         ld c, a						:; to BC
41A1 F1         pop af						:; A is block to be saved from end
41A2 EDB8       lddr						:; move all blocks 1 step
41A4 2A1841     ld hl, ($4118)				:; get first address
41A7 77         ld (hl), a					:; and store saved char
41A8 E5         push hl						:; save address of fortress
41A9 ED4B1C41   ld bc, ($411C)				:; coords of fortress to BC
41AD CD1242     call $4212					:; screen address to HL
41B0 EB         ex de, hl					:; screen address to DE
41B1 E1         pop hl						:; HL=blocks address
41B2 C5         push bc						:; save coords
41B3 3A1E41     ld a, ($411E)				:; A=number of rows
41B6 012000     ld bc, $0020				:; distance between rows
41B9 EDA0       ldi							:C rot_blocks_left_column
											:; copy one
41BB 03         inc bc						:; restore BC
41BC 3D         dec a						:; count rows
41BD FE00       cp $00
41BF 2805       jr z, $41C6					:; jump forward if end of rows
41C1 EB         ex de, hl
41C2 09         add hl, bc					:; move to next row
41C3 EB         ex de, hl
41C4 18F3       jr $41B9
41C6 ED4B1F41   ld bc, ($411F)				:C rot_block_next1
41CA 0600       ld b, $00					:; BC = number of cols
41CC EDB0       ldir						:; copy bottom row
41CE 3A1E41     ld a, ($411E)				:; A = number of rows
41D1 012200     ld bc, $0022
41D4 EDA0       ldi							:C rot_blocks_right_column
											:; copy block
41D6 03         inc bc						:; restore BC
41D7 3D         dec a						:; count rows
41D8 FE00       cp $00
41DA 2807       jr z, $41E3					:; jump forward if finished
41DC EB         ex de, hl
41DD A7         and a
41DE ED42       sbc hl, bc					:; move to previous row
41E0 EB         ex de, hl
41E1 18F1       jr $41D4
41E3 3A1F41     ld a, ($411F)				:C rot_block_next2
											:; A = number of columns
41E6 1B         dec de						:C rot_blocks_top_row
41E7 1B         dec de
41E8 EDA0       ldi							:; copy one block
41EA 3D         dec a
41EB FE00       cp $00
41ED 20F7       jr nz, $41E6				:; jump back if not finished
41EF C1         pop bc
41F0 EB         ex de, hl					:; DE=blocks, HL=screen
41F1 C9         ret
41F2 3EF7       ld a, $F7
41F4 BC         cp h
41F5 280D       jr z, $4204
41F7 3EEF       ld a, $EF
41F9 BC         cp h
41FA 2808       jr z, $4204
41FC 3EDF       ld a, $DF
41FE BC         cp h
41FF 2803       jr z, $4204
4201 C3F543     jp $43F5
4204 AF         xor a
4205 B9         cp c
4206 CAF543     jp z, $43F5
4209 C3EA43     jp $43EA
420C 76         halt
420D 00         nop
420E 04         inc b
420F 2200EA     ld ($EA00), hl
4212 D5         push de
4213 F5         push af
4214 2A0C40     ld hl, ($400C)
4217 23         inc hl
4218 AF         xor a
4219 112100     ld de, $0021
421C B8         cp b
421D 2804       jr z, $4223
421F 19         add hl, de
4220 3C         inc a
4221 18F9       jr $421C
4223 79         ld a, c
4224 1E00       ld e, $00
4226 BB         cp e
4227 2804       jr z, $422D
4229 1C         inc e
422A 23         inc hl
422B 18F9       jr $4226
422D F1         pop af
422E D1         pop de
422F C9         ret
4230 00         nop
4231 00         nop
4232 76         halt
4233 00         nop
4234 05         dec b
4235 3800       jr c, $4237
4237 EA
4238 2182     jp pe, $8221          :C erase_hit_block_from_fortress
423A 40         ld b, b
423B ED5B1C41   ld de, ($411C)
423F D5         push de
4240 78         ld a, b
4241 92         sub d
4242 1600       ld d, $00
4244 5F         ld e, a
4245 19         add hl, de
4246 D1         pop de
4247 79         ld a, c
4248 93         sub e
4249 FE01       cp $01
424B 2805       jr z, $4252
424D 300D       jr nc, $425C
424F 3600       ld (hl), $00
4251 C9         ret
4252 1600       ld d, $00
4254 3E39       ld a, $39
4256 00         nop
4257 5F         ld e, a
4258 19         add hl, de
4259 3600       ld (hl), $00
425B C9         ret
425C 1600       ld d, $00
425E 3E39       ld a, $39
4260 00         nop
4261 87         add a, a
4262 D608       sub $08
4264 5F         ld e, a
4265 19         add hl, de
4266 3600       ld (hl), $00
4268 C9         ret
4269 00         nop
426A 00         nop
426B 00         nop
426C 00         nop
426D 00         nop
426E 76         halt
426F 00         nop
4270 0682       ld b, $82
4272 00         nop
4273 EA
4274 16                     :B alien_col
4275 08                     :B alien_row

4276 00         nop

4277-427B 0000000000        :B ALIEN
427C-4280 8707808404        :B
4281-4285 0084030700        :B
4286-428A 0002800100        :B
428B-428F 0007808400        :B
4290-4294 0087800400        :B
4295-4299 0081008200        :B
429A-429E 0000000000        :B

429F 2A1C41     ld hl, ($411C)  :C DRAW_ALIEN
                                :#;--------------------------------------------------------------------------------
                                :#; Draw alien at fortress_row/fortress_col
                                :#;--------------------------------------------------------------------------------
42A2 7D         ld a, l
42A3 C603       add a, $03
42A5 327442     ld ($4274), a   :; fortress column+3 to alien column
42A8 ED4B7442   ld bc, ($4274)
42AC C5         push bc
42AD 3AEF42     ld a, ($42EF)
42B0 FE00       cp $00
42B2 280E       jr z, $42C2
42B4 05         dec b
42B5 CDD142     call $42D1
42B8 C1         pop bc
42B9 78         ld a, b
42BA FE06       cp $06
42BC C0         ret nz
42BD AF         xor a
42BE 32EF42     ld ($42EF), a   :C store_alien_pos
42C1 C9         ret
42C2 04         inc b           :C alien_downwards
42C3 CDD142     call $42D1
42C6 C1         pop bc
42C7 78         ld a, b
42C8 FE0A       cp $0A
42CA C0         ret nz
42CB 3E01       ld a, $01
42CD 18EF       jr $42BE
42CF 00         nop
42D0 00         nop
42D1 ED437442   ld ($4274), bc      :C draw_alien_at_BC
42D5 CD1242     call $4212
42D8 EB         ex de, hl           :; DE=screen address
42D9 217742     ld hl, $4277        :; source of alien
42DC 3E08       ld a, $08           :; 8 rows
42DE 010500     ld bc, $0005        :C alien_each_row
                                    :; 4 cols
42E1 EDB0       ldir                :; copu one row
42E3 3D         dec a
42E4 FE00       cp $00
42E6 C8         ret z               :; exit afyet 8 rows
42E7 EB         ex de, hl
42E8 011C00     ld bc, $001C        :; advance to next row
42EB 09         add hl, bc
42EC EB         ex de, hl
42ED 18EF       jr $42DE
42EF 01                             :B alien_direction
42F0 0000     ld bc, $0000
42F2 00         nop
42F3 00         nop
42F4 76         halt
42F5 00         nop
42F6 07         rlca
42F7 97         sub a
42F8 00         nop
42F9 EA
42FA-42FE 8180808082                :B SHIP1
42FF-4303 8282818380                :B
4304-4308 0080808000                :B
4309-430D 0707840380                :B
430E-4312 8480808007                :B
4313-4317 8180808082                :B SHIP2
4318-431C 8281838281                :B
431D-4321 0080808000                :B
4322-4326 0784030784                :B
4327-432B 8480808007                :B
432C-4330 8180808082                :B SHIP3
4331-4335 8083828181                :B
4336-433A 0080808000                :B
433B-433F 8003078484                :B
4340-4344 8480808007                :B
4345 03                             :B which_ship
4346 00                             :B ship_col
4347 09                             :B ship_row
4348 ED4B4643   ld bc, ($4346)      :C DRAW_SHIP
                                    :#;--------------------------------------------------------------------------------
                                    :#; Draw ship at row-col ship_row/ship_col
                                    :#;--------------------------------------------------------------------------------
434C CD1242     call $4212          :C DRAW_SHIP_BC
                                    :#;--------------------------------------------------------------------------------
                                    :#; Draw ship at row-col BC
                                    :#;--------------------------------------------------------------------------------
                                    :; HL=screen position                                    
434F EB         ex de, hl           :; DE=screen position
4350 214543     ld hl, $4345
4353 3E01       ld a, $01
4355 BE         cp (hl)
4356 2009       jr nz, $4361
4358 3C         inc a
4359 324543     ld ($4345), a
435C 21FA42     ld hl, $42FA
435F 1816       jr $4377
4361 3E02       ld a, $02           :C not_ship1
4363 BE         cp (hl)
4364 2009       jr nz, $436F
4366 3C         inc a
4367 324543     ld ($4345), a
436A 211343     ld hl, $4313
436D 1808       jr $4377
436F 3E01       ld a, $01           :C not_ship2
4371 324543     ld ($4345), a
4374 212C43     ld hl, $432C
4377 3E05       ld a, $05           :C draw_ship_hl
                                    :#; DE is screen address, HL is ship
                                    :; 5 rows
4379 010500     ld bc, $0005        :C draw_ship_row
                                    :; 5 columns
437C EDB0       ldir                :; copy one row
437E 3D         dec a
437F FE00       cp $00
4381 C8         ret z               :; return when 5 rows are printed
4382 EB         ex de, hl
4383 011C00     ld bc, $001C        :; DE+=33-5=28
4386 09         add hl, bc
4387 EB         ex de, hl
4388 18EF       jr $4379
438A 00         nop
438B 00         nop
438C 00         nop
438D 00         nop
438E 00         nop
438F 76         halt
4390 00         nop
4391 08         ex af, af'
4392 6C         ld l, h
4393 00         nop
4394 EACDBB     jp pe, $BBCD
4397 02         ld (bc), a
4398 ED4B4643   ld bc, ($4346)
439C 3EFF       ld a, $FF
439E BD         cp l
439F 2854       jr z, $43F5
43A1 3D         dec a
43A2 BD         cp l
43A3 282B       jr z, $43D0
43A5 3D         dec a
43A6 BD         cp l
43A7 2827       jr z, $43D0
43A9 3D         dec a
43AA 3D         dec a
43AB BD         cp l
43AC 282D       jr z, $43DB
43AE 3EF7       ld a, $F7
43B0 BD         cp l
43B1 2828       jr z, $43DB
43B3 3EEF       ld a, $EF
43B5 BD         cp l
43B6 2838       jr z, $43F0
43B8 3EDF       ld a, $DF
43BA BD         cp l
43BB 2833       jr z, $43F0
43BD 3EF7       ld a, $F7
43BF 94         sub h
43C0 3024       jr nc, $43E6
43C2 3A1C41     ld a, ($411C)       :C MOVE_SHIP_RIGHT
                                    :#;--------------------------------------------------------------------------------
                                    :#; Called in response to key, BC has ship coords
                                    :#;--------------------------------------------------------------------------------
                                    :; A=fortress column
43C5 D608       sub $08             :; -8 (5 for ship + 2 for cannons + 1)
43C7 B9         cp c
43C8 282B       jr z, $43F5         :; reached limit
43CA CD0544     call $4405
43CD 0C         inc c               :; move right
43CE 1825       jr $43F5
43D0 3E10       ld a, $10           :C MOVE_SHIP_DOWN
                                    :#;--------------------------------------------------------------------------------
                                    :#; Called in response to key, BC has ship coords
                                    :#;--------------------------------------------------------------------------------
43D2 B8         cp b
43D3 2820       jr z, $43F5         :; reached limit
43D5 CD0544     call $4405
43D8 04         inc b               :; move down
43D9 181A       jr $43F5
43DB 3E03       ld a, $03           :C MOVE_SHIP_UP
                                    :#;--------------------------------------------------------------------------------
                                    :#; Called in response to key, BC has ship coords
                                    :#;--------------------------------------------------------------------------------
43DD B8         cp b
43DE 2815       jr z, $43F5         :; reached limit
43E0 CD0544     call $4405
43E3 05         dec b               :; move up
43E4 180F       jr $43F5
43E6 AF         xor a               :C MOVE_SHIP_LEFT
                                    :#;--------------------------------------------------------------------------------
                                    :#; Called in response to key, BC has ship coords
                                    :#;--------------------------------------------------------------------------------
43E7 B9         cp c
43E8 280B       jr z, $43F5         :; reached limit
43EA CD0544     call $4405
43ED 0D         dec c               :; move right
43EE 1805       jr $43F5
43F0 C5         push bc
43F1 CD3144     call $4431
43F4 C1         pop bc
43F5 ED434643   ld ($4346), bc      :C save_ship_pos
43F9 CD4843     call $4348
43FC C9         ret
43FD 00         nop
43FE 00         nop
43FF 76         halt
4400 00         nop
4401 09         add hl, bc
4402 23         inc hl
4403 00         nop
4404 EA
4405 C5         push bc             :C DELETE_SHIP
                                    :#;--------------------------------------------------------------------------------
                                    :#; Delete ship before moving it
                                    :#;--------------------------------------------------------------------------------
                                    :; save coords
4407 ED4B4643   ld bc, (ship_col)
440A CD1242     call $4212          :; HL=screen address
440D 3E05       ld a, $05           :; 5 rows
440F 111C00     ld de, $001C        :; distance in screen between rows
4412 0605       ld b, $05           :C delete_row
                                    :; 5 cols
4414 3600       ld (hl), $00        :C delete_col
                                    :; delete char
4416 23         inc hl
4417 10FB       djnz $4414
4419 19         add hl, de          :; next row
441A 3D         dec a
441B FE00       cp $00
441D 20F3       jr nz, $4412
441F C1         pop bc              :; restore coords
4420 C9         ret

4421 00         nop
4422 00         nop
4423 00         nop
4424 00         nop
4425 00         nop
4426 76         halt
4427 00         nop
4428 0A         ld a, (bc)
4429 2E00       ld l, $00
442B EA
442C 00                             :B torpedo_running
442D 11     jp pe, $1100            :B torpedo_col
442E 12         ld (de), a          :B torpedo_row
442F 0A         ld a, (bc)          :W topedo_pos0
4430 12         ld (de), a
4431 3A2C44     ld a, ($442C)       :C FIRE_TORPEDO
                                    :#;--------------------------------------------------------------------------------
                                    :#; Called in response to key, BC has ship coords
                                    :#;--------------------------------------------------------------------------------
                                    :; is a torpedo on the way?
4434 FE01       cp $01
4436 C8         ret z               :; return if yes
4437 2A4643     ld hl, ($4346)
443A 24         inc h
443B 24         inc h
443C 2C         inc l
443D 2C         inc l
443E 2C         inc l
443F 2C         inc l
4440 222D44     ld ($442D), hl      :; torpedo coord is mouth of ship
4443 222F44     ld ($442F), hl      :; make a copy of the cooords - why?
4446 44         ld b, h
4447 4D         ld c, l
4448 CD1242     call $4212          :; HL=screen position
444B 3634       ld (hl), $34        :; draw torpedo
444D 3E01       ld a, $01
444F 322C44     ld ($442C), a       :; signal torpedo on the way
4452 C9         ret
4453 00         nop
4454 00         nop
4455 00         nop
4456 00         nop
4457 00         nop
4458 76         halt
4459 00         nop
445A 0B         dec bc
445B 68         ld l, b
445C 00         nop
445D EA
445E 00                             :B WALL_HIT
445F 00                             :B ALIEN_HIT
4460 AF         xor a               :C MOVE_TORPEDO
                                    :#;--------------------------------------------------------------------------------
                                    :#; moves torpedo if it has been created
                                    :#;--------------------------------------------------------------------------------
4461 325E44     ld ($445E), a
4464 325F44     ld ($445F), a
4467 3A2C44     ld a, ($442C)
446A FE00       cp $00
446C C8         ret z               :; torpedo was not fired, return
446D ED4B2D44   ld bc, ($442D)
4471 CD1242     call $4212          :; HL=torpedo coord
4474 3E34       ld a, $34           
4476 BE         cp (hl)             :; is there a torpedo?
4477 2002       jr nz, $447B        :; jump forward if not
4479 3600       ld (hl), $00        :; erase it
447B ED5B1C41   ld de, ($411C)      :C topedo_erased
447F 7B         ld a, e
4480 3D         dec a
4481 91         sub c
4482 280B       jr z, $448F         :; torpedo col == fortress col -1
4484 3809       jr c, $448F         :; torpedo col >  fortress col -1
4486 23         inc hl              :; move torpedo
4487 3634       ld (hl), $34        :C draw_torpedo_store_pos
                                    :; draw it
4489 0C         inc c
448A ED432D44   ld ($442D), bc      :; store new coords
448E C9         ret

448F 23         inc hl              :C torpedo_hit
4490 7B         ld a, e             
4491 C604       add a, $04          :; A=fortress-col+4 (alien)
4493 91         sub c
4494 2820       jr z, $44B6         :; inside alien - stop torpedo
4496 AF         xor a
4497 BE         cp (hl)             :; hit space - continue
4498 28ED       jr z, $4487
449A 3E80       ld a, $80
449C BE         cp (hl)             :; hit black border
449D 280C       jr z, $44AB
449F 3E08       ld a, $08
44A1 BE         cp (hl)             :; hit gray border
44A2 2807       jr z, $44AB
44A4 3E01       ld a, $01
44A6 325F44     ld ($445F), a
44A9 180B       jr $44B6
44AB 0C         inc c               :C hit_fortress
44AC 3600       ld (hl), $00        :; erase block
44AE CD3842     call $4238
44B1 3E01       ld a, $01
44B3 325E44     ld ($445E), a
44B6 AF         xor a               :C stop_torpedo
44B7 322C44     ld ($442C), a
44BA C9         ret
44BB 00         nop
44BC 00         nop
44BD 00         nop
44BE 00         nop
44BF 00         nop
44C0 00         nop
44C1 00         nop
44C2 00         nop
44C3 00         nop
44C4 76         halt
44C5 00         nop
44C6 0C         inc c
44C7 91         sub c
44C8 00         nop
44C9 EA
44CA-44CC 008080                    :B BLOCK1
                                    :; WBB block

44CD 80         add a, b            :B save_block1
44CE 00         nop
44CF 00         nop
44D0 00         nop

44D1 010103     ld bc, $0301        :C INTRO_SCR
                                    :#;--------------------------------------------------------------------------------
                                    :#; Draw rotating border and ship
                                    :#;--------------------------------------------------------------------------------
                                    :; row 3, col 1
44D4 CD1242     call $4212          :; HL=screen address
44D7 EB         ex de, hl           :; DE=screen address
44D8 060B       ld b, $0B           :; 10 times+1 for one LDI decrementing B
44DA 21CA44     ld hl, $44CA        :C draw_top_bar
44DD EDA0       ldi                 :; copy block to screen
44DF EDA0       ldi                 :; three times
44E1 EDA0       ldi
44E3 10F5       djnz $44DA          :; 30 chars copied (LDI decs BC, DJNZ decs B)

44E5 3E05       ld a, $05           :; copy 5 blocks of 3 chars
44E7 0603       ld b, $03           :C draw_right_bar
44E9 21CA44     ld hl, $44CA
44EC C5         push bc             :C each_right_bar_block
                                    :; save BC
44ED 012000     ld bc, $0020        :; add 32 to DE, LDI with add 1 more
44F0 EB         ex de, hl
44F1 09         add hl, bc          :; HL+=32
44F2 C1         pop bc
44F3 EB         ex de, hl           :; DE+=32
44F4 EDA0       ldi                 :; DE+=33
44F6 10F4       djnz $44EC          :; copy 3 blocks
44F8 3D         dec a               :; times 5
44F9 FE00       cp $00              :; NOTE: not needed
44FB 20EA       jr nz, $44E7
44FD 012000     ld bc, $0020        :; add 32 to DE
4500 EB         ex de, hl
4501 09         add hl, bc
4502 EB         ex de, hl           :; DE+=32
4503 060A       ld b, $0A           :; 10 blocks of 3 = 30
4505 21CA44     ld hl, $44CA        :C draw_bottom_bar
4508 7E         ld a, (hl)          :; copy 3 chars
4509 12         ld (de), a
450A 23         inc hl
450B 1B         dec de
450C 7E         ld a, (hl)
450D 12         ld (de), a
450E 23         inc hl
450F 1B         dec de
4510 7E         ld a, (hl)
4511 12         ld (de), a
4512 00         nop                 :; NOTE: not needed
4513 1B         dec de
4514 10EF       djnz $4505          :; repeat 10 times
4516 012000     ld bc, $0020        :; subtract 32 from DE
4519 EB         ex de, hl
451A A7         and a
451B ED42       sbc hl, bc          :; DE-=32
451D EB         ex de, hl
451E 3E05       ld a, $05           :; copy 5 blocks of 3 chars
4520 0603       ld b, $03           :C draw_left_bar
4522 21CA44     ld hl, $44CA
4525 F5         push af             :C each_left_bar_block
                                    :; copy one char
4526 7E         ld a, (hl)
4527 12         ld (de), a
4528 F1         pop af
4529 C5         push bc
452A 012100     ld bc, $0021        :; subtract 33 from address
452D EB         ex de, hl
452E A7         and a
452F ED42       sbc hl, bc          :; DE-=33
4531 C1         pop bc
4532 EB         ex de, hl
4533 23         inc hl
4534 10EF       djnz $4525          :; copy 3 blocks
4536 3D         dec a
4537 FE00       cp $00
4539 20E5       jr nz, $4520        :; times 5
453B 3ACA44     ld a, ($44CA)       :#; rotate block
453E 32CD44     ld ($44CD), a
4541 3ACB44     ld a, ($44CB)
4544 32CA44     ld ($44CA), a
4547 3ACC44     ld a, ($44CC)
454A 32CB44     ld ($44CB), a
454D 3ACD44     ld a, ($44CD)
4550 32CC44     ld ($44CC), a
4553 010D0B     ld bc, $0B0D        :; line 11, column 13
4556 C34C43     jp $434C
4559 76         halt
455A 00         nop
455B 0D         dec c
455C 90         sub b
455D 00         nop
455E EA
455F-4563 0081840700				:B ROTATED_SHIP1
4564-4568 8186078281				:B
4569-456D 8207008184				:B
456E-4572 0784818607				:B
4573-4577 0081820700				:B

4578-457C 8180000782				:B ROTATED_SHIP2
457D-4581 8081800780         		:B
4582-4586 8081000780         		:B
4587-458B 8081800780         		:B
458C-4590 8481008007         		:B

4591-4595 0084078200         		:B ROTATED_SHIP3
4596-459A 8281840682       			:B
459B-459F 0782008481         		:B
45A0-45A4 8406820784         		:B
45A5-45A9 0084818200         		:B

45AA-45AE 8180808082         		:B ROTATED_SHIP4
45AF-45B3 8282828280         		:B
45B4-45B8 0080008000         		:B
45B9-45BD 8084848484         		:B
45BE-45C2 8480808007         		:B

45C3 04         inc b					:B rotate_count
45C4 111900     ld de, $0019			:C ROTATE_SHIP
										:; 5*5=25 bytes per ship
45C7 3AC345     ld a, ($45C3)			:; counts 1 to 4
45CA 3C         inc a
45CB FE05       cp $05
45CD 2002       jr nz, $45D1			
45CF 3E01       ld a, $01
45D1 32C345     ld ($45C3), a			:C store_count
45D4 214645     ld hl, $4546			:; address of ship1-25 bytes
45D7 47         ld b, a
45D8 19         add hl, de				:C add_25
45D9 10FD       djnz $45D8
45DB E5         push hl					:; save source address of ship
45DC ED4B4643   ld bc, ($4346)			:; get ship position
45E0 CD1242     call $4212				:; HL = screen position
45E3 EB         ex de, hl				:; DE = screen position
45E4 E1         pop hl					:; HL = source of rotated ship
45E5 C37743     jp $4377
45E8 00         nop
45E9 00         nop
45EA 00         nop
45EB 00         nop
45EC 00         nop
45ED 76         halt
45EE 00         nop
45EF 0E39       ld c, $39
45F1 00         nop
45F2 EA


45F3-4603 0000000000000000340000000000000000    :B torpedo_path
4604 FA45                           :W intro_torpedo_pos
4606 2A0446     ld hl, ($4604)      :C INTRO_FIRE_TORPEDO
                                    :#;--------------------------------------------------------------------------------
                                    :#; Fire intro torpedo
                                    :#;--------------------------------------------------------------------------------
4609 2B         dec hl              :; move back one pos
460A 01F345     ld bc, $45F3
460D A7         and a
460E ED42       sbc hl, bc
4610 3005       jr nc, $4617
4612 21FB45     ld hl, $45FB        :; reached start, go back to end
4615 1801       jr $4618
4617 09         add hl, bc          :C restore_torpedo_pos
4618 E5         push hl             :C draw_torpedo_path
4619 220446     ld ($4604), hl
461C 01120D     ld bc, $0D12        :; row 13, col 18
461F CD1242     call $4212          :; screen pos in HL
4622 D1         pop de                  
4623 EB         ex de, hl           :; DE=screen, HL=torpedo path
4624 010800     ld bc, $0008        :; 1 torpedo and 7 blanks
4627 EDB0       ldir                :; draw
4629 C9         ret


462A 76         halt
462B 00         nop
462C 0F         rrca
462D 2C         inc l
462E 00         nop
462F EA
4630 3A1C       ld a, (fortress_col)    :C DRAW_CANONS
                                    :#;--------------------------------------------------------------------------------
                                    :#; Draw the 4 cannons in front of the fortress
                                    :#;--------------------------------------------------------------------------------
4632 41         ld b, c
4633 D602       sub $02             :; fortress column - 2
4635 4F         ld c, a
4636 0605       ld b, $05           :; row 5
4638 CD4746     call $4647
463B 00         nop
463C CD4746     call $4647
463F CD4746     call $4647
4642 00         nop
4643 CD4746     call $4647
4646 C9         ret
4647 CD1242     call $4212          :C draw_cannon
                                    :; draw cannon at roc-col BC
464A 3681       ld (hl), $81        :; top part of cannon
464C 112100     ld de, $0021
464F 19         add hl, de          :; add 33
4650 3602       ld (hl), $02        :; bottom part of cannon
4652 04         inc b               :; leave 4 rows interval
4653 04         inc b
4654 04         inc b
4655 04         inc b
4656 C9         ret

4657 00         nop
4658 00         nop
4659 00         nop
465A 76         halt
465B 00         nop
465C 103B       djnz $4699
465E 00         nop
465F EA
4660 00                             :B bomb_col
4661 11                             :B bomb_row
4662 00         nop                 :B bomb_running

4663 3A6246     ld a, ($4662)       :C FIRE_BOMB
                                    :#;--------------------------------------------------------------------------------
                                    :#; select bomb position from fortress and ship position
                                    :#;--------------------------------------------------------------------------------
4666 FE01       cp $01
4668 C8         ret z               :; exit if bomb is still running
4669 3A1C41     ld a, ($411C)
466C D603       sub $03
466E 6F         ld l, a             :; L=fortress-col - 3
466F 3A4743     ld a, ($4347)       
4672 67         ld h, a             :; H=ship row
4673 3E05       ld a, $05
4675 94         sub h
4676 3804       jr c, $467C
4678 2605       ld h, $05           :; fire from row 5
467A 1814       jr $4690
467C 3E09       ld a, $09           :C not_cannon1
467E 94         sub h
467F 3804       jr c, $4685
4681 2609       ld h, $09           :; fire from row 9
4683 180B       jr $4690
4685 3E0D       ld a, $0D           :C not_cannon2
4687 94         sub h
4688 3804       jr c, $468E
468A 260D       ld h, $0D           :; fire from row 13
468C 1802       jr $4690
468E 2611       ld h, $11           :C not_cannon3
                                    :; fire from row 17
4690 226046     ld ($4660), hl      :C do_fire_bomb
4693 3E01       ld a, $01
4695 326246     ld ($4662), a
4698 C9         ret

4699 76         halt
469A 00         nop
469B 113800     ld de, $0038
469E EA
469F 00                         	:B SHIP_HIT
46A0 AF     jp pe, $AF00        	:C DRAW_BOMB
46A1 329F46     ld ($469F), a		:; clear ship-hit
46A4 ED4B6046   ld bc, ($4660)		:; get position of bomb
46A8 CD1242     call $4212			:; HL = screen address of bomb
46AB 3600       ld (hl), $00		:; delete bomb
46AD AF         xor a
46AE B9         cp c				:; compare column with zero
46AF 2004       jr nz, $46B5		:; jump forward if not at col zero
46B1 326246     ld ($4662), a		:; at col zero, clear bomb-running
46B4 C9         ret
46B5 0D         dec c				:C move_bomb
									:; move bomb left in column
46B6 2B         dec hl				:; and screen address
46B7 BE         cp (hl)				:; did we hit something?
46B8 2805       jr z, $46BF			:; jump forward if not
46BA 3E34       ld a, $34		
46BC BE         cp (hl)				:; did we hit the torpedo?
46BD 2007       jr nz, $46C6		:; jump if not, we hit the ship
46BF 3604       ld (hl), $04		:C bomb_no_hit
									:; draw bomb
46C1 ED436046   ld ($4660), bc
46C5 C9         ret
46C6 34         inc (hl)			:C bomb_hit_ship
									:; show rubish on hit-point
46C7 32C646     ld ($46C6), a		:; NOTE: should not be here
									:; writes a $34 on the code address, which is inc (hl)
46CA 3E01       ld a, $01
46CC 329F46     ld ($469F), a
46CF C9         ret
46D0 00         nop
46D1 00         nop
46D2 00         nop
46D3 00         nop
46D4 00         nop
46D5 76         halt
46D6 00         nop
46D7 12         ld (de), a
46D8 43         ld b, e
46D9 00         nop
46DA EA
46DB 6100                           :W timer_fortress_move
                                    :; count times until fortress moves
46DD 2ADB46     ld hl, ($46DB)      :C CHECK_FORTRESS_MOVE
                                    :#;--------------------------------------------------------------------------------
                                    :#; advance timer and move fortress left if it reaches $C8
                                    :#;--------------------------------------------------------------------------------
46E0 23         inc hl
46E1 11C800     ld de, $00C8		:C FORTRESS_MOVE_COUNT
46E4 A7         and a
46E5 ED52       sbc hl, de
46E7 2805       jr z, $46EE
46E9 19         add hl, de
46EA 22DB46     ld ($46DB), hl
46ED C9         ret                 :; not time yet to move
46EE 210000     ld hl, $0000        :C do_fortress_move
46F1 22DB46     ld ($46DB), hl      :; reset timer
46F4 CDE147     call $47E1
46F7 ED4B1C41   ld bc, ($411C)
46FB 0D         dec c               :; move fortress one position left
46FC ED431C41   ld ($411C), bc
4700 0D         dec c
4701 0D         dec c
4702 CD1242     call $4212          :; minus 2=cannons column
4705 3E14       ld a, $14           :; 20 lines
4707 010D00     ld bc, $000D        :C move_next_row
                                    :; 13 chars = fortress+cannons
470A 54         ld d, h             :; destintion
470B 5D         ld e, l
470C 23         inc hl              :; source=destination+1
470D EDB0       ldir                :; move row
470F 2B         dec hl
4710 3600       ld (hl), $00        :; delete char to the right
4712 011400     ld bc, $0014        :; move to next line
4715 09         add hl, bc
4716 3D         dec a
4717 FE00       cp $00
4719 20EC       jr nz, $4707        :; next row
471B C9         ret
471C 76         halt
471D 00         nop
471E 13         inc de
471F 6D         ld l, l
4720 00         nop
4721 EA
4722 3804                       :W SCORE
4724-4728						:B NUM_BUFFER
4729 1027       				:W N10000
472B E803 						:W N1000
472D 6400						:W N100
472F 0A00						:W N10
4731 0100						:W N1
4733 00							:W PRINT_ACC
4734 00         nop
4735 01							:B HAVE_DIGITS
4736 AF			xor a			:C SCORE_I2A
								:#; Note: does not work for 0, outputs all blanks
4737 323547     ld ($4735), a
473A 2A2247     ld hl, ($4722)	:; get SCORE
473D 223347     ld ($4733), hl	:; put in number to print
4740 012447     ld bc, $4724	:; output buffer
4743 212947     ld hl, $4729	:; table of divisors
4746 E5         push hl			:; save address of table
4747 AF         xor a			:C next_divisor
4748 E1         pop hl			:; HL=next divisor address
4749 5E         ld e, (hl)		
474A 23         inc hl
474B 56         ld d, (hl)		:; DE=next divisor
474C 2B         dec hl			:; restore HL
474D E5         push hl			:; save HL
474E 2A3347     ld hl, ($4733)	:C subtract_divisor
								:; get accumulator
4751 A7         and a
4752 ED52       sbc hl, de		:; subtract divisor
4754 223347     ld ($4733), hl	:; store accumulator
4757 3803       jr c, $475C		:; jump forward if overflowed
4759 3C         inc a			:; count one more
475A 18F2       jr $474E
475C 19         add hl, de		:C subtraction_overflowed
								:; restore accumulator
475D FE00       cp $00
475F 2009       jr nz, $476A	:; jump forward if not zero
4761 3A3547     ld a, ($4735)	:; if this digit is 0 and have_digits is 0, output a space
4764 FE00       cp $00
4766 3E00       ld a, $00
4768 2809       jr z, $4773
476A C61C       add a, $1C		:C make_digit
								:; add '0'
476C F5         push af
476D 3E01       ld a, $01
476F 323547     ld ($4735), a
4772 F1         pop af
4773 223347     ld ($4733), hl	:C output_digit
								:; restore accumulator
4776 02         ld (bc), a		:; store digit
4777 03         inc bc
4778 E1         pop hl			:; advance in divisor table
4779 23         inc hl
477A 23         inc hl
477B E5         push hl
477C 7B         ld a, e
477D FE01       cp $01			:; check for last divisor
477F 20C6       jr nz, $4747
4781 E1         pop hl
4782 C9         ret
4783 00         nop
4784 00         nop
4785 00         nop
4786 00         nop
4787 00         nop
4788 00         nop
4789 00         nop
478A 00         nop
478B 00         nop
478C 00         nop
478D 76         halt
478E 00         nop
478F 14         inc d
4790 4A         ld c, d
4791 00         nop
4792 EA
4793 3A5F     jp pe, $5F3A          :C INCR_SCORE
4795 44         ld b, h
4796 FE01       cp $01
4798 C8         ret z
4799 3A5E44     ld a, ($445E)
479C FE00       cp $00
479E C8         ret z
479F AF         xor a
47A0 325E44     ld ($445E), a
47A3 110300     ld de, $0003
47A6 2A2F44     ld hl, ($442F)
47A9 2600       ld h, $00
47AB A7         and a
47AC ED52       sbc hl, de
47AE EB         ex de, hl
47AF 2A2247     ld hl, ($4722)
47B2 19         add hl, de          :C score_multiply
47B3 19         add hl, de
47B4 19         add hl, de
47B5 19         add hl, de
47B6 19         add hl, de
47B7 222247     ld ($4722), hl		:; save score
47BA CD3647     call $4736			:; convert to digit sequence
47BD 2A0C40     ld hl, ($400C)		:; display
47C0 111200     ld de, $0012		:; add position of score
47C3 19         add hl, de
47C4 EB         ex de, hl			:; to DE
47C5 212447     ld hl, $4724		:; HL=number buffer
47C8 0605       ld b, $05			:; 5 digits
47CA 7E         ld a, (hl)			:C out_digit
47CB FE00       cp $00				:; is it a blank?
47CD 2806       jr z, $47D5
47CF EDA0       ldi					:; copy digit
47D1 03         inc bc				:; restore BC
47D2 10F6       djnz $47CA			:; next digit
47D4 C9         ret
47D5 23         inc hl				:C skip_blanks
47D6 10F2       djnz $47CA			:; next digit
47D8 C9         ret
47D9 00         nop
47DA 00         nop
47DB 76         halt
47DC 00         nop
47DD 15         dec d
47DE 1A         ld a, (de)
47DF 00         nop
47E0 EA
47E1 ED4B2D44                   :C DELETE_TORPEDO
47E5 CD1242     call $4212
47E8 3E34       ld a, $34       :; does position has a torpedo?
47EA BE         cp (hl)
47EB C0         ret nz          :; ret if not
47EC 3600       ld (hl), $00    :; delete torpedo
47EE C9         ret

47EF EDA06046   ld bc, ($4660)  :C DELETE_BOMB
47F3 CD1242     call $4212
47F6 3600       ld (hl), $00    :; delete bomb
47F8 C9         ret

47F9 76         halt
47FA 00         nop
47FB 1642       ld d, $42
47FD 00         nop
47FE EA
47FF 063A       ld b,$3A            :C INIT_FORTRESS
                                    :#;--------------------------------------------------------------------------------
                                    :#; Init restored fortress
                                    :#;--------------------------------------------------------------------------------
                                    :; 58 chars black
4801 218240     ld hl, $4082
4804 3680       ld (hl), $80        :C fortress_fill_outer_black
4806 23         inc hl
4807 10FB       djnz $4804
4809 0632       ld b, $32           :; 50 chars grey
480B 3608       ld (hl), $08        :C fortress_fill_grey
480D 23         inc hl
480E 10FB       djnz $480B
4810 062A       ld b, $2A           :; 42 chars black
4812 3680       ld (hl), $80        :C fortress_fill_inner_black
4814 23         inc hl
4815 10FB       djnz $4812
4817 211502     ld hl, $0215
481A 221C41     ld ($411C), hl
481D 210000     ld hl, $0000
4820 22DB46     ld ($46DB), hl
4823 225E44     ld ($445E), hl
4826 210009     ld hl, $0900
4829 224643     ld ($4346), hl
482C 210001     ld hl, $0100
482F 226046     ld ($4660), hl
4832 AF         xor a
4833 322C44     ld ($442C), a
4836 326246     ld ($4662), a
4839 329F46     ld ($469F), a
483C C9         ret
483D 00         nop
483E 00         nop
483F 76         halt
4840 00         nop
4841 17         rla
4842 3F         ccf
4843 00         nop
4844 EA
4845-485D                           :B press_any_key_msg

484E 2A39       ld sp, $392A
4850 39         add hl, sp
4851 2A3700     ld hl, ($0037)
4854 3C         inc a
4855 2D         dec l
4856 2A3300     ld hl, ($0033)
4859 37         scf
485A 2A2629     ld hl, ($2926)
485D 3E
485E 010317     ld bc, $1703        :C PRESS_ANY_KEY
                                    :; row 23, col 3
                                    :#;--------------------------------------------------------------------------------
                                    :#; Print message "Press any key"
                                    :#;--------------------------------------------------------------------------------
4861 CD1242     call $4212
4864 EB         ex de, hl           :; DE=screen address
4865 214548     ld hl, $4845
4868 011900     ld bc, $0019
486B EDB0       ldir
486D C9         ret

486E 00         nop
486F 00         nop
4870 00         nop

4871 010317     ld bc, $1703        :C DELETE_PRESS_ANY_KEY
                                    :; row 23, col 3
4874 CD1242     call $4212
4877 0619       ld b, $19
4879 3600       ld (hl), $00        :C put_space
487B 23         inc hl
487C 10FB       djnz $4879
487E C9         ret

487F 00         nop
4880 00         nop
4881 00         nop
4882 76         halt
4883 00         nop
4884 189B       jr $4821
4886 00         nop
4887 EA0085     jp pe, $8500
488A 018500     ld bc, $0085
488D 00         nop
488E 02         ld (bc), a
488F 80         add a, b
4890 03         inc bc
4891 00         nop
4892 00         nop
4893 87         add a, a
4894 80         add a, b
4895 04         inc b
4896 00         nop
4897 00         nop
4898 81         add a, c
4899 00         nop
489A 82         add a, d
489B 00         nop
489C 00         nop
489D 00         nop
489E 00         nop
489F 00         nop
48A0 00         nop
48A1 87         add a, a
48A2 07         rlca
48A3 80         add a, b
48A4 84         add a, h
48A5 04         inc b
48A6 00         nop
48A7 84         add a, h
48A8 03         inc bc
48A9 07         rlca
48AA 00         nop
48AB 00         nop
48AC 02         ld (bc), a
48AD 80         add a, b
48AE 010000     ld bc, $0000
48B1 00         nop
48B2 00         nop                 
48B3 00         nop
48B4 00         nop
48B5 01
48B6 ED4B1C41   ld bc, ($411C)			:C FLASH_ALIEN
48BA 0C         inc c
48BB 0C         inc c
48BC 0C         inc c					:; fortress-col+3
48BD 0605       ld b, $05				:; row=5
48BF CD1242     call $4212				:; HL=screen address
48C2 111C00     ld de, $001C			:; distance between rows
48C5 3E0E       ld a, $0E				:; 14 lines
48C7 F5         push af					:C flash_next_line
										:; save line counter
48C8 0605       ld b, $05				:; 5 columns
48CA 7E         ld a, (hl)				:C flash_next_col
										:; get screen char
48CB EE80       xor $80					:; invert it
48CD 77         ld (hl), a				:; and store
48CE 23         inc hl
48CF 10F9       djnz $48CA				:; loop for all columns
48D1 19         add hl, de				:; next row
48D2 F1         pop af					:; get row count
48D3 3D         dec a
48D4 FE00       cp $00
48D6 20EF       jr nz, $48C7			:; loop for next row
48D8 C9         ret
48D9 EB         ex de, hl
48DA 219C48     ld hl, $489C
48DD C37743     jp $4377
48E0 3E02       ld a, $02
48E2 BE         cp (hl)
48E3 201F       jr nz, $4904
48E5 3C         inc a
48E6 32B548     ld ($48B5), a
48E9 EB         ex de, hl
48EA 23         inc hl
48EB 3605       ld (hl), $05
48ED 23         inc hl
48EE 3602       ld (hl), $02
48F0 23         inc hl
48F1 3605       ld (hl), $05
48F3 111F00     ld de, $001F
48F6 19         add hl, de
48F7 3603       ld (hl), $03
48F9 23         inc hl
48FA 23         inc hl
48FB 3601       ld (hl), $01
48FD 11A300     ld de, $00A3
4900 19         add hl, de
4901 3680       ld (hl), $80
4903 C9         ret
4904 3E01       ld a, $01
4906 32B548     ld ($48B5), a
4909 EB         ex de, hl
490A 23         inc hl
490B 23         inc hl
490C 3600       ld (hl), $00
490E 23         inc hl
490F 3685       ld (hl), $85
4911 112100     ld de, $0021
4914 19         add hl, de
4915 3603       ld (hl), $03
4917 11A500     ld de, $00A5
491A 19         add hl, de
491B 3680       ld (hl), $80
491D C9         ret
491E 00         nop
491F 00         nop
4920 00         nop
4921 76         halt
4922 00         nop
4923 19         add hl, de
4924 24         inc h
4925 00         nop
4926 EAED4B     jp pe, $4BED
4929 1C         inc e
492A 41         ld b, c
492B 0C         inc c
492C 0C         inc c
492D 0C         inc c
492E 0605       ld b, $05
4930 ED434643   ld ($4346), bc
4934 CD0544     call $4405
4937 060E       ld b, $0E
4939 ED434643   ld ($4346), bc
493D CD0544     call $4405
4940 C9         ret
4941 00         nop
4942 00         nop
4943 00         nop
4944 00         nop
4945 00         nop
4946 00         nop
4947 00         nop
4948 00         nop
4949 76         halt
494A 00         nop
494B 1A         ld a, (de)
494C 42         ld b, d
494D 00         nop
494E EA
494F 0100                       :W LEVEL_DELAY
4951 CDDD46     call $46DD      :C GAME
                                    :#;--------------------------------------------------------------------------------
                                    :#; Game loop
                                    :#;--------------------------------------------------------------------------------
4954 CDD149     call $49D1
4957 CD9F42     call $429F
495A CD2C41     call $412C
495D CD6044     call $4460
4960 CD9347     call $4793
4963 3A5F44     ld a, ($445F)
4966 FE01       cp $01
4968 C8         ret z
4969 CD3046     call $4630
496C CDA046     call $46A0
496F 3A9F46     ld a, ($469F)
4972 FE01       cp $01
4974 C8         ret z
4975 CD6346     call $4663
4978 2A4F49     ld hl, ($494F)
497B 110000     ld de, $0000
497E 13         inc de          :C game_delay
497F A7         and a
4980 ED52       sbc hl, de
4982 CA5149     jp z, $4951
4985 19         add hl, de
4986 18F6       jr $497E
4988 00         nop
4989 00         nop
498A 00         nop
498B 00         nop
498C 00         nop
498D 00         nop
498E 00         nop
498F 76         halt
4990 00         nop
4991 1B         dec de
4992 1A         ld a, (de)
4993 00         nop
4994 EA3EBF     jp pe, $BF3E
4997 BD         cp l
4998 2806       jr z, $49A0
499A 3E7F       ld a, $7F
499C BD         cp l
499D C2F043     jp nz, $43F0
49A0 3A1C41     ld a, ($411C)
49A3 C3C543     jp $43C5
49A6 00         nop
49A7 00         nop
49A8 00         nop
49A9 00         nop
49AA 00         nop
49AB 00         nop
49AC 00         nop
49AD 76         halt
49AE 00         nop
49AF 1C         inc e
49B0 1800       jr $49B2
49B2 EA
49B3 010002     ld bc, $0200			:C CLEAR_GAME_AREA
										:; row 2 column 0
49B6 CD1242     call $4212				:; HL = screen address
49B9 3E14       ld a, $14				:; 20 rows
49BB 0620       ld b, $20				:C clear_next_row
										:; 32 chars
49BD 3600       ld (hl), $00			:; clear char
49BF 23         inc hl
49C0 10FB       djnz $49BD				:; loop for columns
49C2 23         inc hl
49C3 3D         dec a
49C4 FE00       cp $00
49C6 20F3       jr nz, $49BB			:; loop for rows
49C8 C9         ret
49C9 76         halt
49CA 00         nop
49CB 1D         dec e
49CC 70         ld (hl), b
49CD 00         nop
49CE EA
49CF DFFB                           :W pressed_key
49D1 CDBB02     call $02BB          :C TEST_KEYBOARD
                                    :#;--------------------------------------------------------------------------------
                                    :#; Test keyboard and do actions
                                    :#;--------------------------------------------------------------------------------
                                    :; scan keyboard, H=column, L=half-row
49D4 ED4B4643   ld bc, ($4346)      :C
49D8 22CF49     ld ($49CF), hl
49DB 11FFFF     ld de, $FFFF
49DE A7         and a
49DF ED52       sbc hl, de
49E1 C8         ret z               :; no key pressed, return
49E2 2ACF49     ld hl, ($49CF)
49E5 CB65       bit 4, l            :; check top right half-row
49E7 CC3144     call z, $4431       :; key pressed - fire torpedo
49EA 2ACF49     ld hl, ($49CF)
49ED ED4B4643   ld bc, ($4346)
49F1 CB6D       bit 5, l            :; check 2nd right half-row
49F3 CC3144     call z, $4431       :; key pressed - fire torpedo
49F6 2ACF49     ld hl, ($49CF)
49F9 ED4B4643   ld bc, ($4346)
49FD CB5D       bit 3, l            :; check top left half-row
49FF CADB43     jp z, $43DB         :; key pressed - move ship up
4A02 CB55       bit 2, l            :; check second left half-row
4A04 CADB43     jp z, $43DB         :; key pressed - move ship up
4A07 CB45       bit 0, l            :; check bottom left half-row
4A09 CAD043     jp z, $43D0         :; key pressed - move ship down
4A0C CB4D       bit 1, l            :; check third left half-row
4A0E CAD043     jp z, $43D0         :; key pressed - move ship down
4A11 CB75       bit 6, l            :; check thrird right half-row
4A13 2803       jr z, $4A18         :; key pressed - move ship left or right
4A15 CB7D       bit 7, l            :; check bottom right half-row
4A17 C0         ret nz              :; not pressed - return
4A18 00         nop                 :C check_left_right
4A19 00         nop
4A1A 00         nop
4A1B 00         nop
4A1C 00         nop
4A1D CB6C       bit 5, h            :; check move ship left
4A1F CAE643     jp z, $43E6
4A22 CB64       bit 4, h            :; check move ship left
4A24 CAE643     jp z, $43E6
4A27 CB5C       bit 3, h            :; check move ship left
4A29 CAE643     jp z, $43E6
4A2C CB54       bit 2, h            :; check move ship right
4A2E CAC243     jp z, $43C2
4A31 CB4C       bit 1, h            :; check move ship right
4A33 CAC243     jp z, $43C2
4A36 C9         ret
4A37 00         nop
4A38 00         nop
4A39 00         nop
4A3A 00         nop
4A3B 00         nop
4A3C 00         nop
4A3D 76         halt
4A3E 00         nop
4A3F 1E02       ld e, $02
4A41 00         nop
4A42 FB         ei
4A43 76         halt
4A44 00         nop
4A45 221700     ld ($0017), hl
4A48 F41D23     call p, $231D
4A4B 25         dec h
4A4C 1E20       ld e, $20
4A4E 7E         ld a, (hl)
4A4F 8F         adc a, a
4A50 0C         inc c
4A51 08         ex af, af'
4A52 00         nop
4A53 00         nop
4A54 1A         ld a, (de)
4A55 1E21       ld e, $21
4A57 1E7E       ld e, $7E
4A59 88         adc a, b
4A5A 7C         ld a, h
4A5B 00         nop
4A5C 00         nop
4A5D 00         nop
4A5E 76         halt
4A5F 00         nop
4A60 23         inc hl
4A61 1600       ld d, $00
4A63 F41D23     call p, $231D
4A66 25         dec h
4A67 1E21       ld e, $21
4A69 7E         ld a, (hl)
4A6A 8F         adc a, a
4A6B 0C         inc c
4A6C 0A         ld a, (bc)
4A6D 00         nop
4A6E 00         nop
4A6F 1A         ld a, (de)
4A70 22257E     ld ($7E25), hl
4A73 87         add a, a
4A74 0A         ld a, (bc)
4A75 00         nop
4A76 00         nop
4A77 00         nop
4A78 76         halt
4A79 00         nop
4A7A 2821       jr z, $4A9D
4A7C 00         nop
4A7D F5         push af
4A7E C1         pop bc
4A7F 217E83     ld hl, $837E
4A82 2000       jr nz, $4A84
4A84 00         nop
4A85 00         nop
4A86 1A         ld a, (de)
4A87 1D         dec e
4A88 1D         dec e
4A89 7E         ld a, (hl)
4A8A 84         add a, h
4A8B 3000       jr nc, $4A8D
4A8D 00         nop
4A8E 00         nop
4A8F 19         add hl, de
4A90 0B         dec bc
4A91 3C         inc a
4A92 2A3128     ld hl, ($2831)
4A95 34         inc (hl)
4A96 322A00     ld ($002A), a
4A99 39         add hl, sp
4A9A 34         inc (hl)
4A9B 0B         dec bc
4A9C 19         add hl, de
4A9D 76         halt
4A9E 00         nop
4A9F 322C00     ld ($002C), a
4AA2 F5         push af
4AA3 C1         pop bc
4AA4 23         inc hl
4AA5 7E         ld a, (hl)
4AA6 83         add a, e
4AA7 60         ld h, b
4AA8 00         nop
4AA9 00         nop
4AAA 00         nop
4AAB 1A         ld a, (de)
4AAC 217E83     ld hl, $837E
4AAF 2000       jr nz, $4AB1
4AB1 00         nop
4AB2 00         nop
4AB3 19         add hl, de
4AB4 0B         dec bc
4AB5 0603       ld b, $03
4AB7 86         add a, (hl)
4AB8 00         nop
4AB9 03         inc bc
4ABA 07         rlca
4ABB 010284     ld bc, $8402
4ABE 03         inc bc
4ABF 00         nop
4AC0 0603       ld b, $03
4AC2 86         add a, (hl)
4AC3 00         nop
4AC4 0603       ld b, $03
4AC6 86         add a, (hl)
4AC7 00         nop
4AC8 05         dec b
4AC9 87         add a, a
4ACA 010B19     ld bc, $190B
4ACD 76         halt
4ACE 00         nop
4ACF 3C         inc a
4AD0 2B         dec hl
4AD1 00         nop
4AD2 F5         push af
4AD3 C1         pop bc
4AD4 24         inc h
4AD5 7E         ld a, (hl)
4AD6 84         add a, h
4AD7 00         nop
4AD8 00         nop
4AD9 00         nop
4ADA 00         nop
4ADB 1A         ld a, (de)
4ADC 217E83     ld hl, $837E
4ADF 2000       jr nz, $4AE1
4AE1 00         nop
4AE2 00         nop
4AE3 19         add hl, de
4AE4 0B         dec bc
4AE5 82         add a, d
4AE6 83         add a, e
4AE7 81         add a, c
4AE8 00         nop
4AE9 00         nop
4AEA 05         dec b
4AEB 00         nop
4AEC 00         nop
4AED 85         add a, l
4AEE 00         nop
4AEF 00         nop
4AF0 82         add a, d
4AF1 83         add a, e
4AF2 81         add a, c
4AF3 00         nop
4AF4 05         dec b
4AF5 00         nop
4AF6 00         nop
4AF7 00         nop
4AF8 07         rlca
4AF9 86         add a, (hl)
4AFA 0B         dec bc
4AFB 19         add hl, de
4AFC 76         halt
4AFD 00         nop
4AFE 46         ld b, (hl)
4AFF 2D         dec l
4B00 00         nop
4B01 F5         push af
4B02 C1         pop bc
4B03 25         dec h
4B04 7E         ld a, (hl)
4B05 84         add a, h
4B06 1000       djnz $4B08
4B08 00         nop
4B09 00         nop
4B0A 1A         ld a, (de)
4B0B 217E83     ld hl, $837E
4B0E 2000       jr nz, $4B10
4B10 00         nop
4B11 00         nop
4B12 19         add hl, de
4B13 0B         dec bc
4B14 05         dec b
4B15 00         nop
4B16 85         add a, l
4B17 00         nop
4B18 00         nop
4B19 05         dec b
4B1A 00         nop
4B1B 00         nop
4B1C 85         add a, l
4B1D 00         nop
4B1E 00         nop
4B1F 05         dec b
4B20 00         nop
4B21 85         add a, l
4B22 00         nop
4B23 86         add a, (hl)
4B24 83         add a, e
4B25 0600       ld b, $00
4B27 05         dec b
4B28 00         nop
4B29 86         add a, (hl)
4B2A 0B         dec bc
4B2B 19         add hl, de
4B2C 00         nop
4B2D 76         halt
4B2E 00         nop
4B2F 50         ld d, b
4B30 3000       jr nc, $4B32
4B32 F5         push af
4B33 C1         pop bc
4B34 1D         dec e
4B35 23         inc hl
4B36 7E         ld a, (hl)
4B37 85         add a, l
4B38 08         ex af, af'
4B39 00         nop
4B3A 00         nop
4B3B 00         nop
4B3C 1A         ld a, (de)
4B3D 1F         rra
4B3E 7E         ld a, (hl)
4B3F 82         add a, d
4B40 40         ld b, b
4B41 00         nop
4B42 00         nop
4B43 00         nop
4B44 19         add hl, de
4B45 0B         dec bc
4B46 2834       jr z, $4B7C
4B48 35         dec (hl)
4B49 37         scf
4B4A 1B         dec de
4B4B 1A         ld a, (de)
4B4C 00         nop
4B4D 35         dec (hl)
4B4E 263A       ld h, $3A
4B50 310028     ld sp, $2800
4B53 2637       ld h, $37
4B55 313834     ld sp, $3438
4B58 33         inc sp
4B59 1A         ld a, (de)
4B5A 00         nop
4B5B 1D         dec e
4B5C 25         dec h
4B5D 24         inc h
4B5E 1E0B       ld e, $0B
4B60 19         add hl, de
4B61 76         halt
4B62 00         nop
4B63 5A         ld e, d
4B64 3600       ld (hl), $00
4B66 F5         push af
4B67 C1         pop bc
4B68 1E1D       ld e, $1D
4B6A 7E         ld a, (hl)
4B6B 85         add a, l
4B6C 2800       jr z, $4B6E
4B6E 00         nop
4B6F 00         nop
4B70 1A         ld a, (de)
4B71 1C         inc e
4B72 7E         ld a, (hl)
4B73 00         nop
4B74 00         nop
4B75 00         nop
4B76 00         nop
4B77 00         nop
4B78 19         add hl, de
4B79 0B         dec bc
4B7A 29         add hl, hl
4B7B 34         inc (hl)
4B7C 00         nop
4B7D 3E34       ld a, $34
4B7F 3A003C     ld a, ($3C00)
4B82 2633       ld h, $33
4B84 39         add hl, sp
4B85 00         nop
4B86 2E33       ld l, $33
4B88 3839       jr c, $4BC3
4B8A 37         scf
4B8B 3A2839     ld a, ($3928)
4B8E 2E34       ld l, $34
4B90 33         inc sp
4B91 380F       jr c, $4BA2
4B93 00         nop
4B94 103E       djnz $4BD4
4B96 1833       jr $4BCB
4B98 110B19     ld de, $190B
4B9B 76         halt
4B9C 00         nop
4B9D 64         ld h, h
4B9E 17         rla
4B9F 00         nop
4BA0 F41D23     call p, $231D
4BA3 1D         dec e
4BA4 21247E     ld hl, $7E24
4BA7 8F         adc a, a
4BA8 060C       ld b, $0C
4BAA 00         nop
4BAB 00         nop
4BAC 1A         ld a, (de)
4BAD 1D         dec e
4BAE 1E24       ld e, $24
4BB0 7E         ld a, (hl)
4BB1 88         adc a, b
4BB2 00         nop
4BB3 00         nop
4BB4 00         nop
4BB5 00         nop
4BB6 76         halt
4BB7 00         nop
4BB8 6E         ld l, (hl)
4BB9 17         rla
4BBA 00         nop
4BBB F41D23     call p, $231D
4BBE 1D         dec e
4BBF 24         inc h
4BC0 1F         rra
4BC1 7E         ld a, (hl)
4BC2 8F         adc a, a
4BC3 063E       ld b, $3E
4BC5 00         nop
4BC6 00         nop
4BC7 1A         ld a, (de)
4BC8 1D         dec e
4BC9 1E24       ld e, $24
4BCB 7E         ld a, (hl)
4BCC 88         adc a, b
4BCD 00         nop
4BCE 00         nop
4BCF 00         nop
4BD0 00         nop
4BD1 76         halt
4BD2 00         nop
4BD3 78         ld a, b
4BD4 17         rla
4BD5 00         nop
4BD6 F41D23     call p, $231D
4BD9 1E1C       ld e, $1C
4BDB 24         inc h
4BDC 7E         ld a, (hl)
4BDD 8F         adc a, a
4BDE 0670       ld b, $70
4BE0 00         nop
4BE1 00         nop
4BE2 1A         ld a, (de)
4BE3 1D         dec e
4BE4 1E24       ld e, $24
4BE6 7E         ld a, (hl)
4BE7 88         adc a, b
4BE8 00         nop
4BE9 00         nop
4BEA 00         nop
4BEB 00         nop
4BEC 76         halt
4BED 00         nop
4BEE 8C         adc a, h
4BEF 1000       djnz $4BF1
4BF1 F1         pop af
4BF2 3114D4     ld sp, $D414
4BF5 1D         dec e
4BF6 23         inc hl
4BF7 221D23     ld ($231D), hl
4BFA 7E         ld a, (hl)
4BFB 8F         adc a, a
4BFC 09         add hl, bc
4BFD A2         and d
4BFE 00         nop
4BFF 00         nop
4C00 76         halt
4C01 00         nop
4C02 96         sub (hl)
4C03 1000       djnz $4C05
4C05 F1         pop af
4C06 3114D4     ld sp, $D414
4C09 1D         dec e
4C0A 23         inc hl
4C0B 25         dec h
4C0C 1E22       ld e, $22
4C0E 7E         ld a, (hl)
4C0F 8F         adc a, a
4C10 0C         inc c
4C11 0C         inc c
4C12 00         nop
4C13 00         nop
4C14 76         halt
4C15 00         nop
4C16 9B         sbc a, e
4C17 1100FA     ld de, $FA00
4C1A 41         ld b, c
4C1B 14         inc d
4C1C 0B         dec bc
4C1D 0B         dec bc
4C1E DEEC       sbc a, $EC
4C20 1D         dec e
4C21 201C       jr nz, $4C3F
4C23 7E         ld a, (hl)
4C24 88         adc a, b
4C25 0C         inc c
4C26 00         nop
4C27 00         nop
4C28 00         nop
4C29 76         halt
4C2A 00         nop
4C2B A0         and b
4C2C 12         ld (de), a
4C2D 00         nop
4C2E FA4114     jp m, $1441
4C31 0B         dec bc
4C32 33         inc sp
4C33 0B         dec bc
4C34 DEEC       sbc a, $EC
4C36 1D         dec e
4C37 25         dec h
4C38 1C         inc e
4C39 7E         ld a, (hl)
4C3A 88         adc a, b
4C3B 3E00       ld a, $00
4C3D 00         nop
4C3E 00         nop
4C3F 76         halt
4C40 00         nop
4C41 AA         xor d
4C42 12         ld (de), a
4C43 00         nop
4C44 FA41DD     jp m, $DD41
4C47 0B         dec bc
4C48 3E0B       ld a, $0B
4C4A DEEC       sbc a, $EC
4C4C 1D         dec e
4C4D 201C       jr nz, $4C6B
4C4F 7E         ld a, (hl)
4C50 88         adc a, b
4C51 0C         inc c
4C52 00         nop
4C53 00         nop
4C54 00         nop
4C55 76         halt
4C56 00         nop
4C57 B4         or h
4C58 0C         inc c
4C59 00         nop
4C5A ED         defb $ED
4C5B 1D         dec e
4C5C 1C         inc e
4C5D 1C         inc e
4C5E 1C         inc e
4C5F 7E         ld a, (hl)
4C60 8A         adc a, d
4C61 7A         ld a, d
4C62 00         nop
4C63 00         nop
4C64 00         nop
4C65 76         halt
4C66 00         nop
4C67 BE         cp (hl)
4C68 0C         inc c
4C69 00         nop
4C6A ED         defb $ED
4C6B 1E1C       ld e, $1C
4C6D 1C         inc e
4C6E 1C         inc e
4C6F 7E         ld a, (hl)
4C70 8B         adc a, e
4C71 7A         ld a, d
4C72 00         nop
4C73 00         nop
4C74 00         nop
4C75 76         halt
4C76 00         nop
4C77 C30C00     jp $000C
4C7A F1         pop af
4C7B 3C         inc a
4C7C 14         inc d
4C7D 1E24       ld e, $24
4C7F 7E         ld a, (hl)
4C80 85         add a, l
4C81 60         ld h, b
4C82 00         nop
4C83 00         nop
4C84 00         nop
4C85 76         halt
4C86 00         nop
4C87 C8         ret z
4C88 0B         dec bc
4C89 00         nop
4C8A F1         pop af
4C8B 3814       jr c, $4CA1
4C8D 1C         inc e
4C8E 7E         ld a, (hl)
4C8F 00         nop
4C90 00         nop
4C91 00         nop
4C92 00         nop
4C93 00         nop
4C94 76         halt
4C95 00         nop
4C96 CA0D00     jp z, $000D
4C99 F1         pop af
4C9A 37         scf
4C9B 14         inc d
4C9C 1E1C       ld e, $1C
4C9E 1C         inc e
4C9F 7E         ld a, (hl)
4CA0 88         adc a, b
4CA1 48         ld c, b
4CA2 00         nop
4CA3 00         nop
4CA4 00         nop
4CA5 76         halt
4CA6 00         nop
4CA7 CD0F00     call $000F
4CAA F41D24     call p, $241D
4CAD 1D         dec e
4CAE 2022       jr nz, $4CD2
4CB0 7E         ld a, (hl)
4CB1 8F         adc a, a
4CB2 0D         dec c
4CB3 C40000     call nz, $0000
4CB6 1A         ld a, (de)
4CB7 37         scf
4CB8 76         halt
4CB9 00         nop
4CBA D21500     jp nc, $0015
4CBD F41D24     call p, $241D
4CC0 1E1D       ld e, $1D
4CC2 1C         inc e
4CC3 7E         ld a, (hl)
4CC4 8F         adc a, a
4CC5 0E44       ld c, $44
4CC7 00         nop
4CC8 00         nop
4CC9 1A         ld a, (de)
4CCA 1C         inc e
4CCB 7E         ld a, (hl)
4CCC 00         nop
4CCD 00         nop
4CCE 00         nop
4CCF 00         nop
4CD0 00         nop
4CD1 76         halt
4CD2 00         nop
4CD3 DC1500     call c, $0015
4CD6 F41D24     call p, $241D
4CD9 1E1D       ld e, $1D
4CDB 1D         dec e
4CDC 7E         ld a, (hl)
4CDD 8F         adc a, a
4CDE 0E46       ld c, $46
4CE0 00         nop
4CE1 00         nop
4CE2 1A         ld a, (de)
4CE3 1C         inc e
4CE4 7E         ld a, (hl)
4CE5 00         nop
4CE6 00         nop
4CE7 00         nop
4CE8 00         nop
4CE9 00         nop
4CEA 76         halt
4CEB 00         nop
4CEC E610       and $10
4CEE 00         nop
4CEF F1         pop af
4CF0 3114D4     ld sp, $D414
4CF3 1D         dec e
4CF4 24         inc h
4CF5 201F       jr nz, $4D16
4CF7 1D         dec e
4CF8 7E         ld a, (hl)
4CF9 8F         adc a, a
4CFA 0F         rrca
4CFB FE00       cp $00
4CFD 00         nop
4CFE 76         halt
4CFF 00         nop
4D00 FA1E00     jp m, $001E
4D03 F5         push af
4D04 C1         pop bc
4D05 1C         inc e
4D06 7E         ld a, (hl)
4D07 00         nop
4D08 00         nop
4D09 00         nop
4D0A 00         nop
4D0B 00         nop
4D0C 1A         ld a, (de)
4D0D 1D         dec e
4D0E 1C         inc e
4D0F 7E         ld a, (hl)
4D10 84         add a, h
4D11 2000       jr nz, $4D13
4D13 00         nop
4D14 00         nop
4D15 19         add hl, de
4D16 0B         dec bc
4D17 3828       jr c, $4D41
4D19 34         inc (hl)
4D1A 37         scf
4D1B 2A0E00     ld hl, ($000E)
4D1E 1C         inc e
4D1F 0B         dec bc
4D20 76         halt
4D21 01040D     ld bc, $0D04
4D24 00         nop
4D25 F1         pop af
4D26 3C         inc a
4D27 14         inc d
4D28 3C         inc a
4D29 15         dec d
4D2A 1D         dec e
4D2B 7E         ld a, (hl)
4D2C 81         add a, c
4D2D 00         nop
4D2E 00         nop
4D2F 00         nop
4D30 00         nop
4D31 76         halt
4D32 010E17     ld bc, $170E
4D35 00         nop
4D36 FA3C14     jp m, $143C
4D39 1F         rra
4D3A 207E       jr nz, $4DBA
4D3C 86         add a, (hl)
4D3D 08         ex af, af'
4D3E 00         nop
4D3F 00         nop
4D40 00         nop
4D41 DEEC       sbc a, $EC
4D43 22201C     ld ($1C20), hl
4D46 7E         ld a, (hl)
4D47 8A         adc a, d
4D48 2000       jr nz, $4D4A
4D4A 00         nop
4D4B 00         nop
4D4C 76         halt
4D4D 01180F     ld bc, $0F18
4D50 00         nop
4D51 F41D23     call p, $231D
4D54 1D         dec e
4D55 21247E     ld hl, $7E24
4D58 8F         adc a, a
4D59 060C       ld b, $0C
4D5B 00         nop
4D5C 00         nop
4D5D 1A         ld a, (de)
4D5E 3C         inc a
4D5F 76         halt
4D60 01220F     ld bc, $0F22
4D63 00         nop
4D64 F41D23     call p, $231D
4D67 1D         dec e
4D68 24         inc h
4D69 1F         rra
4D6A 7E         ld a, (hl)
4D6B 8F         adc a, a
4D6C 063E       ld b, $3E
4D6E 00         nop
4D6F 00         nop
4D70 1A         ld a, (de)
4D71 3C         inc a
4D72 76         halt
4D73 012C0F     ld bc, $0F2C
4D76 00         nop
4D77 F41D23     call p, $231D
4D7A 1E1C       ld e, $1C
4D7C 24         inc h
4D7D 7E         ld a, (hl)
4D7E 8F         adc a, a
4D7F 0670       ld b, $70
4D81 00         nop
4D82 00         nop
4D83 1A         ld a, (de)
4D84 3C         inc a
4D85 76         halt
4D86 013610     ld bc, $1036
4D89 00         nop
4D8A F1         pop af
4D8B 3114D4     ld sp, $D414
4D8E 1D         dec e
4D8F 23         inc hl
4D90 25         dec h
4D91 22247E     ld ($7E24), hl
4D94 8F         adc a, a
4D95 0C         inc c
4D96 60         ld h, b
4D97 00         nop
4D98 00         nop
4D99 76         halt
4D9A 014010     ld bc, $1040
4D9D 00         nop
4D9E F1         pop af
4D9F 3114D4     ld sp, $D414
4DA2 1D         dec e
4DA3 24         inc h
4DA4 211E22     ld hl, $221E
4DA7 7E         ld a, (hl)
4DA8 8F         adc a, a
4DA9 10BC       djnz $4D67
4DAB 00         nop
4DAC 00         nop
4DAD 76         halt
4DAE 015410     ld bc, $1054
4DB1 00         nop
4DB2 F1         pop af
4DB3 3114D4     ld sp, $D414
4DB6 1D         dec e
4DB7 222224     ld ($2422), hl
4DBA 207E       jr nz, $4E3A
4DBC 8F         adc a, a
4DBD 02         ld (bc), a
4DBE 58         ld e, b
4DBF 00         nop
4DC0 00         nop
4DC1 76         halt
4DC2 015E10     ld bc, $105E
4DC5 00         nop
4DC6 F1         pop af
4DC7 3114D4     ld sp, $D414
4DCA 1D         dec e
4DCB 23         inc hl
4DCC 1C         inc e
4DCD 21217E     ld hl, $7E21
4DD0 8F         adc a, a
4DD1 05         dec b
4DD2 3E00       ld a, $00
4DD4 00         nop
4DD5 76         halt
4DD6 016810     ld bc, $1068
4DD9 00         nop
4DDA F1         pop af
4DDB 3114D4     ld sp, $D414
4DDE 1D         dec e
4DDF 23         inc hl
4DE0 1E1E       ld e, $1E
4DE2 207E       jr nz, $4E62
4DE4 8F         adc a, a
4DE5 0690       ld b, $90
4DE7 00         nop
4DE8 00         nop
4DE9 76         halt
4DEA 017211     ld bc, $1172
4DED 00         nop
4DEE FA4114     jp m, $1441
4DF1 0B         dec bc
4DF2 0B         dec bc
4DF3 DEEC       sbc a, $EC
4DF5 1F         rra
4DF6 201C       jr nz, $4E14
4DF8 7E         ld a, (hl)
4DF9 89         adc a, c
4DFA 2A0000     ld hl, ($0000)
4DFD 00         nop
4DFE 76         halt
4DFF 017C10     ld bc, $107C
4E02 00         nop
4E03 F1         pop af
4E04 3114D4     ld sp, $D414
4E07 1D         dec e
4E08 24         inc h
4E09 212021     ld hl, $2120
4E0C 7E         ld a, (hl)
4E0D 8F         adc a, a
4E0E 10E2       djnz $4DF2
4E10 00         nop
4E11 00         nop
4E12 76         halt
4E13 018610     ld bc, $1086
4E16 00         nop
4E17 F1         pop af
4E18 3114D4     ld sp, $D414
4E1B 1D         dec e
4E1C 24         inc h
4E1D 23         inc hl
4E1E 22257E     ld ($7E25), hl
4E21 8F         adc a, a
4E22 12         ld (de), a
4E23 A2         and d
4E24 00         nop
4E25 00         nop
4E26 76         halt
4E27 019010     ld bc, $1090
4E2A 00         nop
4E2B F1         pop af
4E2C 3114D4     ld sp, $D414
4E2F 1D         dec e
4E30 24         inc h
4E31 201C       jr nz, $4E4F
4E33 1D         dec e
4E34 7E         ld a, (hl)
4E35 8F         adc a, a
4E36 0F         rrca
4E37 C20000     jp nz, $0000
4E3A 76         halt
4E3B 019A10     ld bc, $109A
4E3E 00         nop
4E3F F1         pop af
4E40 3114D4     ld sp, $D414
4E43 1D         dec e
4E44 24         inc h
4E45 201D       jr nz, $4E64
4E47 217E8F     ld hl, $8F7E
4E4A 0F         rrca
4E4B DE00       sbc a, $00
4E4D 00         nop
4E4E 76         halt
4E4F 01A421     ld bc, $21A4
4E52 00         nop
4E53 FAD31D     jp m, $1DD3
4E56 23         inc hl
4E57 211C1F     ld hl, $1F1C
4E5A 7E         ld a, (hl)
4E5B 8F         adc a, a
4E5C 08         ex af, af'
4E5D BE         cp (hl)
4E5E 00         nop
4E5F 00         nop
4E60 14         inc d
4E61 1D         dec e
4E62 7E         ld a, (hl)
4E63 81         add a, c
4E64 00         nop
4E65 00         nop
4E66 00         nop
4E67 00         nop
4E68 DEEC       sbc a, $EC
4E6A 211E1C     ld hl, $1C1E
4E6D 7E         ld a, (hl)
4E6E 8A         adc a, d
4E6F 02         ld (bc), a
4E70 00         nop
4E71 00         nop
4E72 00         nop
4E73 76         halt
4E74 01AE14     ld bc, $14AE
4E77 00         nop
4E78 EB         ex de, hl
4E79 33         inc sp
4E7A 14         inc d
4E7B 1D         dec e
4E7C 7E         ld a, (hl)
4E7D 81         add a, c
4E7E 00         nop
4E7F 00         nop
4E80 00         nop
4E81 00         nop
4E82 DF         rst $18
4E83 1F         rra
4E84 1E7E       ld e, $7E
4E86 86         add a, (hl)
4E87 00         nop
4E88 00         nop
4E89 00         nop
4E8A 00         nop
4E8B 76         halt
4E8C 01B810     ld bc, $10B8
4E8F 00         nop
4E90 F1         pop af
4E91 3114D4     ld sp, $D414
4E94 1D         dec e
4E95 23         inc hl
4E96 24         inc h
4E97 221C7E     ld ($7E1C), hl
4E9A 8F         adc a, a
4E9B 0B         dec bc
4E9C 88         adc a, b
4E9D 00         nop
4E9E 00         nop
4E9F 76         halt
4EA0 01C207     ld bc, $07C2
4EA3 00         nop
4EA4 F1         pop af
4EA5 311431     ld sp, $3114
4EA8 15         dec d
4EA9 33         inc sp
4EAA 76         halt
4EAB 01CC03     ld bc, $03CC
4EAE 00         nop
4EAF F3         di
4EB0 33         inc sp
4EB1 76         halt
4EB2 01D610     ld bc, $10D6
4EB5 00         nop
4EB6 F1         pop af
4EB7 3114D4     ld sp, $D414
4EBA 1D         dec e
4EBB 23         inc hl
4EBC 201D       jr nz, $4EDB
4EBE 1F         rra
4EBF 7E         ld a, (hl)
4EC0 8F         adc a, a
4EC1 08         ex af, af'
4EC2 0A         ld a, (bc)
4EC3 00         nop
4EC4 00         nop
4EC5 76         halt
4EC6 01DB15     ld bc, $15DB
4EC9 00         nop
4ECA F41D24     call p, $241D
4ECD 1C         inc e
4ECE 1D         dec e
4ECF 227E8F     ld ($8F7E), hl
4ED2 0C         inc c
4ED3 C0         ret nz
4ED4 00         nop
4ED5 00         nop
4ED6 1A         ld a, (de)
4ED7 1C         inc e
4ED8 7E         ld a, (hl)
4ED9 00         nop
4EDA 00         nop
4EDB 00         nop
4EDC 00         nop
4EDD 00         nop
4EDE 76         halt
4EDF 01E015     ld bc, $15E0
4EE2 00         nop
4EE3 F41D23     call p, $231D
4EE6 2021       jr nz, $4F09
4EE8 1E7E       ld e, $7E
4EEA 8F         adc a, a
4EEB 08         ex af, af'
4EEC 58         ld e, b
4EED 00         nop
4EEE 00         nop
4EEF 1A         ld a, (de)
4EF0 1C         inc e
4EF1 7E         ld a, (hl)
4EF2 00         nop
4EF3 00         nop
4EF4 00         nop
4EF5 00         nop
4EF6 00         nop
4EF7 76         halt
4EF8 01E515     ld bc, $15E5
4EFB 00         nop
4EFC F41D24     call p, $241D
4EFF 1C         inc e
4F00 1D         dec e
4F01 24         inc h
4F02 7E         ld a, (hl)
4F03 8F         adc a, a
4F04 0C         inc c
4F05 C40000     call nz, $0000
4F08 1A         ld a, (de)
4F09 1C         inc e
4F0A 7E         ld a, (hl)
4F0B 00         nop
4F0C 00         nop
4F0D 00         nop
4F0E 00         nop
4F0F 00         nop
4F10 76         halt
4F11 01EA15     ld bc, $15EA
4F14 00         nop
4F15 F41D23     call p, $231D
4F18 1E1E       ld e, $1E
4F1A 1E7E       ld e, $7E
4F1C 8F         adc a, a
4F1D 068C       ld b, $8C
4F1F 00         nop
4F20 00         nop
4F21 1A         ld a, (de)
4F22 1C         inc e
4F23 7E         ld a, (hl)
4F24 00         nop
4F25 00         nop
4F26 00         nop
4F27 00         nop
4F28 00         nop
4F29 76         halt
4F2A 01F415     ld bc, $15F4
4F2D 00         nop
4F2E F41D23     call p, $231D
4F31 1E1E       ld e, $1E
4F33 1F         rra
4F34 7E         ld a, (hl)
4F35 8F         adc a, a
4F36 068E       ld b, $8E
4F38 00         nop
4F39 00         nop
4F3A 1A         ld a, (de)
4F3B 25         dec h
4F3C 7E         ld a, (hl)
4F3D 84         add a, h
4F3E 1000       djnz $4F40
4F40 00         nop
4F41 00         nop
4F42 76         halt
4F43 01F915     ld bc, $15F9
4F46 00         nop
4F47 F41D24     call p, $241D
4F4A 1C         inc e
4F4B 23         inc hl
4F4C 25         dec h
4F4D 7E         ld a, (hl)
4F4E 8F         adc a, a
4F4F 0D         dec c
4F50 3E00       ld a, $00
4F52 00         nop
4F53 1A         ld a, (de)
4F54 1C         inc e
4F55 7E         ld a, (hl)
4F56 00         nop
4F57 00         nop
4F58 00         nop
4F59 00         nop
4F5A 00         nop
4F5B 76         halt
4F5C 01FE0B     ld bc, $0BFE
4F5F 00         nop
4F60 EC1E22     call pe, $221E
4F63 1C         inc e
4F64 7E         ld a, (hl)
4F65 89         adc a, c
4F66 02         ld (bc), a
4F67 00         nop
4F68 00         nop
4F69 00         nop
4F6A 76         halt
4F6B 02         ld (bc), a
4F6C 08         ex af, af'
4F6D 14         inc d
4F6E 00         nop
4F6F EB         ex de, hl
4F70 33         inc sp
4F71 14         inc d
4F72 1D         dec e
4F73 7E         ld a, (hl)
4F74 81         add a, c
4F75 00         nop
4F76 00         nop
4F77 00         nop
4F78 00         nop
4F79 DF         rst $18
4F7A 211C7E     ld hl, $7E1C
4F7D 86         add a, (hl)
4F7E 48         ld c, b
4F7F 00         nop
4F80 00         nop
4F81 00         nop
4F82 76         halt
4F83 02         ld (bc), a
4F84 1C         inc e
4F85 1000       djnz $4F87
4F87 F1         pop af
4F88 3114D4     ld sp, $D414
4F8B 1D         dec e
4F8C 24         inc h
4F8D 221D20     ld ($201D), hl
4F90 7E         ld a, (hl)
4F91 8F         adc a, a
4F92 116C00     ld de, $006C
4F95 00         nop
4F96 76         halt
4F97 02         ld (bc), a
4F98 210D00     ld hl, $000D
4F9B F1         pop af
4F9C 311431     ld sp, $3114
4F9F 15         dec d
4FA0 1C         inc e
4FA1 7E         ld a, (hl)
4FA2 00         nop
4FA3 00         nop
4FA4 00         nop
4FA5 00         nop
4FA6 00         nop
4FA7 76         halt
4FA8 02         ld (bc), a
4FA9 23         inc hl
4FAA 0D         dec c
4FAB 00         nop
4FAC F1         pop af
4FAD 311431     ld sp, $3114
4FB0 15         dec d
4FB1 1C         inc e
4FB2 7E         ld a, (hl)
4FB3 00         nop
4FB4 00         nop
4FB5 00         nop
4FB6 00         nop
4FB7 00         nop
4FB8 76         halt
4FB9 02         ld (bc), a
4FBA 2603       ld h, $03
4FBC 00         nop
4FBD F3         di
4FBE 33         inc sp
4FBF 76         halt
4FC0 02         ld (bc), a
4FC1 3027       jr nc, $4FEA
4FC3 00         nop
4FC4 F1         pop af
4FC5 3814       jr c, $4FDB
4FC7 D31D       out ($1D), a
4FC9 24         inc h
4FCA 1E1D       ld e, $1D
4FCC 1C         inc e
4FCD 7E         ld a, (hl)
4FCE 8F         adc a, a
4FCF 0E44       ld c, $44
4FD1 00         nop
4FD2 00         nop
4FD3 15         dec d
4FD4 1E21       ld e, $21
4FD6 227E89     ld ($897E), hl
4FD9 00         nop
4FDA 00         nop
4FDB 00         nop
4FDC 00         nop
4FDD 17         rla
4FDE D31D       out ($1D), a
4FE0 24         inc h
4FE1 1E1D       ld e, $1D
4FE3 1D         dec e
4FE4 7E         ld a, (hl)
4FE5 8F         adc a, a
4FE6 0E46       ld c, $46
4FE8 00         nop
4FE9 00         nop
4FEA 76         halt
4FEB 02         ld (bc), a
4FEC 3A2000     ld a, ($0020)
4FEF F1         pop af
4FF0 3814       jr c, $5006
4FF2 3815       jr c, $5009
4FF4 2B         dec hl
4FF5 17         rla
4FF6 1010       djnz $5008
4FF8 D31D       out ($1D), a
4FFA 23         inc hl
4FFB 2021       jr nz, $501E
4FFD 217E8F     ld hl, $8F7E
5000 08         ex af, af'
5001 5E         ld e, (hl)
5002 00         nop
5003 00         nop
5004 11161F     ld de, $1F16
5007 7E         ld a, (hl)
5008 82         add a, d
5009 40         ld b, b
500A 00         nop
500B 00         nop
500C 00         nop
500D 117602     ld de, $0276
5010 3F         ccf
5011 15         dec d
5012 00         nop
5013 F5         push af
5014 C1         pop bc
5015 1C         inc e
5016 7E         ld a, (hl)
5017 00         nop
5018 00         nop
5019 00         nop
501A 00         nop
501B 00         nop
501C 1A         ld a, (de)
501D 1D         dec e
501E 23         inc hl
501F 7E         ld a, (hl)
5020 85         add a, l
5021 08         ex af, af'
5022 00         nop
5023 00         nop
5024 00         nop
5025 19         add hl, de
5026 3876       jr c, $509E
5028 02         ld (bc), a
5029 44         ld b, h
502A 1C         inc e
502B 00         nop
502C F41D24     call p, $241D
502F 1E1D       ld e, $1D
5031 1D         dec e
5032 7E         ld a, (hl)
5033 8F         adc a, a
5034 0E46       ld c, $46
5036 00         nop
5037 00         nop
5038 1A         ld a, (de)
5039 CF         rst $08
503A 1038       djnz $5074
503C 181E       jr $505C
503E 21227E     ld hl, $7E22
5041 89         adc a, c
5042 00         nop
5043 00         nop
5044 00         nop
5045 00         nop
5046 117602     ld de, $0276
5049 4E         ld c, (hl)
504A 2800       jr z, $504C
504C F41D24     call p, $241D
504F 1E1D       ld e, $1D
5051 1C         inc e
5052 7E         ld a, (hl)
5053 8F         adc a, a
5054 0E44       ld c, $44
5056 00         nop
5057 00         nop
5058 1A         ld a, (de)
5059 1038       djnz $5093
505B 161E       ld d, $1E
505D 21227E     ld hl, $7E22
5060 89         adc a, c
5061 00         nop
5062 00         nop
5063 00         nop
5064 00         nop
5065 17         rla
5066 D31D       out ($1D), a
5068 24         inc h
5069 1E1D       ld e, $1D
506B 1D         dec e
506C 7E         ld a, (hl)
506D 8F         adc a, a
506E 0E46       ld c, $46
5070 00         nop
5071 00         nop
5072 117602     ld de, $0276
5075 50         ld d, b
5076 1A         ld a, (de)
5077 00         nop
5078 FA3712     jp m, $1237
507B 1E1C       ld e, $1C
507D 7E         ld a, (hl)
507E 85         add a, l
507F 2000       jr nz, $5081
5081 00         nop
5082 00         nop
5083 DEF1       sbc a, $F1
5085 37         scf
5086 14         inc d
5087 37         scf
5088 161E       ld d, $1E
508A 1C         inc e
508B 7E         ld a, (hl)
508C 85         add a, l
508D 2000       jr nz, $508F
508F 00         nop
5090 00         nop
5091 76         halt
5092 02         ld (bc), a
5093 53         ld d, e
5094 0F         rrca
5095 00         nop
5096 F41D24     call p, $241D
5099 1D         dec e
509A 2022       jr nz, $50BE
509C 7E         ld a, (hl)
509D 8F         adc a, a
509E 0D         dec c
509F C40000     call nz, $0000
50A2 1A         ld a, (de)
50A3 37         scf
50A4 76         halt
50A5 02         ld (bc), a
50A6 58         ld e, b
50A7 1000       djnz $50A9
50A9 F1         pop af
50AA 3114D4     ld sp, $D414
50AD 1D         dec e
50AE 24         inc h
50AF 24         inc h
50B0 22237E     ld ($7E23), hl
50B3 8F         adc a, a
50B4 13         inc de
50B5 66         ld h, (hl)
50B6 00         nop
50B7 00         nop
50B8 76         halt
50B9 02         ld (bc), a
50BA 6C         ld l, h
50BB 1000       djnz $50BD
50BD F1         pop af
50BE 3114D4     ld sp, $D414
50C1 1D         dec e
50C2 24         inc h
50C3 201F       jr nz, $50E4
50C5 1D         dec e
50C6 7E         ld a, (hl)
50C7 8F         adc a, a
50C8 0F         rrca
50C9 FE00       cp $00
50CB 00         nop
50CC 76         halt
50CD 02         ld (bc), a
50CE 76         halt
50CF 0B         dec bc
50D0 00         nop
50D1 EC1F1D     call pe, $1D1F
50D4 1C         inc e
50D5 7E         ld a, (hl)
50D6 89         adc a, c
50D7 1B         dec de
50D8 00         nop
50D9 00         nop
50DA 00         nop
50DB 76         halt
50DC 02         ld (bc), a
50DD 80         add a, b
50DE 1000       djnz $50E0
50E0 F1         pop af
50E1 3114D4     ld sp, $D414
50E4 1D         dec e
50E5 24         inc h
50E6 24         inc h
50E7 22237E     ld ($7E23), hl
50EA 8F         adc a, a
50EB 13         inc de
50EC 66         ld h, (hl)
50ED 00         nop
50EE 00         nop
50EF 76         halt
50F0 02         ld (bc), a
50F1 8A         adc a, d
50F2 1B         dec de
50F3 00         nop
50F4 F5         push af
50F5 C1         pop bc
50F6 1C         inc e
50F7 7E         ld a, (hl)
50F8 00         nop
50F9 00         nop
50FA 00         nop
50FB 00         nop
50FC 00         nop
50FD 1A         ld a, (de)
50FE 207E       jr nz, $517E
5100 83         add a, e
5101 00         nop
5102 00         nop
5103 00         nop
5104 00         nop
5105 19         add hl, de
5106 0B         dec bc
5107 2B         dec hl
5108 2E33       ld l, $33
510A 2631       ld h, $31
510C 0B         dec bc
510D 19         add hl, de
510E 76         halt
510F 02         ld (bc), a
5110 94         sub h
5111 1D         dec e
5112 00         nop
5113 F5         push af
5114 C1         pop bc
5115 1D         dec e
5116 1C         inc e
5117 7E         ld a, (hl)
5118 84         add a, h
5119 2000       jr nz, $511B
511B 00         nop
511C 00         nop
511D 1A         ld a, (de)
511E 1D         dec e
511F 1F         rra
5120 7E         ld a, (hl)
5121 84         add a, h
5122 50         ld d, b
5123 00         nop
5124 00         nop
5125 00         nop
5126 19         add hl, de
5127 0B         dec bc
5128 07         rlca
5129 03         inc bc
512A 03         inc bc
512B 03         inc bc
512C 03         inc bc
512D 84         add a, h
512E 0B         dec bc
512F 76         halt
5130 02         ld (bc), a
5131 9E         sbc a, (hl)
5132 1D         dec e
5133 00         nop
5134 F5         push af
5135 C1         pop bc
5136 1D         dec e
5137 1D         dec e
5138 7E         ld a, (hl)
5139 84         add a, h
513A 3000       jr nc, $513C
513C 00         nop
513D 00         nop
513E 1A         ld a, (de)
513F 1D         dec e
5140 1F         rra
5141 7E         ld a, (hl)
5142 84         add a, h
5143 50         ld d, b
5144 00         nop
5145 00         nop
5146 00         nop
5147 19         add hl, de
5148 0B         dec bc
5149 05         dec b
514A 2C         inc l
514B 2632       ld h, $32
514D 2A850B     ld hl, ($0B85)
5150 76         halt
5151 02         ld (bc), a
5152 A8         xor b
5153 1D         dec e
5154 00         nop
5155 F5         push af
5156 C1         pop bc
5157 1D         dec e
5158 1E7E       ld e, $7E
515A 84         add a, h
515B 40         ld b, b
515C 00         nop
515D 00         nop
515E 00         nop
515F 1A         ld a, (de)
5160 1D         dec e
5161 1F         rra
5162 7E         ld a, (hl)
5163 84         add a, h
5164 50         ld d, b
5165 00         nop
5166 00         nop
5167 00         nop
5168 19         add hl, de
5169 0B         dec bc
516A 05         dec b
516B 34         inc (hl)
516C 3B         dec sp
516D 2A3785     ld hl, ($8537)
5170 0B         dec bc
5171 76         halt
5172 02         ld (bc), a
5173 B2         or d
5174 1D         dec e
5175 00         nop
5176 F5         push af
5177 C1         pop bc
5178 1D         dec e
5179 1F         rra
517A 7E         ld a, (hl)
517B 84         add a, h
517C 50         ld d, b
517D 00         nop
517E 00         nop
517F 00         nop
5180 1A         ld a, (de)
5181 1D         dec e
5182 1F         rra
5183 7E         ld a, (hl)
5184 84         add a, h
5185 50         ld d, b
5186 00         nop
5187 00         nop
5188 00         nop
5189 19         add hl, de
518A 0B         dec bc
518B 82         add a, d
518C 83         add a, e
518D 83         add a, e
518E 83         add a, e
518F 83         add a, e
5190 81         add a, c
5191 0B         dec bc
5192 76         halt
5193 02         ld (bc), a
5194 BC         cp h
5195 2A00F5     ld hl, ($F500)
5198 C1         pop bc
5199 1D         dec e
519A 227E85     ld ($857E), hl
519D 00         nop
519E 00         nop
519F 00         nop
51A0 00         nop
51A1 1A         ld a, (de)
51A2 227E83     ld ($837E), hl
51A5 40         ld b, b
51A6 00         nop
51A7 00         nop
51A8 00         nop
51A9 19         add hl, de
51AA 0B         dec bc
51AB 35         dec (hl)
51AC 31263E     ld sp, $3E26
51AF 00         nop
51B0 262C       ld h, $2C
51B2 262E       ld h, $2E
51B4 33         inc sp
51B5 0F         rrca
51B6 00         nop
51B7 103E       djnz $51F7
51B9 00         nop
51BA 34         inc (hl)
51BB 37         scf
51BC 00         nop
51BD 33         inc sp
51BE 110B76     ld de, $760B
51C1 02         ld (bc), a
51C2 D0         ret nc
51C3 09         add hl, bc
51C4 00         nop
51C5 FA4114     jp m, $1441
51C8 0B         dec bc
51C9 33         inc sp
51CA 0B         dec bc
51CB DEE3       sbc a, $E3
51CD 76         halt
51CE 02         ld (bc), a
51CF DA1200     jp c, $0012
51D2 FA4114     jp m, $1441
51D5 0B         dec bc
51D6 3E0B       ld a, $0B
51D8 DEEC       sbc a, $EC
51DA 1D         dec e
51DB 25         dec h
51DC 1C         inc e
51DD 7E         ld a, (hl)
51DE 88         adc a, b
51DF 3E00       ld a, $00
51E1 00         nop
51E2 00         nop
51E3 76         halt
51E4 02         ld (bc), a
51E5 E40B00     call po, $000B
51E8 EC231E     call pe, $1E23
51EB 1C         inc e
51EC 7E         ld a, (hl)
51ED 8A         adc a, d
51EE 34         inc (hl)
51EF 00         nop
51F0 00         nop
51F1 00         nop
51F2 76         halt
51F3 03         inc bc
51F4 E8         ret pe
51F5 02         ld (bc), a
51F6 00         nop
51F7 FB         ei
51F8 76         halt
51F9 03         inc bc
51FA F22000     jp p, $0020
51FD F5         push af
51FE C1         pop bc
51FF 1C         inc e
5200 7E         ld a, (hl)
5201 00         nop
5202 00         nop
5203 00         nop
5204 00         nop
5205 00         nop
5206 1A         ld a, (de)
5207 1D         dec e
5208 1C         inc e
5209 7E         ld a, (hl)
520A 84         add a, h
520B 2000       jr nz, $520D
520D 00         nop
520E 00         nop
520F 19         add hl, de
5210 0B         dec bc
5211 3E34       ld a, $34
5213 3A3700     ld a, ($0037)
5216 2C         inc l
5217 34         inc (hl)
5218 2631       ld h, $31
521A 0E0B       ld c, $0B
521C 76         halt
521D 03         inc bc
521E FC6D00     call m, $006D
5221 F5         push af
5222 C1         pop bc
5223 1E7E       ld e, $7E
5225 82         add a, d
5226 00         nop
5227 00         nop
5228 00         nop
5229 00         nop
522A 1A         ld a, (de)
522B 1C         inc e
522C 7E         ld a, (hl)
522D 00         nop
522E 00         nop
522F 00         nop
5230 00         nop
5231 00         nop
5232 19         add hl, de
5233 0B         dec bc
5234 39         add hl, sp
5235 34         inc (hl)
5236 00         nop
5237 3828       jr c, $5261
5239 34         inc (hl)
523A 37         scf
523B 2A0026     ld hl, ($2600)
523E 3800       jr c, $5240
5240 322633     ld ($3326), a
5243 3E00       ld a, $00
5245 35         dec (hl)
5246 34         inc (hl)
5247 2E33       ld l, $33
5249 39         add hl, sp
524A 3800       jr c, $524C
524C 2638       ld h, $38
524E 00         nop
524F 3E34       ld a, $34
5251 3A0000     ld a, ($0000)
5254 00         nop
5255 2826       jr z, $527D
5257 33         inc sp
5258 00         nop
5259 27         daa
525A 3E00       ld a, $00
525C 29         add hl, hl
525D 2A3839     ld hl, ($3938)
5260 37         scf
5261 34         inc (hl)
5262 3E2E       ld a, $2E
5264 33         inc sp
5265 2C         inc l
5266 00         nop
5267 39         add hl, sp
5268 2D         dec l
5269 2A0026     ld hl, ($2600)
526C 312E2A     ld sp, $2A2E
526F 33         inc sp
5270 00         nop
5271 2633       ld h, $33
5273 29         add hl, hl
5274 00         nop
5275 27         daa
5276 313428     ld sp, $2834
5279 3038       jr nc, $52B3
527B 00         nop
527C 34         inc (hl)
527D 2B         dec hl
527E 00         nop
527F 2D         dec l
5280 2E38       ld l, $38
5282 00         nop
5283 2B         dec hl
5284 34         inc (hl)
5285 37         scf
5286 39         add hl, sp
5287 37         scf
5288 2A3838     ld hl, ($3838)
528B 1B         dec de
528C 0B         dec bc
528D 76         halt
528E 04         inc b
528F 0689       ld b, $89
5291 00         nop
5292 F5         push af
5293 C1         pop bc
5294 227E83     ld ($837E), hl
5297 40         ld b, b
5298 00         nop
5299 00         nop
529A 00         nop
529B 1A         ld a, (de)
529C 1C         inc e
529D 7E         ld a, (hl)
529E 00         nop
529F 00         nop
52A0 00         nop
52A1 00         nop
52A2 00         nop
52A3 19         add hl, de
52A4 0B         dec bc
52A5 3E34       ld a, $34
52A7 3A002D     ld a, ($2D00)
52AA 263B       ld h, $3B
52AC 2A002B     ld hl, ($2B00)
52AF 2E3B       ld l, $3B
52B1 2A0038     ld hl, ($3800)
52B4 2D         dec l
52B5 2E35       ld l, $35
52B7 3800       jr c, $52B9
52B9 39         add hl, sp
52BA 34         inc (hl)
52BB 00         nop
52BC 29         add hl, hl
52BD 2A3839     ld hl, ($3938)
52C0 37         scf
52C1 34         inc (hl)
52C2 3E00       ld a, $00
52C4 00         nop
52C5 00         nop
52C6 39         add hl, sp
52C7 2D         dec l
52C8 2A0026     ld hl, ($2600)
52CB 312E2A     ld sp, $2A2E
52CE 33         inc sp
52CF 00         nop
52D0 2638       ld h, $38
52D2 00         nop
52D3 322633     ld ($3326), a
52D6 3E00       ld a, $00
52D8 39         add hl, sp
52D9 2E32       ld l, $32
52DB 2A3800     ld hl, ($0038)
52DE 2638       ld h, $38
52E0 00         nop
52E1 3E34       ld a, $34
52E3 3A0000     ld a, ($0000)
52E6 2826       jr z, $530E
52E8 33         inc sp
52E9 1B         dec de
52EA 00         nop
52EB 00         nop
52EC 3C         inc a
52ED 2637       ld h, $37
52EF 33         inc sp
52F0 2E33       ld l, $33
52F2 2C         inc l
52F3 00         nop
52F4 1600       ld d, $00
52F6 39         add hl, sp
52F7 2D         dec l
52F8 2A0026     ld hl, ($2600)
52FB 312E2A     ld sp, $2A2E
52FE 33         inc sp
52FF 00         nop
5300 2C         inc l
5301 3A3338     ld a, ($3833)
5304 00         nop
5305 00         nop
5306 2637       ld h, $37
5308 2A002E     ld hl, ($2E00)
530B 33         inc sp
530C 29         add hl, hl
530D 2A3839     ld hl, ($3938)
5310 37         scf
5311 3A2839     ld a, ($3928)
5314 2E27       ld l, $27
5316 312A1B     ld sp, $1B2A
5319 0B         dec bc
531A 76         halt
531B 04         inc b
531C 1093       djnz $52B1
531E 00         nop
531F F5         push af
5320 C1         pop bc
5321 1D         dec e
5322 1D         dec e
5323 7E         ld a, (hl)
5324 84         add a, h
5325 3000       jr nc, $5327
5327 00         nop
5328 00         nop
5329 1A         ld a, (de)
532A 1C         inc e
532B 7E         ld a, (hl)
532C 00         nop
532D 00         nop
532E 00         nop
532F 00         nop
5330 00         nop
5331 19         add hl, de
5332 0B         dec bc
5333 35         dec (hl)
5334 34         inc (hl)
5335 2E33       ld l, $33
5337 39         add hl, sp
5338 3800       jr c, $533A
533A 2637       ld h, $37
533C 2A0026     ld hl, ($2600)
533F 3C         inc a
5340 2637       ld h, $37
5342 29         add hl, hl
5343 2A2900     ld hl, ($0029)
5346 27         daa
5347 2638       ld h, $38
5349 2A2900     ld hl, ($0029)
534C 34         inc (hl)
534D 33         inc sp
534E 00         nop
534F 00         nop
5350 00         nop
5351 00         nop
5352 00         nop
5353 00         nop
5354 3830       jr c, $5386
5356 2E31       ld l, $31
5358 310031     ld sp, $3100
535B 2A3B2A     ld hl, ($2A3B)
535E 310026     ld sp, $2600
5361 33         inc sp
5362 29         add hl, hl
5363 00         nop
5364 39         add hl, sp
5365 2D         dec l
5366 2A0029     ld hl, ($2900)
5369 2E38       ld l, $38
536B 39         add hl, sp
536C 2633       ld h, $33
536E 282A       jr z, $539A
5370 00         nop
5371 00         nop
5372 00         nop
5373 00         nop
5374 3E34       ld a, $34
5376 3A3700     ld a, ($0037)
5379 382D       jr c, $53A8
537B 2E35       ld l, $35
537D 00         nop
537E 2E38       ld l, $38
5380 00         nop
5381 2B         dec hl
5382 37         scf
5383 34         inc (hl)
5384 320039     ld ($3900), a
5387 2D         dec l
5388 2A0031     ld hl, ($3100)
538B 2A2B39     ld hl, ($392B)
538E 00         nop
538F 2A292C     ld hl, ($2C29)
5392 2A0034     ld hl, ($3400)
5395 2B         dec hl
5396 00         nop
5397 39         add hl, sp
5398 2D         dec l
5399 2A0038     ld hl, ($3800)
539C 2837       jr z, $53D5
539E 2A2A33     ld hl, ($332A)
53A1 00         nop
53A2 3C         inc a
53A3 2D         dec l
53A4 2A3300     ld hl, ($0033)
53A7 2E39       ld l, $39
53A9 00         nop
53AA 2B         dec hl
53AB 2E37       ld l, $37
53AD 2A381B     ld hl, ($1B38)
53B0 0B         dec bc
53B1 76         halt
53B2 04         inc b
53B3 1A         ld a, (de)
53B4 80         add a, b
53B5 00         nop
53B6 F5         push af
53B7 C1         pop bc
53B8 1D         dec e
53B9 227E85     ld ($857E), hl
53BC 00         nop
53BD 00         nop
53BE 00         nop
53BF 00         nop
53C0 1A         ld a, (de)
53C1 1C         inc e
53C2 7E         ld a, (hl)
53C3 00         nop
53C4 00         nop
53C5 00         nop
53C6 00         nop
53C7 00         nop
53C8 19         add hl, de
53C9 0B         dec bc
53CA 29         add hl, hl
53CB 2A3839     ld hl, ($3938)
53CE 37         scf
53CF 34         inc (hl)
53D0 3E2E       ld a, $2E
53D2 33         inc sp
53D3 2C         inc l
53D4 00         nop
53D5 39         add hl, sp
53D6 2D         dec l
53D7 2A0026     ld hl, ($2600)
53DA 312E2A     ld sp, $2A2E
53DD 33         inc sp
53DE 00         nop
53DF 2E38       ld l, $38
53E1 00         nop
53E2 3C         inc a
53E3 34         inc (hl)
53E4 37         scf
53E5 39         add hl, sp
53E6 2D         dec l
53E7 00         nop
53E8 00         nop
53E9 00         nop
53EA 00         nop
53EB 2B         dec hl
53EC 2E2B       ld l, $2B
53EE 39         add hl, sp
53EF 3E00       ld a, $00
53F1 39         add hl, sp
53F2 2E32       ld l, $32
53F4 2A3800     ld hl, ($0038)
53F7 39         add hl, sp
53F8 2D         dec l
53F9 2A0035     ld hl, ($3500)
53FC 34         inc (hl)
53FD 2E33       ld l, $33
53FF 39         add hl, sp
5400 3800       jr c, $5402
5402 263C       ld h, $3C
5404 2637       ld h, $37
5406 29         add hl, hl
5407 2A2900     ld hl, ($0029)
540A 00         nop
540B 2B         dec hl
540C 34         inc (hl)
540D 37         scf
540E 00         nop
540F 29         add hl, hl
5410 2A3839     ld hl, ($3938)
5413 37         scf
5414 34         inc (hl)
5415 3E2E       ld a, $2E
5417 33         inc sp
5418 2C         inc l
5419 00         nop
541A 2600       ld h, $00
541C 27         daa
541D 313428     ld sp, $2834
5420 3000       jr nc, $5422
5422 34         inc (hl)
5423 2B         dec hl
5424 00         nop
5425 2D         dec l
5426 2E38       ld l, $38
5428 00         nop
5429 00         nop
542A 00         nop
542B 2B         dec hl
542C 34         inc (hl)
542D 37         scf
542E 39         add hl, sp
542F 37         scf
5430 2A3838     ld hl, ($3838)
5433 1B         dec de
5434 0B         dec bc
5435 76         halt
5436 04         inc b
5437 24         inc h
5438 33         inc sp
5439 00         nop
543A F5         push af
543B C1         pop bc
543C 1E1D       ld e, $1D
543E 7E         ld a, (hl)
543F 85         add a, l
5440 2800       jr z, $5442
5442 00         nop
5443 00         nop
5444 1A         ld a, (de)
5445 1D         dec e
5446 7E         ld a, (hl)
5447 81         add a, c
5448 00         nop
5449 00         nop
544A 00         nop
544B 00         nop
544C 19         add hl, de
544D 0B         dec bc
544E 2D         dec l
544F 2E39       ld l, $39
5451 00         nop
5452 2633       ld h, $33
5454 3E00       ld a, $00
5456 312A39     ld sp, $392A
5459 39         add hl, sp
545A 2A3700     ld hl, ($0037)
545D 39         add hl, sp
545E 34         inc (hl)
545F 00         nop
5460 2834       jr z, $5496
5462 33         inc sp
5463 39         add hl, sp
5464 2E33       ld l, $33
5466 3A2A1B     ld a, ($1B2A)
5469 1B         dec de
546A 1B         dec de
546B 0B         dec bc
546C 76         halt
546D 04         inc b
546E 2E12       ld l, $12
5470 00         nop
5471 FA4114     jp m, $1441
5474 0B         dec bc
5475 0B         dec bc
5476 DEEC       sbc a, $EC
5478 1D         dec e
5479 1C         inc e
547A 23         inc hl
547B 1C         inc e
547C 7E         ld a, (hl)
547D 8B         adc a, e
547E 05         dec b
547F C0         ret nz
5480 00         nop
5481 00         nop
5482 76         halt
5483 04         inc b
5484 3802       jr c, $5488
5486 00         nop
5487 FB         ei
5488 76         halt
5489 04         inc b
548A 42         ld b, d
548B 2E00       ld l, $00
548D F5         push af
548E C1         pop bc
548F 1C         inc e
5490 7E         ld a, (hl)
5491 00         nop
5492 00         nop
5493 00         nop
5494 00         nop
5495 00         nop
5496 1A         ld a, (de)
5497 1F         rra
5498 7E         ld a, (hl)
5499 82         add a, d
549A 40         ld b, b
549B 00         nop
549C 00         nop
549D 00         nop
549E 19         add hl, de
549F 0B         dec bc
54A0 3E34       ld a, $34
54A2 3A3700     ld a, ($0037)
54A5 382D       jr c, $54D4
54A7 2E35       ld l, $35
54A9 3800       jr c, $54AB
54AB 2834       jr z, $54E1
54AD 33         inc sp
54AE 39         add hl, sp
54AF 37         scf
54B0 34         inc (hl)
54B1 310035     ld sp, $3500
54B4 2633       ld h, $33
54B6 2A310E     ld hl, ($0E31)
54B9 0B         dec bc
54BA 76         halt
54BB 04         inc b
54BC 4C         ld c, h
54BD B2         or d
54BE 00         nop
54BF F5         push af
54C0 C1         pop bc
54C1 1F         rra
54C2 7E         ld a, (hl)
54C3 82         add a, d
54C4 40         ld b, b
54C5 00         nop
54C6 00         nop
54C7 00         nop
54C8 1A         ld a, (de)
54C9 1E7E       ld e, $7E
54CB 82         add a, d
54CC 00         nop
54CD 00         nop
54CE 00         nop
54CF 00         nop
54D0 19         add hl, de
54D1 0B         dec bc
54D2 07         rlca
54D3 03         inc bc
54D4 03         inc bc
54D5 03         inc bc
54D6 03         inc bc
54D7 03         inc bc
54D8 03         inc bc
54D9 03         inc bc
54DA 03         inc bc
54DB 03         inc bc
54DC 03         inc bc
54DD 84         add a, h
54DE 00         nop
54DF 00         nop
54E0 00         nop
54E1 00         nop
54E2 07         rlca
54E3 03         inc bc
54E4 03         inc bc
54E5 03         inc bc
54E6 03         inc bc
54E7 03         inc bc
54E8 03         inc bc
54E9 03         inc bc
54EA 03         inc bc
54EB 03         inc bc
54EC 03         inc bc
54ED 84         add a, h
54EE 00         nop
54EF 00         nop
54F0 00         nop
54F1 00         nop
54F2 05         dec b
54F3 1D         dec e
54F4 00         nop
54F5 1E00       ld e, $00
54F7 1F         rra
54F8 00         nop
54F9 2000       jr nz, $54FB
54FB 210085     ld hl, $8500
54FE 00         nop
54FF 00         nop
5500 00         nop
5501 00         nop
5502 05         dec b
5503 220023     ld ($2300), hl
5506 00         nop
5507 24         inc h
5508 00         nop
5509 25         dec h
550A 00         nop
550B 1C         inc e
550C 00         nop
550D 85         add a, l
550E 00         nop
550F 00         nop
5510 00         nop
5511 00         nop
5512 05         dec b
5513 00         nop
5514 3600       ld (hl), $00
5516 3C         inc a
5517 00         nop
5518 2A0037     ld hl, ($3700)
551B 00         nop
551C 39         add hl, sp
551D 85         add a, l
551E 00         nop
551F 00         nop
5520 00         nop
5521 00         nop
5522 05         dec b
5523 00         nop
5524 3E00       ld a, $00
5526 3A002E     ld a, ($2E00)
5529 00         nop
552A 34         inc (hl)
552B 00         nop
552C 35         dec (hl)
552D 85         add a, l
552E 00         nop
552F 00         nop
5530 00         nop
5531 00         nop
5532 82         add a, d
5533 83         add a, e
5534 83         add a, e
5535 83         add a, e
5536 83         add a, e
5537 83         add a, e
5538 83         add a, e
5539 83         add a, e
553A 83         add a, e
553B 83         add a, e
553C 83         add a, e
553D 81         add a, c
553E 00         nop
553F 00         nop
5540 00         nop
5541 00         nop
5542 82         add a, d
5543 83         add a, e
5544 83         add a, e
5545 83         add a, e
5546 83         add a, e
5547 83         add a, e
5548 83         add a, e
5549 83         add a, e
554A 83         add a, e
554B 83         add a, e
554C 83         add a, e
554D 81         add a, c
554E 00         nop
554F 00         nop
5550 00         nop
5551 00         nop
5552 32343B     ld ($3B34), a
5555 2A0038     ld hl, ($3800)
5558 2D         dec l
5559 2E35       ld l, $35
555B 00         nop
555C 3A3500     ld a, ($0035)
555F 00         nop
5560 00         nop
5561 00         nop
5562 2B         dec hl
5563 2E37       ld l, $37
5565 2A0039     ld hl, ($3900)
5568 34         inc (hl)
5569 37         scf
556A 35         dec (hl)
556B 2A2934     ld hl, ($3429)
556E 0B         dec bc
556F 00         nop
5570 76         halt
5571 04         inc b
5572 56         ld d, (hl)
5573 F400F5     call p, $F500
5576 C1         pop bc
5577 1D         dec e
5578 1D         dec e
5579 7E         ld a, (hl)
557A 84         add a, h
557B 3000       jr nc, $557D
557D 00         nop
557E 00         nop
557F 1A         ld a, (de)
5580 1C         inc e
5581 7E         ld a, (hl)
5582 00         nop
5583 00         nop
5584 00         nop
5585 00         nop
5586 00         nop
5587 19         add hl, de
5588 0B         dec bc
5589 07         rlca
558A 03         inc bc
558B 03         inc bc
558C 03         inc bc
558D 03         inc bc
558E 03         inc bc
558F 03         inc bc
5590 03         inc bc
5591 03         inc bc
5592 03         inc bc
5593 84         add a, h
5594 00         nop
5595 00         nop
5596 07         rlca
5597 03         inc bc
5598 03         inc bc
5599 03         inc bc
559A 03         inc bc
559B 03         inc bc
559C 03         inc bc
559D 84         add a, h
559E 00         nop
559F 07         rlca
55A0 03         inc bc
55A1 03         inc bc
55A2 03         inc bc
55A3 03         inc bc
55A4 03         inc bc
55A5 03         inc bc
55A6 03         inc bc
55A7 03         inc bc
55A8 84         add a, h
55A9 05         dec b
55AA 2600       ld h, $00
55AC 3800       jr c, $55AE
55AE 29         add hl, hl
55AF 00         nop
55B0 2B         dec hl
55B1 00         nop
55B2 2C         inc l
55B3 85         add a, l
55B4 00         nop
55B5 00         nop
55B6 05         dec b
55B7 00         nop
55B8 2D         dec l
55B9 00         nop
55BA 2F         cpl
55BB 00         nop
55BC 3085       jr nc, $5543
55BE 00         nop
55BF 05         dec b
55C0 00         nop
55C1 31002A     ld sp, $2A00
55C4 33         inc sp
55C5 39         add hl, sp
55C6 2A3785     ld hl, ($8537)
55C9 05         dec b
55CA 00         nop
55CB 3F         ccf
55CC 00         nop
55CD 3D         dec a
55CE 00         nop
55CF 2800       jr z, $55D1
55D1 3B         dec sp
55D2 00         nop
55D3 85         add a, l
55D4 00         nop
55D5 00         nop
55D6 05         dec b
55D7 27         daa
55D8 00         nop
55D9 33         inc sp
55DA 00         nop
55DB 320085     ld ($8500), a
55DE 00         nop
55DF 05         dec b
55E0 1B         dec de
55E1 00         nop
55E2 3835       jr c, $5619
55E4 2628       ld h, $28
55E6 2A0085     ld hl, ($8500)
55E9 82         add a, d
55EA 83         add a, e
55EB 83         add a, e
55EC 83         add a, e
55ED 83         add a, e
55EE 83         add a, e
55EF 83         add a, e
55F0 83         add a, e
55F1 83         add a, e
55F2 83         add a, e
55F3 81         add a, c
55F4 00         nop
55F5 00         nop
55F6 82         add a, d
55F7 83         add a, e
55F8 83         add a, e
55F9 83         add a, e
55FA 83         add a, e
55FB 83         add a, e
55FC 83         add a, e
55FD 81         add a, c
55FE 00         nop
55FF 82         add a, d
5600 83         add a, e
5601 83         add a, e
5602 83         add a, e
5603 83         add a, e
5604 83         add a, e
5605 83         add a, e
5606 83         add a, e
5607 83         add a, e
5608 81         add a, c
5609 00         nop
560A 32343B     ld ($3B34), a
560D 2A0038     ld hl, ($3800)
5610 2D         dec l
5611 2E35       ld l, $35
5613 00         nop
5614 00         nop
5615 00         nop
5616 00         nop
5617 00         nop
5618 32343B     ld ($3B34), a
561B 2A0000     ld hl, ($0000)
561E 00         nop
561F 00         nop
5620 00         nop
5621 00         nop
5622 32343B     ld ($3B34), a
5625 2A0000     ld hl, ($0000)
5628 00         nop
5629 00         nop
562A 00         nop
562B 00         nop
562C 29         add hl, hl
562D 34         inc (hl)
562E 3C         inc a
562F 33         inc sp
5630 00         nop
5631 00         nop
5632 00         nop
5633 00         nop
5634 00         nop
5635 00         nop
5636 00         nop
5637 00         nop
5638 382D       jr c, $5667
563A 2E35       ld l, $35
563C 00         nop
563D 00         nop
563E 00         nop
563F 00         nop
5640 00         nop
5641 00         nop
5642 382D       jr c, $5671
5644 2E35       ld l, $35
5646 00         nop
5647 00         nop
5648 00         nop
5649 00         nop
564A 00         nop
564B 00         nop
564C 00         nop
564D 00         nop
564E 00         nop
564F 00         nop
5650 00         nop
5651 00         nop
5652 00         nop
5653 00         nop
5654 00         nop
5655 00         nop
5656 00         nop
5657 00         nop
5658 312A2B     ld sp, $2B2A
565B 39         add hl, sp
565C 00         nop
565D 00         nop
565E 00         nop
565F 00         nop
5660 00         nop
5661 00         nop
5662 37         scf
5663 2E2C       ld l, $2C
5665 2D         dec l
5666 39         add hl, sp
5667 0B         dec bc
5668 76         halt
5669 04         inc b
566A 60         ld h, b
566B 33         inc sp
566C 00         nop
566D F5         push af
566E C1         pop bc
566F 1E1D       ld e, $1D
5671 7E         ld a, (hl)
5672 85         add a, l
5673 2800       jr z, $5675
5675 00         nop
5676 00         nop
5677 1A         ld a, (de)
5678 1D         dec e
5679 7E         ld a, (hl)
567A 81         add a, c
567B 00         nop
567C 00         nop
567D 00         nop
567E 00         nop
567F 19         add hl, de
5680 0B         dec bc
5681 2D         dec l
5682 2E39       ld l, $39
5684 00         nop
5685 2633       ld h, $33
5687 3E00       ld a, $00
5689 312A39     ld sp, $392A
568C 39         add hl, sp
568D 2A3700     ld hl, ($0037)
5690 39         add hl, sp
5691 34         inc (hl)
5692 00         nop
5693 2834       jr z, $56C9
5695 33         inc sp
5696 39         add hl, sp
5697 2E33       ld l, $33
5699 3A2A1B     ld a, ($1B2A)
569C 1B         dec de
569D 1B         dec de
569E 0B         dec bc
569F 76         halt
56A0 04         inc b
56A1 6A         ld l, d
56A2 12         ld (de), a
56A3 00         nop
56A4 FA4114     jp m, $1441
56A7 0B         dec bc
56A8 0B         dec bc
56A9 DEEC       sbc a, $EC
56AB 1D         dec e
56AC 1D         dec e
56AD 1F         rra
56AE 1C         inc e
56AF 7E         ld a, (hl)
56B0 8B         adc a, e
56B1 0D         dec c
56B2 40         ld b, b
56B3 00         nop
56B4 00         nop
56B5 76         halt
56B6 04         inc b
56B7 74         ld (hl), h
56B8 02         ld (bc), a
56B9 00         nop
56BA FE76       cp $76
56BC 07         rlca
56BD D0         ret nc
56BE 02         ld (bc), a
56BF 00         nop
56C0 FB         ei
56C1 76         halt
56C2 07         rlca
56C3 DA2200     jp c, $0022
56C6 F5         push af
56C7 C1         pop bc
56C8 1E7E       ld e, $7E
56CA 82         add a, d
56CB 00         nop
56CC 00         nop
56CD 00         nop
56CE 00         nop
56CF 1A         ld a, (de)
56D0 1D         dec e
56D1 7E         ld a, (hl)
56D2 81         add a, c
56D3 00         nop
56D4 00         nop
56D5 00         nop
56D6 00         nop
56D7 19         add hl, de
56D8 0B         dec bc
56D9 3830       jr c, $570B
56DB 2E31       ld l, $31
56DD 310031     ld sp, $3100
56E0 2A3B2A     ld hl, ($2A3B)
56E3 31380E     ld sp, $0E38
56E6 0B         dec bc
56E7 76         halt
56E8 07         rlca
56E9 E41E00     call po, $001E
56EC F5         push af
56ED C1         pop bc
56EE 207E       jr nz, $576E
56F0 83         add a, e
56F1 00         nop
56F2 00         nop
56F3 00         nop
56F4 00         nop
56F5 1A         ld a, (de)
56F6 1D         dec e
56F7 217E84     ld hl, $847E
56FA 70         ld (hl), b
56FB 00         nop
56FC 00         nop
56FD 00         nop
56FE 19         add hl, de
56FF 0B         dec bc
5700 1D         dec e
5701 00         nop
5702 14         inc d
5703 00         nop
5704 3831       jr c, $5737
5706 34         inc (hl)
5707 3C         inc a
5708 0B         dec bc
5709 76         halt
570A 07         rlca
570B EE21       xor $21
570D 00         nop
570E F5         push af
570F C1         pop bc
5710 227E83     ld ($837E), hl
5713 40         ld b, b
5714 00         nop
5715 00         nop
5716 00         nop
5717 1A         ld a, (de)
5718 1D         dec e
5719 217E84     ld hl, $847E
571C 70         ld (hl), b
571D 00         nop
571E 00         nop
571F 00         nop
5720 19         add hl, de
5721 0B         dec bc
5722 1E00       ld e, $00
5724 14         inc d
5725 00         nop
5726 263B       ld h, $3B
5728 2A3726     ld hl, ($2637)
572B 2C         inc l
572C 2A0B76     ld hl, ($760B)
572F 07         rlca
5730 F8         ret m
5731 1E00       ld e, $00
5733 F5         push af
5734 C1         pop bc
5735 24         inc h
5736 7E         ld a, (hl)
5737 84         add a, h
5738 00         nop
5739 00         nop
573A 00         nop
573B 00         nop
573C 1A         ld a, (de)
573D 1D         dec e
573E 217E84     ld hl, $847E
5741 70         ld (hl), b
5742 00         nop
5743 00         nop
5744 00         nop
5745 19         add hl, de
5746 0B         dec bc
5747 1F         rra
5748 00         nop
5749 14         inc d
574A 00         nop
574B 2B         dec hl
574C 2638       ld h, $38
574E 39         add hl, sp
574F 0B         dec bc
5750 76         halt
5751 08         ex af, af'
5752 02         ld (bc), a
5753 24         inc h
5754 00         nop
5755 F5         push af
5756 C1         pop bc
5757 1D         dec e
5758 1C         inc e
5759 7E         ld a, (hl)
575A 84         add a, h
575B 2000       jr nz, $575D
575D 00         nop
575E 00         nop
575F 1A         ld a, (de)
5760 1D         dec e
5761 217E84     ld hl, $847E
5764 70         ld (hl), b
5765 00         nop
5766 00         nop
5767 00         nop
5768 19         add hl, de
5769 0B         dec bc
576A 2000       jr nz, $576C
576C 14         inc d
576D 00         nop
576E 3B         dec sp
576F 2A373E     ld hl, ($3E37)
5772 00         nop
5773 2B         dec hl
5774 2638       ld h, $38
5776 39         add hl, sp
5777 0B         dec bc
5778 76         halt
5779 08         ex af, af'
577A 0C         inc c
577B 23         inc hl
577C 00         nop
577D F5         push af
577E C1         pop bc
577F 1D         dec e
5780 1E7E       ld e, $7E
5782 84         add a, h
5783 40         ld b, b
5784 00         nop
5785 00         nop
5786 00         nop
5787 1A         ld a, (de)
5788 1D         dec e
5789 217E84     ld hl, $847E
578C 70         ld (hl), b
578D 00         nop
578E 00         nop
578F 00         nop
5790 19         add hl, de
5791 0B         dec bc
5792 210014     ld hl, $1400
5795 00         nop
5796 383A       jr c, $57D2
5798 2E28       ld l, $28
579A 2E29       ld l, $29
579C 2631       ld h, $31
579E 0B         dec bc
579F 76         halt
57A0 08         ex af, af'
57A1 162A       ld d, $2A
57A3 00         nop
57A4 F5         push af
57A5 C1         pop bc
57A6 1D         dec e
57A7 227E85     ld ($857E), hl
57AA 00         nop
57AB 00         nop
57AC 00         nop
57AD 00         nop
57AE 1A         ld a, (de)
57AF 227E83     ld ($837E), hl
57B2 40         ld b, b
57B3 00         nop
57B4 00         nop
57B5 00         nop
57B6 19         add hl, de
57B7 0B         dec bc
57B8 35         dec (hl)
57B9 37         scf
57BA 2A3838     ld hl, ($3838)
57BD 00         nop
57BE 3830       jr c, $57F0
57C0 2E31       ld l, $31
57C2 310031     ld sp, $3100
57C5 2A3B2A     ld hl, ($2A3B)
57C8 311B1B     ld sp, $1B1B
57CB 1B         dec de
57CC 0B         dec bc
57CD 76         halt
57CE 08         ex af, af'
57CF 2012       jr nz, $57E3
57D1 00         nop
57D2 FA4114     jp m, $1441
57D5 0B         dec bc
57D6 0B         dec bc
57D7 DEEC       sbc a, $EC
57D9 1E1C       ld e, $1C
57DB 24         inc h
57DC 1C         inc e
57DD 7E         ld a, (hl)
57DE 8C         adc a, h
57DF 02         ld (bc), a
57E0 00         nop
57E1 00         nop
57E2 00         nop
57E3 76         halt
57E4 08         ex af, af'
57E5 2A0F00     ld hl, ($000F)
57E8 F1         pop af
57E9 3B         dec sp
57EA 14         inc d
57EB C44116     call nz, $1641
57EE 1E24       ld e, $24
57F0 7E         ld a, (hl)
57F1 85         add a, l
57F2 60         ld h, b
57F3 00         nop
57F4 00         nop
57F5 00         nop
57F6 76         halt
57F7 08         ex af, af'
57F8 34         inc (hl)
57F9 2100FA     ld hl, $FA00
57FC 3B         dec sp
57FD 13         inc de
57FE 1D         dec e
57FF 7E         ld a, (hl)
5800 81         add a, c
5801 00         nop
5802 00         nop
5803 00         nop
5804 00         nop
5805 D9         exx
5806 3B         dec sp
5807 12         ld (de), a
5808 217E83     ld hl, $837E
580B 2000       jr nz, $580D
580D 00         nop
580E 00         nop
580F DEEC       sbc a, $EC
5811 1E1C       ld e, $1C
5813 24         inc h
5814 1C         inc e
5815 7E         ld a, (hl)
5816 8C         adc a, h
5817 02         ld (bc), a
5818 00         nop
5819 00         nop
581A 00         nop
581B 76         halt
581C 08         ex af, af'
581D 39         add hl, sp
581E 02         ld (bc), a
581F 00         nop
5820 FB         ei
5821 76         halt
5822 08         ex af, af'
5823 3E15       ld a, $15
5825 00         nop
5826 F41D24     call p, $241D
5829 23         inc hl
582A 22237E     ld ($7E23), hl
582D 8F         adc a, a
582E 12         ld (de), a
582F 9E         sbc a, (hl)
5830 00         nop
5831 00         nop
5832 1A         ld a, (de)
5833 1D         dec e
5834 7E         ld a, (hl)
5835 81         add a, c
5836 00         nop
5837 00         nop
5838 00         nop
5839 00         nop
583A 76         halt
583B 08         ex af, af'
583C 48         ld c, b
583D 17         rla
583E 00         nop
583F F41D24     call p, $241D
5842 23         inc hl
5843 22247E     ld ($7E24), hl
5846 8F         adc a, a
5847 12         ld (de), a
5848 A0         and b
5849 00         nop
584A 00         nop
584B 1A         ld a, (de)
584C 217E83     ld hl, $837E
584F 2000       jr nz, $5851
5851 00         nop
5852 00         nop
5853 163B       ld d, $3B
5855 76         halt
5856 08         ex af, af'
5857 52         ld d, d
5858 0E00       ld c, $00
585A F1         pop af
585B 2B         dec hl
585C 14         inc d
585D 211C7E     ld hl, $7E1C
5860 86         add a, (hl)
5861 48         ld c, b
5862 00         nop
5863 00         nop
5864 00         nop
5865 17         rla
5866 3B         dec sp
5867 76         halt
5868 08         ex af, af'
5869 66         ld h, (hl)
586A 13         inc de
586B 00         nop
586C EB         ex de, hl
586D 33         inc sp
586E 14         inc d
586F 1D         dec e
5870 7E         ld a, (hl)
5871 81         add a, c
5872 00         nop
5873 00         nop
5874 00         nop
5875 00         nop
5876 DF         rst $18
5877 217E83     ld hl, $837E
587A 2000       jr nz, $587C
587C 00         nop
587D 00         nop
587E 76         halt
587F 08         ex af, af'
5880 70         ld (hl), b
5881 1100F1     ld de, $F100
5884 2614       ld h, $14
5886 1D         dec e
5887 24         inc h
5888 1F         rra
5889 211F7E     ld hl, $7E1F
588C 8F         adc a, a
588D 0F         rrca
588E 62         ld h, d
588F 00         nop
5890 00         nop
5891 15         dec d
5892 33         inc sp
5893 76         halt
5894 08         ex af, af'
5895 7A         ld a, d
5896 0B         dec bc
5897 00         nop
5898 F4261A     call p, $1A26
589B 1C         inc e
589C 7E         ld a, (hl)
589D 00         nop
589E 00         nop
589F 00         nop
58A0 00         nop
58A1 00         nop
58A2 76         halt
58A3 08         ex af, af'
58A4 84         add a, h
58A5 1100FA     ld de, $FA00
58A8 33         inc sp
58A9 DB3B       in a, ($3B)
58AB DEF4       sbc a, $F4
58AD 261A       ld h, $1A
58AF 1E21       ld e, $21
58B1 7E         ld a, (hl)
58B2 85         add a, l
58B3 48         ld c, b
58B4 00         nop
58B5 00         nop
58B6 00         nop
58B7 76         halt
58B8 08         ex af, af'
58B9 8E         adc a, (hl)
58BA 03         inc bc
58BB 00         nop
58BC F3         di
58BD 33         inc sp
58BE 76         halt
58BF 08         ex af, af'
58C0 98         sbc a, b
58C1 02         ld (bc), a
58C2 00         nop
58C3 FE76       cp $76
58C5 0B         dec bc
58C6 B8         cp b
58C7 0A         ld a, (bc)
58C8 00         nop
58C9 F8         ret m
58CA 0B         dec bc
58CB 2639       ld h, $39
58CD 39         add hl, sp
58CE 2628       ld h, $28
58D0 B0         or b
58D1 0B         dec bc
58D2 76         halt
58D3 0C         inc c
58D4 1C         inc e
58D5 0A         ld a, (bc)
58D6 00         nop
58D7 EC1F1C     call pe, $1C1F
58DA 7E         ld a, (hl)
58DB 85         add a, l
58DC 70         ld (hl), b
58DD 00         nop
58DE 00         nop
58DF 00         nop
58E0 76         halt
58E1 76         halt
58E2 00         nop
58E3 00         nop
58E4 00         nop
58E5 00         nop
58E6 00         nop
58E7 00         nop
58E8 00         nop
58E9 00         nop
58EA 00         nop
58EB 00         nop
58EC 00         nop
58ED 00         nop
58EE 00         nop
58EF 00         nop
58F0 00         nop
58F1 00         nop
58F2 00         nop
58F3 00         nop
58F4 00         nop
58F5 00         nop
58F6 00         nop
58F7 00         nop
58F8 00         nop
58F9 00         nop
58FA 00         nop
58FB 00         nop
58FC 00         nop
58FD 00         nop
58FE 00         nop
58FF 00         nop
5900 00         nop
5901 00         nop
5902 76         halt
5903 00         nop
5904 00         nop
5905 00         nop
5906 00         nop
5907 00         nop
5908 00         nop
5909 00         nop
590A 00         nop
590B 00         nop
590C 00         nop
590D 00         nop
590E 00         nop
590F 00         nop
5910 00         nop
5911 00         nop
5912 00         nop
5913 00         nop
5914 00         nop
5915 00         nop
5916 00         nop
5917 00         nop
5918 00         nop
5919 00         nop
591A 00         nop
591B 00         nop
591C 00         nop
591D 00         nop
591E 00         nop
591F 00         nop
5920 00         nop
5921 00         nop
5922 00         nop
5923 76         halt
5924 00         nop
5925 00         nop
5926 00         nop
5927 00         nop
5928 00         nop
5929 00         nop
592A 00         nop
592B 00         nop
592C 00         nop
592D 00         nop
592E 00         nop
592F 00         nop
5930 00         nop
5931 00         nop
5932 00         nop
5933 00         nop
5934 00         nop
5935 00         nop
5936 00         nop
5937 00         nop
5938 00         nop
5939 00         nop
593A 00         nop
593B 00         nop
593C 00         nop
593D 00         nop
593E 00         nop
593F 00         nop
5940 00         nop
5941 00         nop
5942 00         nop
5943 00         nop
5944 76         halt
5945 00         nop
5946 00         nop
5947 00         nop
5948 00         nop
5949 00         nop
594A 00         nop
594B 00         nop
594C 00         nop
594D 00         nop
594E 00         nop
594F 00         nop
5950 00         nop
5951 00         nop
5952 00         nop
5953 00         nop
5954 00         nop
5955 00         nop
5956 00         nop
5957 00         nop
5958 00         nop
5959 00         nop
595A 00         nop
595B 00         nop
595C 00         nop
595D 00         nop
595E 00         nop
595F 00         nop
5960 00         nop
5961 00         nop
5962 00         nop
5963 00         nop
5964 00         nop
5965 76         halt
5966 00         nop
5967 00         nop
5968 00         nop
5969 00         nop
596A 00         nop
596B 00         nop
596C 00         nop
596D 00         nop
596E 00         nop
596F 00         nop
5970 00         nop
5971 00         nop
5972 00         nop
5973 00         nop
5974 00         nop
5975 00         nop
5976 00         nop
5977 00         nop
5978 00         nop
5979 00         nop
597A 00         nop
597B 00         nop
597C 00         nop
597D 00         nop
597E 00         nop
597F 00         nop
5980 00         nop
5981 00         nop
5982 00         nop
5983 00         nop
5984 00         nop
5985 00         nop
5986 76         halt
5987 00         nop
5988 00         nop
5989 00         nop
598A 00         nop
598B 00         nop
598C 00         nop
598D 00         nop
598E 00         nop
598F 00         nop
5990 00         nop
5991 00         nop
5992 00         nop
5993 00         nop
5994 00         nop
5995 00         nop
5996 00         nop
5997 00         nop
5998 00         nop
5999 00         nop
599A 00         nop
599B 00         nop
599C 00         nop
599D 00         nop
599E 00         nop
599F 00         nop
59A0 00         nop
59A1 00         nop
59A2 00         nop
59A3 00         nop
59A4 00         nop
59A5 00         nop
59A6 00         nop
59A7 76         halt
59A8 00         nop
59A9 00         nop
59AA 00         nop
59AB 00         nop
59AC 00         nop
59AD 00         nop
59AE 00         nop
59AF 00         nop
59B0 00         nop
59B1 00         nop
59B2 00         nop
59B3 00         nop
59B4 00         nop
59B5 00         nop
59B6 00         nop
59B7 00         nop
59B8 00         nop
59B9 00         nop
59BA 00         nop
59BB 00         nop
59BC 00         nop
59BD 00         nop
59BE 00         nop
59BF 00         nop
59C0 00         nop
59C1 00         nop
59C2 00         nop
59C3 00         nop
59C4 00         nop
59C5 00         nop
59C6 00         nop
59C7 00         nop
59C8 76         halt
59C9 00         nop
59CA 00         nop
59CB 00         nop
59CC 00         nop
59CD 00         nop
59CE 00         nop
59CF 00         nop
59D0 00         nop
59D1 00         nop
59D2 00         nop
59D3 00         nop
59D4 00         nop
59D5 00         nop
59D6 00         nop
59D7 00         nop
59D8 00         nop
59D9 00         nop
59DA 00         nop
59DB 00         nop
59DC 00         nop
59DD 00         nop
59DE 00         nop
59DF 00         nop
59E0 00         nop
59E1 00         nop
59E2 00         nop
59E3 00         nop
59E4 00         nop
59E5 00         nop
59E6 00         nop
59E7 00         nop
59E8 00         nop
59E9 76         halt
59EA 00         nop
59EB 00         nop
59EC 00         nop
59ED 00         nop
59EE 00         nop
59EF 00         nop
59F0 00         nop
59F1 00         nop
59F2 00         nop
59F3 00         nop
59F4 00         nop
59F5 00         nop
59F6 00         nop
59F7 00         nop
59F8 00         nop
59F9 00         nop
59FA 00         nop
59FB 00         nop
59FC 00         nop
59FD 00         nop
59FE 00         nop
59FF 00         nop
5A00 00         nop
5A01 00         nop
5A02 00         nop
5A03 00         nop
5A04 00         nop
5A05 00         nop
5A06 00         nop
5A07 00         nop
5A08 00         nop
5A09 00         nop
5A0A 76         halt
5A0B 00         nop
5A0C 00         nop
5A0D 00         nop
5A0E 00         nop
5A0F 00         nop
5A10 00         nop
5A11 00         nop
5A12 00         nop
5A13 00         nop
5A14 00         nop
5A15 00         nop
5A16 00         nop
5A17 00         nop
5A18 00         nop
5A19 00         nop
5A1A 00         nop
5A1B 00         nop
5A1C 00         nop
5A1D 00         nop
5A1E 00         nop
5A1F 00         nop
5A20 00         nop
5A21 00         nop
5A22 00         nop
5A23 00         nop
5A24 00         nop
5A25 00         nop
5A26 00         nop
5A27 00         nop
5A28 00         nop
5A29 00         nop
5A2A 00         nop
5A2B 76         halt
5A2C 00         nop
5A2D 00         nop
5A2E 00         nop
5A2F 00         nop
5A30 00         nop
5A31 00         nop
5A32 00         nop
5A33 00         nop
5A34 00         nop
5A35 00         nop
5A36 00         nop
5A37 00         nop
5A38 00         nop
5A39 00         nop
5A3A 00         nop
5A3B 00         nop
5A3C 00         nop
5A3D 00         nop
5A3E 00         nop
5A3F 00         nop
5A40 00         nop
5A41 00         nop
5A42 00         nop
5A43 00         nop
5A44 00         nop
5A45 00         nop
5A46 00         nop
5A47 00         nop
5A48 00         nop
5A49 00         nop
5A4A 00         nop
5A4B 00         nop
5A4C 76         halt
5A4D 00         nop
5A4E 00         nop
5A4F 00         nop
5A50 00         nop
5A51 00         nop
5A52 00         nop
5A53 00         nop
5A54 00         nop
5A55 00         nop
5A56 00         nop
5A57 00         nop
5A58 00         nop
5A59 00         nop
5A5A 00         nop
5A5B 00         nop
5A5C 00         nop
5A5D 00         nop
5A5E 00         nop
5A5F 00         nop
5A60 00         nop
5A61 00         nop
5A62 00         nop
5A63 00         nop
5A64 00         nop
5A65 00         nop
5A66 00         nop
5A67 00         nop
5A68 00         nop
5A69 00         nop
5A6A 00         nop
5A6B 00         nop
5A6C 00         nop
5A6D 76         halt
5A6E 00         nop
5A6F 00         nop
5A70 00         nop
5A71 00         nop
5A72 00         nop
5A73 00         nop
5A74 00         nop
5A75 00         nop
5A76 00         nop
5A77 00         nop
5A78 00         nop
5A79 00         nop
5A7A 00         nop
5A7B 00         nop
5A7C 00         nop
5A7D 00         nop
5A7E 00         nop
5A7F 00         nop
5A80 00         nop
5A81 00         nop
5A82 00         nop
5A83 00         nop
5A84 00         nop
5A85 00         nop
5A86 00         nop
5A87 00         nop
5A88 00         nop
5A89 00         nop
5A8A 00         nop
5A8B 00         nop
5A8C 00         nop
5A8D 00         nop
5A8E 76         halt
5A8F 00         nop
5A90 00         nop
5A91 00         nop
5A92 00         nop
5A93 00         nop
5A94 00         nop
5A95 00         nop
5A96 00         nop
5A97 00         nop
5A98 00         nop
5A99 00         nop
5A9A 00         nop
5A9B 00         nop
5A9C 00         nop
5A9D 00         nop
5A9E 00         nop
5A9F 00         nop
5AA0 00         nop
5AA1 00         nop
5AA2 00         nop
5AA3 00         nop
5AA4 00         nop
5AA5 00         nop
5AA6 00         nop
5AA7 00         nop
5AA8 00         nop
5AA9 00         nop
5AAA 00         nop
5AAB 00         nop
5AAC 00         nop
5AAD 00         nop
5AAE 00         nop
5AAF 76         halt
5AB0 00         nop
5AB1 00         nop
5AB2 00         nop
5AB3 00         nop
5AB4 00         nop
5AB5 00         nop
5AB6 00         nop
5AB7 00         nop
5AB8 00         nop
5AB9 00         nop
5ABA 00         nop
5ABB 00         nop
5ABC 00         nop
5ABD 00         nop
5ABE 00         nop
5ABF 00         nop
5AC0 00         nop
5AC1 00         nop
5AC2 00         nop
5AC3 00         nop
5AC4 00         nop
5AC5 00         nop
5AC6 00         nop
5AC7 00         nop
5AC8 00         nop
5AC9 00         nop
5ACA 00         nop
5ACB 00         nop
5ACC 00         nop
5ACD 00         nop
5ACE 00         nop
5ACF 00         nop
5AD0 76         halt
5AD1 00         nop
5AD2 00         nop
5AD3 00         nop
5AD4 00         nop
5AD5 00         nop
5AD6 00         nop
5AD7 00         nop
5AD8 00         nop
5AD9 00         nop
5ADA 00         nop
5ADB 00         nop
5ADC 00         nop
5ADD 00         nop
5ADE 00         nop
5ADF 00         nop
5AE0 00         nop
5AE1 00         nop
5AE2 00         nop
5AE3 00         nop
5AE4 00         nop
5AE5 00         nop
5AE6 00         nop
5AE7 00         nop
5AE8 00         nop
5AE9 00         nop
5AEA 00         nop
5AEB 00         nop
5AEC 00         nop
5AED 00         nop
5AEE 00         nop
5AEF 00         nop
5AF0 00         nop
5AF1 76         halt
5AF2 00         nop
5AF3 00         nop
5AF4 00         nop
5AF5 00         nop
5AF6 00         nop
5AF7 00         nop
5AF8 00         nop
5AF9 00         nop
5AFA 00         nop
5AFB 00         nop
5AFC 00         nop
5AFD 00         nop
5AFE 00         nop
5AFF 00         nop
5B00 00         nop
5B01 00         nop
5B02 00         nop
5B03 00         nop
5B04 00         nop
5B05 00         nop
5B06 00         nop
5B07 00         nop
5B08 00         nop
5B09 00         nop
5B0A 00         nop
5B0B 00         nop
5B0C 00         nop
5B0D 00         nop
5B0E 00         nop
5B0F 00         nop
5B10 00         nop
5B11 00         nop
5B12 76         halt
5B13 00         nop
5B14 00         nop
5B15 00         nop
5B16 00         nop
5B17 00         nop
5B18 00         nop
5B19 00         nop
5B1A 00         nop
5B1B 00         nop
5B1C 00         nop
5B1D 00         nop
5B1E 00         nop
5B1F 00         nop
5B20 00         nop
5B21 00         nop
5B22 00         nop
5B23 00         nop
5B24 00         nop
5B25 00         nop
5B26 00         nop
5B27 00         nop
5B28 00         nop
5B29 00         nop
5B2A 00         nop
5B2B 00         nop
5B2C 00         nop
5B2D 00         nop
5B2E 00         nop
5B2F 00         nop
5B30 00         nop
5B31 00         nop
5B32 00         nop
5B33 76         halt
5B34 00         nop
5B35 00         nop
5B36 00         nop
5B37 00         nop
5B38 00         nop
5B39 00         nop
5B3A 00         nop
5B3B 00         nop
5B3C 00         nop
5B3D 00         nop
5B3E 00         nop
5B3F 00         nop
5B40 00         nop
5B41 00         nop
5B42 00         nop
5B43 00         nop
5B44 00         nop
5B45 00         nop
5B46 00         nop
5B47 00         nop
5B48 00         nop
5B49 00         nop
5B4A 00         nop
5B4B 00         nop
5B4C 00         nop
5B4D 00         nop
5B4E 00         nop
5B4F 00         nop
5B50 00         nop
5B51 00         nop
5B52 00         nop
5B53 00         nop
5B54 76         halt
5B55 00         nop
5B56 00         nop
5B57 00         nop
5B58 00         nop
5B59 00         nop
5B5A 00         nop
5B5B 00         nop
5B5C 00         nop
5B5D 00         nop
5B5E 00         nop
5B5F 00         nop
5B60 00         nop
5B61 00         nop
5B62 00         nop
5B63 00         nop
5B64 00         nop
5B65 00         nop
5B66 00         nop
5B67 00         nop
5B68 00         nop
5B69 00         nop
5B6A 00         nop
5B6B 00         nop
5B6C 00         nop
5B6D 00         nop
5B6E 00         nop
5B6F 00         nop
5B70 00         nop
5B71 00         nop
5B72 00         nop
5B73 00         nop
5B74 00         nop
5B75 76         halt
5B76 00         nop
5B77 00         nop
5B78 00         nop
5B79 00         nop
5B7A 00         nop
5B7B 00         nop
5B7C 00         nop
5B7D 00         nop
5B7E 00         nop
5B7F 00         nop
5B80 00         nop
5B81 00         nop
5B82 00         nop
5B83 00         nop
5B84 00         nop
5B85 00         nop
5B86 00         nop
5B87 00         nop
5B88 00         nop
5B89 00         nop
5B8A 00         nop
5B8B 00         nop
5B8C 00         nop
5B8D 00         nop
5B8E 00         nop
5B8F 00         nop
5B90 00         nop
5B91 00         nop
5B92 00         nop
5B93 00         nop
5B94 00         nop
5B95 00         nop
5B96 76         halt
5B97 00         nop
5B98 00         nop
5B99 00         nop
5B9A 00         nop
5B9B 00         nop
5B9C 00         nop
5B9D 00         nop
5B9E 00         nop
5B9F 00         nop
5BA0 00         nop
5BA1 00         nop
5BA2 00         nop
5BA3 00         nop
5BA4 00         nop
5BA5 00         nop
5BA6 00         nop
5BA7 00         nop
5BA8 00         nop
5BA9 00         nop
5BAA 00         nop
5BAB 00         nop
5BAC 00         nop
5BAD 00         nop
5BAE 00         nop
5BAF 00         nop
5BB0 00         nop
5BB1 00         nop
5BB2 00         nop
5BB3 00         nop
5BB4 00         nop
5BB5 00         nop
5BB6 00         nop
5BB7 76         halt
5BB8 00         nop
5BB9 00         nop
5BBA 00         nop
5BBB 00         nop
5BBC 00         nop
5BBD 00         nop
5BBE 00         nop
5BBF 00         nop
5BC0 00         nop
5BC1 00         nop
5BC2 00         nop
5BC3 00         nop
5BC4 00         nop
5BC5 00         nop
5BC6 00         nop
5BC7 00         nop
5BC8 00         nop
5BC9 00         nop
5BCA 00         nop
5BCB 00         nop
5BCC 00         nop
5BCD 00         nop
5BCE 00         nop
5BCF 00         nop
5BD0 00         nop
5BD1 00         nop
5BD2 00         nop
5BD3 00         nop
5BD4 00         nop
5BD5 00         nop
5BD6 00         nop
5BD7 00         nop
5BD8 76         halt
5BD9 00         nop
5BDA 00         nop
5BDB 00         nop
5BDC 00         nop
5BDD 00         nop
5BDE 00         nop
5BDF 00         nop
5BE0 00         nop
5BE1 00         nop
5BE2 00         nop
5BE3 00         nop
5BE4 00         nop
5BE5 00         nop
5BE6 00         nop
5BE7 00         nop
5BE8 00         nop
5BE9 00         nop
5BEA 00         nop
5BEB 00         nop
5BEC 00         nop
5BED 00         nop
5BEE 00         nop
5BEF 00         nop
5BF0 00         nop
5BF1 00         nop
5BF2 00         nop
5BF3 00         nop
5BF4 00         nop
5BF5 00         nop
5BF6 00         nop
5BF7 00         nop
5BF8 00         nop
5BF9 76         halt
5BFA 71         ld (hl), c
5BFB 00         nop
5BFC 00         nop
5BFD 00         nop
5BFE 00         nop
5BFF 00         nop
5C00 7B         ld a, e
5C01 83         add a, e
5C02 2000       jr nz, $5C04
5C04 00         nop
5C05 00         nop
5C06 6B         ld l, e
5C07 88         adc a, b
5C08 7A         ld a, d
5C09 00         nop
5C0A 00         nop
5C0B 00         nop
5C0C F3         di
5C0D 86         add a, (hl)
5C0E 04         inc b
5C0F 00         nop
5C10 00         nop
5C11 00         nop
5C12 86         add a, (hl)
5C13 00         nop
5C14 00         nop
5C15 00         nop
5C16 00         nop
5C17 81         add a, c
5C18 00         nop
5C19 00         nop
5C1A 00         nop
5C1B 00         nop
5C1C AF         xor a
5C1D 01668F     ld bc, $8F66
5C20 0F         rrca
5C21 6C         ld l, h
5C22 00         nop
5C23 00         nop
5C24 7C         ld a, h
5C25 86         add a, (hl)
5C26 08         ex af, af'
5C27 00         nop
5C28 00         nop
5C29 00         nop
5C2A 78         ld a, b
5C2B 00         nop
5C2C 00         nop
5C2D 00         nop
5C2E 00         nop
5C2F 00         nop
5C30 77         ld (hl), a
5C31 88         adc a, b
5C32 48         ld c, b
5C33 00         nop
5C34 00         nop
5C35 00         nop
5C36 80         add a, b
