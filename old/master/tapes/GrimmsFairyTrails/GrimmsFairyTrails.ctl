;------------------------------------------------------------------------------
; CPU::Z80::Disassembler control file
;------------------------------------------------------------------------------

4009 :F ../../t/GrimmsFairyTrails.p
400C := D_FILE
400E := DF_CC

4081 EA       						:B 
									:; REM of start of assembly
4082-4083 0000         				:B 

4084 00         nop
4085 00         nop
4086 00         nop
4087 00         nop
4088 2600       ld h, $00			:C PUT_CHAR_AT
									:#;--------------------------------------------------------------------------------
									:#; input is BC with col-row, A with char to print
									:#; output is HL with screen address and D with previous char
									:#;--------------------------------------------------------------------------------
									
408A 69         ld l, c
408B CB25       sla l
408D CB25       sla l
408F CB25       sla l
4091 CB25       sla l
4093 CB25       sla l
4095 CB14       rl h
4097 59         ld e, c
4098 1600       ld d, $00
409A 19         add hl, de			:; HL=C*33
409B 58         ld e, b
409C 19         add hl, de			:; HL=C*33+B
409D ED5B0C40   ld de, ($400C)		:; get display file
40A1 19         add hl, de
40A2 110100     ld de, $0001		:; add one
40A5 19         add hl, de
40A6 56         ld d, (hl)			:; read previous char
40A7 77         ld (hl), a			:; write char
40A8 C9         ret
40A9 00         nop
40AA 00         nop
40AB 00         nop
40AC 00         nop
40AD 00         nop
40AE 00         nop
40AF 00         nop
40B0 00         nop
40B1 00         nop
40B2 00         nop
40B3 00         nop
40B4 00         nop
40B5 00         nop
40B6 00         nop
40B7 00         nop
40B8 00         nop
40B9 00         nop
40BA 00         nop
40BB 00         nop
40BC 00         nop
40BD 00         nop
40BE 00         nop
40BF 00         nop
40C0 00         nop
40C1 00         nop
40C2 00         nop
40C3 00         nop
40C4 00         nop
40C5 00         nop
40C6 00         nop
40C7 00         nop
40C8 00         nop
40C9 00         nop
40CA 00         nop
40CB 00         nop
40CC 00         nop
40CD 00         nop
40CE 00         nop
40CF 00         nop
40D0 00         nop
40D1 00         nop
40D2 00         nop
40D3 00         nop
40D4 00         nop
40D5 00         nop
40D6 00         nop
40D7 00         nop
40D8 00         nop
40D9 00         nop
40DA 00         nop
40DB 00         nop
40DC 00         nop
40DD 00         nop
40DE 00         nop
40DF 00         nop
40E0 0E16       ld c, $16			:C FILL_SCR
									:#;--------------------------------------------------------------------------------
									:#; Fill 22 center lines of screen with character in BACKGROUND+1
									:#;--------------------------------------------------------------------------------
									:; C=22 lines
40E2 2A0C40     ld hl, ($400C)
40E5 23         inc hl
40E6 112100     ld de, $0021		:; size of row
40E9 0620       ld b, $20			:C fill_row
									:; B=32 chars
40EB ED5A       adc hl, de			:; move to next row
40ED 22FF40     ld ($40FF), hl
40F0 3680		ld (hl), $80		:C BACKGROUND					
40F2 23         inc hl
40F3 05         dec b
40F4 C2F040     jp nz, $40F0
40F7 2AFF40     ld hl, ($40FF)
40FA 0D         dec c
40FB C2E940     jp nz, $40E9
40FE C9         ret
40FF BD63							:W fill_line_addr
4101 00         nop
4102 061E       ld b, $1E			:C DRAW_DOTS
									:#;--------------------------------------------------------------------------------
									:#; Fill a rectangle of dots before the maze walls are drawn
									:#;--------------------------------------------------------------------------------
									:; B=30 column 30
4104 0E0F       ld c, $0F			:; C=15 row 15
4106 3E1B       ld a, $1B			:C draw_next_dot
									:; A=dot
4108 CD8840     call $4088			:; put a dot
410B 0D         dec c
410C 79         ld a, c
410D FE01       cp $01
410F 20F5       jr nz, $4106
4111 0E0F       ld c, $0F			:; C=row 15
4113 05         dec b
4114 20F0       jr nz, $4106
4116 C9         ret
4117 00         nop
4118 0E0A       ld c, $0A			:C DRAW_WALLS1
									:#;--------------------------------------------------------------------------------
									:#; Draw walls
									:#;--------------------------------------------------------------------------------
									:; C=row 10
411A 061D       ld b, $1D			:; B=column 29 
411C 3E80       ld a, $80			:C wall_next_dot1
									:; char=solid block
411E CD8840     call $4088			:; put a char
4121 0D         dec c
4122 79         ld a, c
4123 FE06       cp $06
4125 20F5       jr nz, $411C		:; repeat until row 6
4127 0E0A       ld c, $0A			:; C=row 10
4129 05         dec b
412A 05         dec b				:; C=column -= 2
412B 78         ld a, b
412C FE0F       cp $0F				:; repeat until column 15
412E 20EC       jr nz, $411C
4130 0E0A       ld c, $0A			:; C=row 10
4132 060E       ld b, $0E			:C wall_next_dot2
									:; B=column 14
4134 3E80       ld a, $80			:C wall_next_dot3
									:; char=solid block
4136 CD8840     call $4088			:; put a char
4139 0D         dec c
413A 79         ld a, c
413B FE06       cp $06
413D 20F5       jr nz, $4134		:; repeat until row 6
413F 0E0A       ld c, $0A			:; C=row 10
4141 05         dec b
4142 05         dec b
4143 78         ld a, b
4144 FE00       cp $00
4146 20EC       jr nz, $4134
4148 0610       ld b, $10
414A 0E0A       ld c, $0A
414C 3E80       ld a, $80			:C wall_next_dot4
414E CD8840     call $4088
4151 05         dec b
4152 78         ld a, b
4153 FE0E       cp $0E
4155 20F5       jr nz, $414C
4157 0E07       ld c, $07
4159 0610       ld b, $10
415B 3E80       ld a, $80			:C wall_next_dot5
415D CD8840     call $4088
4160 05         dec b
4161 78         ld a, b
4162 FE0E       cp $0E
4164 20F5       jr nz, $415B
4166 C9         ret
4167 00         nop
4168 00         nop
4169 00         nop
416A 00         nop
416B 00         nop
416C 00         nop
416D 00         nop
416E 061D       ld b, $1D			:C DRAW_WALLS2
4170 0E0C       ld c, $0C			:C code2
4172 3E80       ld a, $80			:C wall_next_dot6
4174 CD8840     call $4088
4177 05         dec b
4178 CD8840     call $4088
417B 05         dec b
417C 05         dec b
417D 78         ld a, b
417E FE02       cp $02				:C code5
4180 20F0       jr nz, $4172
4182 061D       ld b, $1D			:C code3
4184 0E03       ld c, $03			:C code4
4186 3E80       ld a, $80			:C wall_next_dot7
4188 CD8840     call $4088
418B 05         dec b
418C CD8840     call $4088
418F 05         dec b
4190 05         dec b
4191 78         ld a, b
4192 FE02       cp $02				:C code6
4194 20F0       jr nz, $4186
4196 C9         ret
4197 00         nop
4198 00         nop
4199 00         nop
419A 00         nop
419B 00         nop
419C 00         nop
419D 00         nop
419E 00         nop
419F 00         nop
41A0 02         ld (bc), a
41A1 1E
41A2 10       ld e, $10				:B var4
41A3 09         add hl, bc			:B var5
41A4 02         ld (bc), a
41A5 1E00       ld e, $00
41A7 00         nop
41A8 09         add hl, bc
41A9 1820       jr $41CB
41AB 62         ld h, d
41AC 01								:B var3			
41AD 01
41AE 00								:B var2
41AF 00         nop
41B0 00         nop					:B var1
41B1 00         nop
41B2 00         nop
41B3 00         nop
41B4 00         nop
41B5 00         nop
41B6 00         nop
41B7 00         nop
41B8 00         nop
41B9 00         nop
41BA 00         nop
41BB 00         nop
41BC 00         nop
41BD 00         nop
41BE 00         nop
41BF 00         nop
41C0 00         nop
41C1 00         nop
41C2 00         nop
41C3 00         nop
41C4 00         nop
41C5 00         nop
41C6 00         nop
41C7 00         nop
41C8 00         nop
41C9 00         nop
41CA 00         nop
41CB 00         nop
41CC 00         nop
41CD 00         nop
41CE 00         nop
41CF 00         nop
41D0 00         nop
41D1 00         nop
41D2 00         nop
41D3 00         nop
41D4 21A141     ld hl, $41A1
41D7 46         ld b, (hl)
41D8 21A041     ld hl, $41A0
41DB 4E         ld c, (hl)
41DC 3E34       ld a, $34
41DE CD8840     call $4088
41E1 C9         ret
41E2 00         nop
41E3 00         nop
41E4 00         nop
41E5 00         nop
41E6 CDD441     call $41D4
41E9 23         inc hl
41EA 7E         ld a, (hl)
41EB FE80       cp $80
41ED C8         ret z
41EE FE1B       cp $1B
41F0 CC9042     call z, $4290
41F3 11B041     ld de, $41B0
41F6 1A         ld a, (de)
41F7 FEC8       cp $C8
41F9 C8         ret z
41FA 3E00       ld a, $00
41FC CD8840     call $4088
41FF 04         inc b
4200 3E34       ld a, $34
4202 CD8840     call $4088
4205 78         ld a, b
4206 32A141     ld ($41A1), a
4209 C9         ret
420A 00         nop
420B 00         nop
420C 00         nop
420D 00         nop
420E CDD441     call $41D4
4211 2B         dec hl
4212 7E         ld a, (hl)
4213 FE80       cp $80
4215 C8         ret z
4216 FE1B       cp $1B
4218 CC9042     call z, $4290
421B 11B041     ld de, $41B0
421E 1A         ld a, (de)
421F FEC8       cp $C8
4221 C8         ret z
4222 3E00       ld a, $00
4224 CD8840     call $4088
4227 05         dec b
4228 3E34       ld a, $34
422A CD8840     call $4088
422D 78         ld a, b
422E 32A141     ld ($41A1), a
4231 C9         ret
4232 00         nop
4233 00         nop
4234 00         nop
4235 00         nop
4236 CDD441     call $41D4
4239 112100     ld de, $0021
423C ED52       sbc hl, de
423E 7E         ld a, (hl)
423F FE80       cp $80
4241 C8         ret z
4242 FE1B       cp $1B
4244 CC9042     call z, $4290
4247 11B041     ld de, $41B0
424A 1A         ld a, (de)
424B FEC8       cp $C8
424D C8         ret z
424E 3E00       ld a, $00
4250 CD8840     call $4088
4253 0D         dec c
4254 3E34       ld a, $34
4256 CD8840     call $4088
4259 79         ld a, c
425A 32A041     ld ($41A0), a
425D C9         ret
425E 00         nop
425F 00         nop
4260 00         nop
4261 00         nop
4262 00         nop
4263 CDD441     call $41D4
4266 112100     ld de, $0021
4269 ED5A       adc hl, de
426B 7E         ld a, (hl)
426C FE80       cp $80
426E C8         ret z
426F FE1B       cp $1B
4271 CC9042     call z, $4290
4274 11B041     ld de, $41B0
4277 1A         ld a, (de)
4278 FEC8       cp $C8
427A C8         ret z
427B 3E00       ld a, $00
427D CD8840     call $4088
4280 0C         inc c
4281 3E34       ld a, $34
4283 CD8840     call $4088
4286 79         ld a, c
4287 32A041     ld ($41A0), a
428A C9         ret
428B 00         nop
428C 00         nop
428D 00         nop
428E 00         nop
428F 00         nop
4290 3AB041     ld a, ($41B0)
4293 C601       add a, $01
4295 32B041     ld ($41B0), a
4298 C9         ret
4299 00         nop
429A 00         nop
429B 00         nop
429C 00         nop
429D 00         nop
429E 00         nop
429F 00         nop
42A0 00         nop
42A1 00         nop
42A2 00         nop
42A3 00         nop
42A4 00         nop
42A5 00         nop
42A6 00         nop
42A7 00         nop
42A8 00         nop
42A9 00         nop
42AA 00         nop
42AB 00         nop
42AC 00         nop
42AD 00         nop
42AE 00         nop
42AF 00         nop
42B0 00         nop
42B1 00         nop
42B2 00         nop
42B3 00         nop
42B4 00         nop
42B5 00         nop
42B6 00         nop
42B7 00         nop
42B8 00         nop
42B9 00         nop
42BA 00         nop
42BB 00         nop
42BC 00         nop
42BD 00         nop
42BE 00         nop
42BF 00         nop
42C0 00         nop
42C1 00         nop
42C2 00         nop
42C3 00         nop
42C4 00         nop
42C5 00         nop
42C6 00         nop
42C7 00         nop
42C8 00         nop
42C9 00         nop
42CA 00         nop
42CB 00         nop
42CC 00         nop
42CD 00         nop
42CE 00         nop
42CF 00         nop
42D0 00         nop
42D1 00         nop
42D2 00         nop
42D3 00         nop
42D4 00         nop
42D5 00         nop
42D6 00         nop
42D7 00         nop
42D8 00         nop
42D9 00         nop
42DA 00         nop
42DB 00         nop
42DC 00         nop
42DD 00         nop
42DE 00         nop
42DF 00         nop
42E0 00         nop
42E1 00         nop
42E2 00         nop
42E3 00         nop
42E4 00         nop
42E5 00         nop
42E6 00         nop
42E7 00         nop
42E8 00         nop
42E9 00         nop
42EA 00         nop
42EB 00         nop
42EC 00         nop
42ED 00         nop
42EE 00         nop
42EF 00         nop
42F0 00         nop
42F1 00         nop
42F2 00         nop
42F3 00         nop
42F4 00         nop
42F5 00         nop
42F6 00         nop
42F7 00         nop
42F8 00         nop
42F9 00         nop
42FA 00         nop
42FB 00         nop
42FC 00         nop
42FD 00         nop
42FE 00         nop
42FF 00         nop
4300 00         nop
4301 00         nop
4302 00         nop
4303 00         nop
4304 00         nop
4305 00         nop
4306 00         nop
4307 00         nop
4308 00         nop
4309 00         nop
430A 00         nop
430B 00         nop
430C 00         nop
430D 00         nop
430E 00         nop
430F 00         nop
4310 00         nop
4311 00         nop
4312 00         nop
4313 00         nop
4314 00         nop
4315 00         nop
4316 00         nop
4317 00         nop
4318 00         nop
4319 00         nop
431A 00         nop
431B 00         nop
431C CDF943     call $43F9
431F CD3043     call $4330
4322 3AAE41     ld a, ($41AE)
4325 FE3D       cp $3D
4327 C8         ret z
4328 C31C43     jp $431C
432B 00         nop
432C 00         nop
432D 00         nop
432E 00         nop
432F 00         nop
4330 3AA041     ld a, ($41A0)
4333 21A441     ld hl, $41A4
4336 96         sub (hl)
4337 FA5243     jp m, $4352
433A 1602       ld d, $02
433C 47         ld b, a
433D 3AA141     ld a, ($41A1)
4340 21A541     ld hl, $41A5
4343 96         sub (hl)
4344 FA5943     jp m, $4359
4347 1E02       ld e, $02
4349 4F         ld c, a
434A 79         ld a, c
434B B8         cp b
434C FA6043     jp m, $4360
434F F26943     jp p, $4369
4352 1601       ld d, $01
4354 ED44       neg
4356 C33C43     jp $433C
4359 1E01       ld e, $01
435B ED44       neg
435D C34943     jp $4349
4360 7A         ld a, d
4361 FE01       cp $01
4363 CA8843     jp z, $4388
4366 C27243     jp nz, $4372
4369 7B         ld a, e
436A FE01       cp $01
436C CA9E43     jp z, $439E
436F C2B443     jp nz, $43B4
4372 CD7744     call $4477
4375 FE80       cp $80
4377 C0         ret nz
4378 CDC044     call $44C0
437B FE80       cp $80
437D C0         ret nz
437E CDE844     call $44E8
4381 FE80       cp $80
4383 C0         ret nz
4384 CD9A44     call $449A
4387 C9         ret
4388 CD9A44     call $449A
438B FE80       cp $80
438D C0         ret nz
438E CDC044     call $44C0
4391 FE80       cp $80
4393 C0         ret nz
4394 CDE844     call $44E8
4397 FE80       cp $80
4399 C0         ret nz
439A CD7744     call $4477
439D C9         ret
439E CDE844     call $44E8
43A1 FE80       cp $80
43A3 C0         ret nz
43A4 CD9A44     call $449A
43A7 FE80       cp $80
43A9 C0         ret nz
43AA CD7744     call $4477
43AD FE80       cp $80
43AF C0         ret nz
43B0 CDC044     call $44C0
43B3 C9         ret
43B4 CDC044     call $44C0
43B7 FE80       cp $80
43B9 C0         ret nz
43BA CD7744     call $4477
43BD FE80       cp $80
43BF C0         ret nz
43C0 CD9A44     call $449A
43C3 FE80       cp $80
43C5 C0         ret nz
43C6 CDE844     call $44E8
43C9 C9         ret
43CA 00         nop
43CB 00         nop
43CC 00         nop
43CD 00         nop
43CE 00         nop
43CF 00         nop
43D0 00         nop
43D1 00         nop
43D2 00         nop
43D3 00         nop
43D4 00         nop
43D5 00         nop
43D6 00         nop
43D7 00         nop
43D8 00         nop
43D9 00         nop
43DA 00         nop
43DB 00         nop
43DC 00         nop
43DD 00         nop
43DE 00         nop
43DF 00         nop
43E0 00         nop
43E1 00         nop
43E2 00         nop
43E3 00         nop
43E4 00         nop
43E5 00         nop
43E6 00         nop
43E7 00         nop
43E8 00         nop
43E9 00         nop
43EA 00         nop
43EB 00         nop
43EC 00         nop
43ED 00         nop
43EE 00         nop
43EF 00         nop
43F0 00         nop
43F1 00         nop
43F2 00         nop
43F3 00         nop
43F4 00         nop
43F5 00         nop
43F6 00         nop
43F7 00         nop
43F8 23         inc hl
43F9 3A2540     ld a, ($4025)
43FC 00         nop
43FD 3A2640     ld a, ($4026)
4400 47         ld b, a
4401 3A2540     ld a, ($4025)
4404 B9         cp c
4405 C20D44     jp nz, $440D
4408 3A2640     ld a, ($4026)
440B B8         cp b
440C C8         ret z
440D 3A2640     ld a, ($4026)
4410 47         ld b, a
4411 3A2540     ld a, ($4025)
4414 4F         ld c, a
4415 FEFF       cp $FF
4417 C8         ret z
4418 1EFF       ld e, $FF
441A 1600       ld d, $00
441C 78         ld a, b
441D 0F         rrca
441E 47         ld b, a
441F 7B         ld a, e
4420 9F         sbc a, a
4421 F626       or $26
4423 2E05       ld l, $05
4425 95         sub l
4426 85         add a, l
4427 37         scf
4428 5F         ld e, a
4429 79         ld a, c
442A 1F         rra
442B 4F         ld c, a
442C 7B         ld a, e
442D 38F7       jr c, $4426
442F 48         ld c, b
4430 2D         dec l
4431 2E01       ld l, $01
4433 20F1       jr nz, $4426
4435 217D00     ld hl, $007D
4438 5F         ld e, a
4439 19         add hl, de
443A 7E         ld a, (hl)
443B 32F843     ld ($43F8), a
443E FE21       cp $21
4440 CC0E42     call z, $420E
4443 FE22       cp $22
4445 CC6342     call z, $4263
4448 FE23       cp $23
444A CC3642     call z, $4236
444D FE24       cp $24
444F CCE641     call z, $41E6
4452 C9         ret
4453 F9         ld sp, hl
4454 43         ld b, e
4455 00         nop
4456 00         nop
4457 00         nop
4458 00         nop
4459 00         nop
445A 00         nop
445B 00         nop
445C 21A541     ld hl, $41A5
445F 46         ld b, (hl)
4460 21A441     ld hl, $41A4
4463 4E         ld c, (hl)
4464 3E08       ld a, $08
4466 CD8840     call $4088
4469 C9         ret
446A 7A         ld a, d
446B 44         ld b, h
446C C9         ret
446D A2         and d
446E 44         ld b, h
446F C9         ret
4470 00         nop
4471 00         nop
4472 00         nop
4473 00         nop
4474 00         nop
4475 00         nop
4476 00         nop
4477 CD5C44     call $445C
447A 112100     ld de, $0021
447D ED5A       adc hl, de
447F 7E         ld a, (hl)
4480 CB7F       bit 7, a
4482 C0         ret nz
4483 FE34       cp $34
4485 CC8845     call z, $4588
4488 FE08       cp $08
448A C8         ret z
448B CD8840     call $4088
448E 0C         inc c
448F 3E08       ld a, $08
4491 CD8840     call $4088
4494 79         ld a, c
4495 32A441     ld ($41A4), a
4498 C9         ret
4499 00         nop
449A CD5C44     call $445C
449D 112100     ld de, $0021
44A0 ED52       sbc hl, de
44A2 7E         ld a, (hl)
44A3 CB7F       bit 7, a
44A5 C0         ret nz
44A6 FE34       cp $34
44A8 CC8845     call z, $4588
44AB FE08       cp $08
44AD C8         ret z
44AE CD8840     call $4088
44B1 0D         dec c
44B2 3E08       ld a, $08
44B4 CD8840     call $4088
44B7 79         ld a, c
44B8 32A441     ld ($41A4), a
44BB C9         ret
44BC 41         ld b, c
44BD C9         ret
44BE 9F         sbc a, a
44BF 44         ld b, h
44C0 CD5C44     call $445C
44C3 23         inc hl
44C4 7E         ld a, (hl)
44C5 CB7F       bit 7, a
44C7 C0         ret nz
44C8 FE34       cp $34
44CA CC8845     call z, $4588
44CD FE08       cp $08
44CF C8         ret z
44D0 CD8840     call $4088
44D3 04         inc b
44D4 3E08       ld a, $08
44D6 CD8840     call $4088
44D9 78         ld a, b
44DA 32A541     ld ($41A5), a
44DD C9         ret
44DE 00         nop
44DF 00         nop
44E0 00         nop
44E1 00         nop
44E2 00         nop
44E3 00         nop
44E4 00         nop
44E5 00         nop
44E6 00         nop
44E7 00         nop
44E8 CD5C44     call $445C
44EB 2B         dec hl
44EC 7E         ld a, (hl)
44ED CB7F       bit 7, a
44EF C0         ret nz
44F0 FE34       cp $34
44F2 CC8845     call z, $4588
44F5 FE08       cp $08
44F7 C8         ret z
44F8 CD8840     call $4088
44FB 05         dec b
44FC 3E08       ld a, $08
44FE CD8840     call $4088
4501 78         ld a, b
4502 32A541     ld ($41A5), a
4505 C9         ret
4506 00         nop
4507 00         nop
4508 00         nop
4509 00         nop
450A 00         nop
450B 00         nop
450C 00         nop
450D 00         nop
450E 00         nop
450F 00         nop
4510 00         nop
4511 00         nop
4512 00         nop
4513 00         nop
4514 00         nop
4515 00         nop
4516 00         nop
4517 00         nop
4518 00         nop
4519 00         nop
451A 00         nop
451B 00         nop
451C 00         nop
451D 00         nop
451E 00         nop
451F 00         nop
4520 00         nop
4521 00         nop
4522 00         nop
4523 00         nop
4524 21A941     ld hl, $41A9
4527 46         ld b, (hl)
4528 21A841     ld hl, $41A8
452B 4E         ld c, (hl)
452C 3E08       ld a, $08
452E CD8840     call $4088
4531 C9         ret
4532 88         adc a, b
4533 CD2445     call $4524
4536 112100     ld de, $0021
4539 ED5A       adc hl, de
453B 7E         ld a, (hl)
453C CB7F       bit 7, a
453E C0         ret nz
453F FE34       cp $34
4541 CC8845     call z, $4588
4544 FE08       cp $08
4546 C8         ret z
4547 CD8840     call $4088
454A 0C         inc c
454B 3E08       ld a, $08
454D CD8840     call $4088
4550 79         ld a, c
4551 32A841     ld ($41A8), a
4554 C9         ret
4555 CDCD24     call $24CD
4558 45         ld b, l
4559 112100     ld de, $0021
455C ED52       sbc hl, de
455E 7E         ld a, (hl)
455F CB7F       bit 7, a
4561 C0         ret nz
4562 FE34       cp $34
4564 CC8845     call z, $4588
4567 FE08       cp $08
4569 C8         ret z
456A CD8840     call $4088
456D 0D         dec c
456E 3E08       ld a, $08
4570 CD8840     call $4088
4573 79         ld a, c
4574 32A841     ld ($41A8), a
4577 C9         ret
4578 00         nop
4579 00         nop
457A 00         nop
457B 00         nop
457C 00         nop
457D 00         nop
457E 00         nop
457F 00         nop
4580 00         nop
4581 00         nop
4582 00         nop
4583 00         nop
4584 00         nop
4585 00         nop
4586 00         nop
4587 00         nop
4588 3AAC41     ld a, ($41AC)
458B FE02       cp $02
458D CA9645     jp z, $4596
4590 3E3D       ld a, $3D
4592 32AE41     ld ($41AE), a
4595 C9         ret
4596 CD7C47     call $477C
4599 C9         ret
459A 00         nop
459B 00         nop
459C CD2445     call $4524
459F 23         inc hl
45A0 7E         ld a, (hl)
45A1 CB7F       bit 7, a
45A3 C0         ret nz
45A4 FE34       cp $34
45A6 CC8845     call z, $4588
45A9 FE08       cp $08
45AB C8         ret z
45AC CD8840     call $4088
45AF 04         inc b
45B0 3E08       ld a, $08
45B2 CD8840     call $4088
45B5 78         ld a, b
45B6 32A941     ld ($41A9), a
45B9 C9         ret
45BA CD2445     call $4524
45BD 2B         dec hl
45BE 7E         ld a, (hl)
45BF CB7F       bit 7, a
45C1 C0         ret nz
45C2 FE34       cp $34
45C4 CC8845     call z, $4588
45C7 FE08       cp $08
45C9 C8         ret z
45CA CD8840     call $4088
45CD 05         dec b
45CE 3E08       ld a, $08
45D0 CD8840     call $4088
45D3 78         ld a, b
45D4 32A941     ld ($41A9), a
45D7 C9         ret
45D8 00         nop
45D9 00         nop
45DA 00         nop
45DB 00         nop
45DC 00         nop
45DD 00         nop
45DE 00         nop
45DF 00         nop
45E0 00         nop
45E1 00         nop
45E2 00         nop
45E3 00         nop
45E4 00         nop
45E5 00         nop
45E6 00         nop
45E7 00         nop
45E8 00         nop
45E9 00         nop
45EA 00         nop
45EB 00         nop
45EC 00         nop
45ED 00         nop
45EE 00         nop
45EF 00         nop
45F0 00         nop
45F1 00         nop
45F2 00         nop
45F3 00         nop
45F4 00         nop
45F5 00         nop
45F6 00         nop
45F7 00         nop
45F8 00         nop
45F9 00         nop
45FA 00         nop
45FB 00         nop
45FC 00         nop
45FD 00         nop
45FE 00         nop
45FF 00         nop
4600 00         nop
4601 00         nop
4602 00         nop
4603 00         nop
4604 00         nop
4605 00         nop
4606 00         nop
4607 00         nop
4608 00         nop
4609 00         nop
460A 00         nop
460B 00         nop
460C 00         nop
460D 00         nop
460E 00         nop
460F 00         nop
4610 00         nop
4611 00         nop
4612 00         nop
4613 00         nop
4614 00         nop
4615 00         nop
4616 00         nop
4617 00         nop
4618 00         nop
4619 00         nop
461A 00         nop
461B 00         nop
461C 00         nop
461D 00         nop
461E 00         nop
461F 00         nop
4620 00         nop
4621 00         nop
4622 00         nop
4623 00         nop
4624 00         nop
4625 00         nop
4626 00         nop
4627 00         nop
4628 00         nop
4629 00         nop
462A 00         nop
462B 00         nop
462C 00         nop
462D 00         nop
462E 00         nop
462F 00         nop
4630 00         nop
4631 00         nop
4632 00         nop
4633 00         nop
4634 00         nop
4635 00         nop
4636 00         nop
4637 00         nop
4638 00         nop
4639 00         nop
463A 00         nop
463B 00         nop
463C 00         nop
463D 00         nop
463E 00         nop
463F 00         nop
4640 00         nop
4641 00         nop
4642 00         nop
4643 00         nop
4644 00         nop
4645 00         nop
4646 00         nop
4647 00         nop
4648 00         nop
4649 00         nop
464A 00         nop
464B 00         nop
464C 00         nop
464D 00         nop
464E 00         nop
464F 00         nop
4650 CDF943     call $43F9
4653 3AAD41     ld a, ($41AD)
4656 FE02       cp $02
4658 C26F46     jp nz, $466F
465B 3E01       ld a, $01
465D 32AD41     ld ($41AD), a
4660 CD3043     call $4330
4663 CDB446     call $46B4
4666 3AAE41     ld a, ($41AE)
4669 FE3D       cp $3D
466B C8         ret z
466C C37446     jp $4674
466F C601       add a, $01
4671 32AD41     ld ($41AD), a
4674 0E05       ld c, $05			:C LEVEL_DELAY
									:; POKEd from basic 5 for fastest, 30 for slowest
4676 06FA       ld b, $FA
4678 3E01       ld a, $01
467A 05         dec b
467B 20FB       jr nz, $4678
467D 0D         dec c
467E 20F6       jr nz, $4676
4680 3AB041     ld a, ($41B0)
4683 FEFA       cp $FA
4685 C8         ret z
4686 2AAA41     ld hl, ($41AA)
4689 46         ld b, (hl)
468A 78         ld a, b
468B FE34       cp $34
468D CC4A47     call z, $474A
4690 C35046     jp $4650
4693 50         ld d, b
4694 46         ld b, (hl)
4695 00         nop
4696 00         nop
4697 00         nop
4698 00         nop
4699 00         nop
469A 00         nop
469B 00         nop
469C 00         nop
469D 00         nop
469E 00         nop
469F 00         nop
46A0 00         nop
46A1 00         nop
46A2 00         nop
46A3 00         nop
46A4 00         nop
46A5 00         nop
46A6 00         nop
46A7 00         nop
46A8 00         nop
46A9 00         nop
46AA 00         nop
46AB 00         nop
46AC 00         nop
46AD 00         nop
46AE 00         nop
46AF 00         nop
46B0 00         nop
46B1 00         nop
46B2 00         nop
46B3 00         nop
46B4 3AA041     ld a, ($41A0)
46B7 21A841     ld hl, $41A8
46BA 96         sub (hl)
46BB FAD646     jp m, $46D6
46BE 1602       ld d, $02
46C0 47         ld b, a
46C1 3AA141     ld a, ($41A1)
46C4 21A941     ld hl, $41A9
46C7 96         sub (hl)
46C8 FADD46     jp m, $46DD
46CB 1E02       ld e, $02
46CD 4F         ld c, a
46CE 79         ld a, c
46CF B8         cp b
46D0 FAE446     jp m, $46E4
46D3 F2ED46     jp p, $46ED
46D6 1601       ld d, $01
46D8 ED44       neg
46DA C3C046     jp $46C0
46DD 1E01       ld e, $01
46DF ED44       neg
46E1 C3CD46     jp $46CD
46E4 7A         ld a, d
46E5 FE01       cp $01
46E7 CA0C47     jp z, $470C
46EA C2F646     jp nz, $46F6
46ED 7B         ld a, e
46EE FE01       cp $01
46F0 CA1547     jp z, $4715
46F3 C21E47     jp nz, $471E
46F6 CD3345     call $4533
46F9 FE80       cp $80
46FB C0         ret nz
46FC CDBA45     call $45BA
46FF FE80       cp $80
4701 C0         ret nz
4702 CD9C45     call $459C
4705 FE80       cp $80
4707 C0         ret nz
4708 CD5645     call $4556
470B C9         ret
470C CD5645     call $4556
470F FE80       cp $80
4711 C0         ret nz
4712 00         nop
4713 00         nop
4714 00         nop
4715 CDBA45     call $45BA
4718 FE80       cp $80
471A C0         ret nz
471B C3F646     jp $46F6
471E CD9C45     call $459C
4721 FE80       cp $80
4723 C0         ret nz
4724 C3F646     jp $46F6
4727 00         nop
4728 00         nop
4729 00         nop
472A 00         nop
472B 00         nop
472C 21A241     ld hl, $41A2
472F 46         ld b, (hl)
4730 21A341     ld hl, $41A3
4733 4E         ld c, (hl)
4734 3E97       ld a, $97
4736 CD8840     call $4088
4739 22AA41     ld ($41AA), hl
473C C9         ret
473D 00         nop
473E 00         nop
473F 00         nop
4740 00         nop
4741 00         nop
4742 00         nop
4743 00         nop
4744 00         nop
4745 00         nop
4746 00         nop
4747 00         nop
4748 00         nop
4749 00         nop
474A 3AAC41     ld a, ($41AC)
474D C601       add a, $01
474F 32AC41     ld ($41AC), a
4752 0610       ld b, $10
4754 0E09       ld c, $09
4756 3E00       ld a, $00
4758 CD8840     call $4088
475B 3E10       ld a, $10
475D 32A141     ld ($41A1), a
4760 3E0F       ld a, $0F
4762 32A041     ld ($41A0), a
4765 3E34       ld a, $34
4767 CDD441     call $41D4
476A C9         ret
476B 00         nop
476C 00         nop
476D 00         nop
476E 00         nop
476F 00         nop
4770 00         nop
4771 00         nop
4772 00         nop
4773 00         nop
4774 00         nop
4775 00         nop
4776 00         nop
4777 00         nop
4778 00         nop
4779 00         nop
477A 00         nop
477B 00         nop
477C 3D         dec a
477D 32AC41     ld ($41AC), a
4780 3E10       ld a, $10
4782 32A141     ld ($41A1), a
4785 3E0F       ld a, $0F
4787 32A041     ld ($41A0), a
478A 3E34       ld a, $34
478C C5         push bc
478D D5         push de
478E E5         push hl
478F CDD441     call $41D4
4792 E1         pop hl
4793 D1         pop de
4794 C1         pop bc
4795 3E00       ld a, $00
4797 C9         ret
4798 00         nop
4799 00         nop
479A 00         nop
479B 00         nop
479C 00         nop
479D 00         nop
479E 00         nop
479F 00         nop
47A0 00         nop
47A1 00         nop
47A2 00         nop
47A3 00         nop
47A4 00         nop
47A5 00         nop
47A6 00         nop
47A7 00         nop
47A8 00         nop
47A9 00         nop				:B BANNER_CHAR
47AA 07         rlca			:B BANNER_COL
47AB 00         nop
47AC 00         nop
47AD 00         nop
47AE-47BD 00870483028506810186058203840780	:B banner_bits_lu
47BE 00         nop
47BF 00         nop
47C0 00         nop
47C1 00         nop
47C2 2A0C40     ld hl, ($400C)		:C DISPLAY_BANNER
									:#;--------------------------------------------------------------------------------
									:#; Display one character in big format
									:#;--------------------------------------------------------------------------------
									:; address of screen
47C5 23         inc hl
47C6 01F101     ld bc, $01F1		:C SCR_OFFSET
									:; input offset POKEd from BASIC
47C9 3AAA47     ld a, ($47AA)
47CC FE01       cp $01
47CE C2D647     jp nz, $47D6		:; skip if column not 1
47D1 ED4A       adc hl, bc			:; for column 1 add input offset
47D3 220E40     ld ($400E), hl
47D6 21A947     ld hl, $47A9		:C skip_setting_df_cc
									:; HL points at char to print
47D9 7E         ld a, (hl)			:; A has char to print
47DA A7         and a
47DB 17         rla
47DC 17         rla					:; multiply by 4
47DD D8         ret c				:; exit if bit 6 was 1
47DE 17         rla					:; multiply by 8
47DF 1600       ld d, $00
47E1 CB12       rl d				:; carry into D
47E3 5F         ld e, a				:; DE=char*8
47E4 21001E     ld hl, $1E00		:; base of CHARS table
47E7 19         add hl, de			:; HL=address of char bitmap
47E8 0E04       ld c, $04			:; 4 rows
47EA 0604       ld b, $04			:C banner_row
									:; 4 columns
47EC 56         ld d, (hl)			:; get bitmap of two rows into DE
47ED 23         inc hl
47EE 5E         ld e, (hl)
47EF 23         inc hl
47F0 E5         push hl				:; save char table pointer
47F1 AF         xor a				:C banner_col
									:; do some magic
47F2 CB12       rl d
47F4 17         rla
47F5 CB12       rl d
47F7 17         rla
47F8 CB13       rl e
47FA 17         rla
47FB CB13       rl e
47FD 17         rla
47FE 21AE47     ld hl, $47AE		:; lookup table of bitmaps
4801 85         add a, l
4802 6F         ld l, a				:; HL points to correct bitmap
4803 7E         ld a, (hl)			:; A has the bitmap
4804 2A0E40     ld hl, ($400E)		:; HL has the print position
4807 77         ld (hl), a			:; print the character
4808 23         inc hl				:; advance print position
4809 220E40     ld ($400E), hl		:; and store it
480C 10E3       djnz $47F1			:; next column
480E D5         push de				:; save bitmap
480F 111D00     ld de, $001D		:; offset to next row
4812 19         add hl, de			:; add to DF_CC
4813 220E40     ld ($400E), hl		:; and store
4816 D1         pop de				:; restore bitmap
4817 E1         pop hl				:; restore chars pointer
4818 0D         dec c
4819 20CF       jr nz, $47EA		:; next row
481B 1180FF     ld de, $FF80		:; offset to next character
481E 2A0E40     ld hl, ($400E)		:; add to DF_CC
4821 19         add hl, de
4822 220E40     ld ($400E), hl		:; and store it
4825 7E         ld a, (hl)			:; read screen char; Note: not needed
4826 C9         ret
4827 00         nop
4828 00         nop
4829 00         nop
482A 00         nop
482B 00         nop
482C 00         nop
482D 00         nop
482E 00         nop
482F 00         nop
4830 00         nop
4831 00         nop
4832 00         nop
4833 00         nop
4834 00         nop
4835 00         nop
4836 00         nop
4837 00         nop
4838 00         nop
4839 00         nop
483A 00         nop
483B 00         nop
483C 00         nop
483D 00         nop
483E 00         nop
483F 00         nop
4840 00         nop
4841 00         nop
4842 00         nop
4843 00         nop
4844 00         nop
4845 00         nop
4846 00         nop
4847 00         nop
4848 00         nop
4849 00         nop
484A 00         nop
484B 00         nop
484C 00         nop
484D 00         nop
484E 00         nop
484F 00         nop
4850 00         nop
4851 00         nop
4852 00         nop
4853 00         nop
4854 00         nop
4855 00         nop
4856 00         nop
4857 00         nop
4858 00         nop
4859 00         nop
485A 00         nop
485B 00         nop
485C 00         nop
485D 00         nop
485E 00         nop
485F 00         nop
4860 00         nop
4861 00         nop
4862 00         nop
4863 00         nop
4864 00         nop
4865 00         nop
4866 00         nop
4867 00         nop
4868 00         nop
4869 00         nop
486A 00         nop
486B 00         nop
486C 00         nop
486D 00         nop
486E 00         nop
486F 00         nop
4870 00         nop
4871 00         nop
4872 00         nop
4873 00         nop
4874 00         nop
4875 00         nop
4876 00         nop
4877 00         nop
4878 00         nop
4879 00         nop
487A 00         nop
487B 00         nop
487C 00         nop
487D 00         nop
487E 00         nop
487F 00         nop
4880 00         nop
4881 00         nop
4882 00         nop
4883 00         nop
4884 00         nop
4885 00         nop
4886 00         nop
4887 00         nop
4888 00         nop
4889 00         nop
488A 00         nop
488B 00         nop
488C 00         nop
488D 00         nop
488E 00         nop
488F 00         nop
4890 00         nop
4891 00         nop
4892 00         nop
4893 00         nop
4894 00         nop
4895 00         nop
4896 00         nop
4897 00         nop
4898 00         nop
4899 00         nop
489A 00         nop
489B 00         nop
489C 00         nop
489D 00         nop
489E 00         nop
489F 00         nop
48A0 00         nop
48A1 00         nop
48A2 00         nop
48A3 00         nop
48A4 00         nop
48A5 00         nop
48A6 00         nop
48A7 00         nop
48A8 00         nop
48A9 00         nop
48AA 00         nop
48AB 00         nop
48AC 00         nop
48AD 00         nop
48AE 00         nop
48AF 00         nop
48B0 00         nop
48B1 00         nop
48B2 00         nop
48B3 00         nop
48B4 00         nop
48B5 00         nop
48B6 00         nop
48B7 00         nop
48B8 00         nop
48B9 00         nop
48BA 00         nop
48BB 00         nop
48BC 00         nop
48BD 00         nop
48BE 00         nop
48BF 00         nop
48C0 00         nop
48C1 00         nop
48C2 00         nop
48C3 00         nop
48C4 00         nop
48C5 00         nop
48C6 00         nop
48C7 00         nop
48C8 00         nop
48C9 00         nop
48CA 00         nop
48CB 00         nop
48CC 00         nop
48CD 00         nop
48CE 00         nop
48CF 00         nop
48D0 00         nop
48D1 00         nop
48D2 00         nop
48D3 00         nop
48D4 00         nop
48D5 00         nop
48D6 00         nop
48D7 00         nop
48D8 00         nop
48D9 00         nop
48DA 00         nop
48DB 00         nop
48DC 00         nop
48DD 00         nop
48DE 00         nop
48DF 00         nop
48E0 00         nop
48E1 00         nop
48E2 00         nop
48E3 00         nop
48E4 00         nop
48E5 00         nop
48E6 00         nop
48E7 00         nop
48E8 00         nop
48E9 00         nop
48EA 00         nop
48EB 00         nop
48EC 00         nop
48ED 00         nop
48EE 00         nop
48EF 00         nop
48F0 00         nop
48F1 00         nop
48F2 00         nop
48F3 00         nop
48F4 00         nop
48F5 00         nop
48F6 00         nop
48F7 00         nop
48F8 00         nop
48F9 00         nop
48FA 00         nop
48FB 00         nop
48FC 00         nop
48FD 00         nop
48FE 00         nop
48FF 00         nop
4900 00         nop
4901 00         nop
4902 00         nop
4903 00         nop
4904 00         nop
4905 00         nop
4906 00         nop
4907 00         nop
4908 00         nop
4909 00         nop
490A 00         nop
490B 00         nop
490C 00         nop
490D 00         nop
490E 00         nop
490F 00         nop
4910 00         nop
4911 00         nop
4912 00         nop
4913 00         nop
4914 00         nop
4915 00         nop
4916 00         nop
4917 00         nop
4918 00         nop
4919 00         nop
491A 00         nop
491B 00         nop
491C 00         nop
491D 00         nop
491E 00         nop
491F 00         nop
4920 00         nop
4921 00         nop
4922 00         nop
4923 00         nop
4924 00         nop
4925 00         nop
4926 00         nop
4927 00         nop
4928 00         nop
4929 00         nop
492A 00         nop
492B 00         nop
492C 00         nop
492D 00         nop
492E 00         nop
492F 00         nop
4930 00         nop
4931 00         nop
4932 00         nop
4933 00         nop
4934 00         nop
4935 00         nop
4936 00         nop
4937 00         nop
4938 00         nop
4939 00         nop
493A 00         nop
493B 00         nop
493C 00         nop
493D 00         nop
493E 00         nop
493F 00         nop
4940 00         nop
4941 00         nop
4942 00         nop
4943 00         nop
4944 00         nop
4945 00         nop
4946 00         nop
4947 00         nop
4948 00         nop
4949 00         nop
494A 00         nop
494B 00         nop
494C 00         nop
494D 00         nop
494E 00         nop
494F 00         nop
4950 00         nop
4951 00         nop
4952 00         nop
4953 00         nop
4954 00         nop
4955 00         nop
4956 00         nop
4957 00         nop
4958 00         nop
4959 00         nop
495A 00         nop
495B 00         nop
495C 00         nop
495D 00         nop
495E 00         nop
495F 00         nop
4960 00         nop
4961 00         nop
4962 00         nop
4963 00         nop
4964 00         nop
4965 00         nop
4966 00         nop
4967 00         nop
4968 00         nop
4969 00         nop
496A 00         nop
496B 00         nop
496C 00         nop
496D 00         nop
496E 00         nop
496F 00         nop
4970 00         nop
4971 00         nop
4972 00         nop
4973 00         nop
4974 00         nop
4975 00         nop
4976 00         nop
4977 00         nop
4978 00         nop
4979 00         nop
497A 00         nop
497B 00         nop
497C 00         nop
497D 00         nop
497E 00         nop
497F 00         nop
4980 00         nop
4981 00         nop
4982 00         nop
4983 00         nop
4984 00         nop
4985 00         nop
4986 00         nop
4987 00         nop
4988 00         nop
4989 00         nop
498A 00         nop
498B 00         nop
498C 00         nop
498D 00         nop
498E 00         nop
498F 00         nop
4990 00         nop
4991 00         nop
4992 00         nop
4993 00         nop
4994 00         nop
4995 00         nop
4996 00         nop
4997 00         nop
4998 00         nop
4999 00         nop
499A 00         nop
499B 00         nop
499C 00         nop
499D 00         nop
499E 00         nop
499F 00         nop
49A0 00         nop
49A1 00         nop
49A2 00         nop
49A3 00         nop
49A4 00         nop
49A5 00         nop
49A6 00         nop
49A7 00         nop
49A8 00         nop
49A9 00         nop
49AA 00         nop
49AB 00         nop
49AC 00         nop
49AD 00         nop
49AE 00         nop
49AF 00         nop
49B0 00         nop
49B1 00         nop
49B2 00         nop
49B3 00         nop
49B4 00         nop
49B5 00         nop
49B6 00         nop
49B7 00         nop
49B8 00         nop
49B9 00         nop
49BA 00         nop
49BB 00         nop
49BC 00         nop
49BD 00         nop
49BE 00         nop
49BF 00         nop
49C0 00         nop
49C1 00         nop
49C2 00         nop
49C3 00         nop
49C4 00         nop
49C5 00         nop
49C6 00         nop
49C7 00         nop
49C8 00         nop
49C9 00         nop
49CA 00         nop
49CB 00         nop
49CC 00         nop
49CD 00         nop
49CE 00         nop
49CF 00         nop
49D0 00         nop
49D1 00         nop
49D2 00         nop
49D3 00         nop
49D4 00         nop
49D5 00         nop
49D6 00         nop
49D7 00         nop
49D8 00         nop
49D9 00         nop
49DA 00         nop
49DB 00         nop
49DC 00         nop
49DD 00         nop
49DE 00         nop
49DF 00         nop
49E0 00         nop
49E1 00         nop
49E2 00         nop
49E3 00         nop
49E4 00         nop
49E5 00         nop
49E6 00         nop
49E7 00         nop
49E8 00         nop
49E9 00         nop
49EA 00         nop
49EB 00         nop
49EC 00         nop
49ED 00         nop
49EE 00         nop
49EF 00         nop
49F0 00         nop
49F1 00         nop
49F2 00         nop
49F3 00         nop
49F4 00         nop
49F5 00         nop
49F6 00         nop
49F7 00         nop
49F8 00         nop
49F9 00         nop
49FA 00         nop
49FB 00         nop
49FC 00         nop
49FD 00         nop
49FE 00         nop
49FF 00         nop
4A00 00         nop
4A01 00         nop
4A02 00         nop
4A03 00         nop
4A04 00         nop
4A05 00         nop
4A06 00         nop
4A07 00         nop
4A08 00         nop
4A09 00         nop
4A0A 00         nop
4A0B 00         nop
4A0C 00         nop
4A0D 00         nop
4A0E 00         nop
4A0F 00         nop
4A10 00         nop
4A11 00         nop
4A12 00         nop
4A13 00         nop
4A14 00         nop
4A15 00         nop
4A16 00         nop
4A17 00         nop
4A18 00         nop
4A19 00         nop
4A1A 00         nop
4A1B 00         nop
4A1C 00         nop
4A1D 00         nop
4A1E 00         nop
4A1F 00         nop
4A20 00         nop
4A21 00         nop
4A22 00         nop
4A23 00         nop
4A24 00         nop
4A25 00         nop
4A26 00         nop
4A27 00         nop
4A28 00         nop
4A29 00         nop
4A2A 00         nop
4A2B 00         nop
4A2C 00         nop
4A2D 00         nop
4A2E 00         nop
4A2F 00         nop
4A30 00         nop
4A31 00         nop
4A32 00         nop
4A33 00         nop
4A34 00         nop
4A35 00         nop
4A36 00         nop
4A37 00         nop
4A38 00         nop
4A39 00         nop
4A3A 00         nop
4A3B 00         nop
4A3C 00         nop
4A3D 00         nop
4A3E 00         nop
4A3F 00         nop
4A40 00         nop
4A41 00         nop
4A42 00         nop
4A43 00         nop
4A44 00         nop
4A45 00         nop
4A46 00         nop
4A47 00         nop
4A48 00         nop
4A49 00         nop
4A4A 00         nop
4A4B 00         nop
4A4C 00         nop
4A4D 00         nop
4A4E 00         nop
4A4F 00         nop
4A50 00         nop
4A51 00         nop
4A52 00         nop
4A53 00         nop
4A54 00         nop
4A55 00         nop
4A56 00         nop
4A57 00         nop
4A58 00         nop
4A59 00         nop
4A5A 00         nop
4A5B 00         nop
4A5C 00         nop
4A5D 00         nop
4A5E 00         nop
4A5F 00         nop
4A60 00         nop
4A61 00         nop
4A62 00         nop
4A63 00         nop
4A64 00         nop
4A65 00         nop
4A66 00         nop
4A67 00         nop
4A68 00         nop
4A69 00         nop
4A6A 00         nop
4A6B 00         nop
4A6C 00         nop
4A6D 00         nop
4A6E 00         nop
4A6F 00         nop
4A70 00         nop
4A71 00         nop
4A72 00         nop
4A73 00         nop
4A74 00         nop
4A75 00         nop
4A76 00         nop
4A77 00         nop
4A78 00         nop
4A79 00         nop
4A7A 00         nop
4A7B 00         nop
4A7C 00         nop
4A7D 00         nop
4A7E 00         nop
4A7F 00         nop
4A80 00         nop
4A81 00         nop
4A82 00         nop
4A83 00         nop
4A84 00         nop
4A85 00         nop
4A86 00         nop
4A87 00         nop
4A88 00         nop
4A89 00         nop
4A8A 00         nop
4A8B 00         nop
4A8C 00         nop
4A8D 00         nop
4A8E 00         nop
4A8F 00         nop
4A90 00         nop
4A91 00         nop
4A92 00         nop
4A93 00         nop
4A94 00         nop
4A95 00         nop
4A96 00         nop
4A97 00         nop
4A98 00         nop
4A99 00         nop
4A9A 00         nop
4A9B 00         nop
4A9C 00         nop
4A9D 00         nop
4A9E 00         nop
4A9F 00         nop
4AA0 00         nop
4AA1 00         nop
4AA2 00         nop
4AA3 00         nop
4AA4 00         nop
4AA5 00         nop
4AA6 00         nop
4AA7 00         nop
4AA8 00         nop
4AA9 00         nop
4AAA 00         nop
4AAB 00         nop
4AAC 00         nop
4AAD 00         nop
4AAE 00         nop
4AAF 00         nop
4AB0 00         nop
4AB1 00         nop
4AB2 00         nop
4AB3 00         nop
4AB4 00         nop
4AB5 00         nop
4AB6 00         nop
4AB7 00         nop
4AB8 00         nop
4AB9 00         nop
4ABA 00         nop
4ABB 00         nop
4ABC 00         nop
4ABD 00         nop
4ABE 00         nop
4ABF 00         nop
4AC0 00         nop
4AC1 00         nop
4AC2 00         nop
4AC3 00         nop
4AC4 00         nop
4AC5 00         nop
4AC6 00         nop
4AC7 00         nop
4AC8 00         nop
4AC9 00         nop
4ACA 00         nop
4ACB 00         nop
4ACC 00         nop
4ACD 00         nop
4ACE 00         nop
4ACF 00         nop
4AD0 00         nop
4AD1 00         nop
4AD2 00         nop
4AD3 00         nop
4AD4 00         nop
4AD5 00         nop
4AD6 00         nop
4AD7 00         nop
4AD8 00         nop
4AD9 00         nop
4ADA 00         nop
4ADB 00         nop
4ADC 00         nop
4ADD 00         nop
4ADE 00         nop
4ADF 00         nop
4AE0 00         nop
4AE1 00         nop
4AE2 00         nop
4AE3 00         nop
4AE4 00         nop
4AE5 00         nop
4AE6 00         nop
4AE7 00         nop
4AE8 00         nop
4AE9 00         nop
4AEA 00         nop
4AEB 00         nop
4AEC 00         nop
4AED 00         nop
4AEE 00         nop
4AEF 00         nop
4AF0 00         nop
4AF1 00         nop
4AF2 00         nop
4AF3 00         nop
4AF4 00         nop
4AF5 00         nop
4AF6 00         nop
4AF7 00         nop
4AF8 00         nop
4AF9 00         nop
4AFA 00         nop
4AFB 00         nop
4AFC 00         nop
4AFD 00         nop
4AFE 00         nop
4AFF 00         nop
4B00 00         nop
4B01 00         nop
4B02 00         nop
4B03 00         nop
4B04 00         nop
4B05 00         nop
4B06 00         nop
4B07 00         nop
4B08 00         nop
4B09 00         nop
4B0A 00         nop
4B0B 00         nop
4B0C 00         nop
4B0D 00         nop
4B0E 00         nop
4B0F 00         nop
4B10 00         nop
4B11 00         nop
4B12 00         nop
4B13 00         nop
4B14 00         nop
4B15 00         nop
4B16 00         nop
4B17 00         nop
4B18 00         nop
4B19 00         nop
4B1A 00         nop
4B1B 00         nop
4B1C 00         nop
4B1D 00         nop
4B1E 00         nop
4B1F 00         nop
4B20 00         nop
4B21 00         nop
4B22 00         nop
4B23 00         nop
4B24 00         nop
4B25 00         nop
4B26 00         nop
4B27 00         nop
4B28 00         nop
4B29 00         nop
4B2A 00         nop
4B2B 00         nop
4B2C 00         nop
4B2D 00         nop
4B2E 00         nop
4B2F 00         nop
4B30 00         nop
4B31 00         nop
4B32 00         nop
4B33 00         nop
4B34 00         nop
4B35 00         nop
4B36 00         nop
4B37 00         nop
4B38 00         nop
4B39 00         nop
4B3A 00         nop
4B3B 00         nop
4B3C 00         nop
4B3D 00         nop
4B3E 00         nop
4B3F 00         nop
4B40 00         nop
4B41 00         nop
4B42 00         nop
4B43 00         nop
4B44 00         nop
4B45 00         nop
4B46 00         nop
4B47 00         nop
4B48 00         nop
4B49 00         nop
4B4A 00         nop
4B4B 00         nop
4B4C 00         nop
4B4D 00         nop
4B4E 00         nop
4B4F 00         nop
4B50 00         nop
4B51 00         nop
4B52 00         nop
4B53 00         nop
4B54 00         nop
4B55 00         nop
4B56 00         nop
4B57 00         nop
4B58 00         nop
4B59 00         nop
4B5A 00         nop
4B5B 00         nop
4B5C 00         nop
4B5D 00         nop
4B5E 00         nop
4B5F 00         nop
4B60 00         nop
4B61 00         nop
4B62 00         nop
4B63 00         nop
4B64 00         nop
4B65 00         nop
4B66 00         nop
4B67 00         nop
4B68 00         nop
4B69 00         nop
4B6A 00         nop
4B6B 00         nop
4B6C 00         nop
4B6D 00         nop
4B6E 00         nop
4B6F 00         nop
4B70 00         nop
4B71 00         nop
4B72 00         nop
4B73 00         nop
4B74 00         nop
4B75 00         nop
4B76 00         nop
4B77 00         nop
4B78 00         nop
4B79 00         nop
4B7A 00         nop
4B7B 00         nop
4B7C 00         nop
4B7D 00         nop
4B7E 00         nop
4B7F 00         nop
4B80 00         nop
4B81 00         nop
4B82 00         nop
4B83 00         nop
4B84 00         nop
4B85 00         nop
4B86 00         nop
4B87 00         nop
4B88 00         nop
4B89 00         nop
4B8A 00         nop
4B8B 00         nop
4B8C 00         nop
4B8D 00         nop
4B8E 00         nop
4B8F 00         nop
4B90 00         nop
4B91 00         nop
4B92 00         nop
4B93 00         nop
4B94 00         nop
4B95 00         nop
4B96 00         nop
4B97 00         nop
4B98 00         nop
4B99 00         nop
4B9A 00         nop
4B9B 00         nop
4B9C 00         nop
4B9D 00         nop
4B9E 00         nop
4B9F 00         nop
4BA0 00         nop
4BA1 00         nop
4BA2 00         nop
4BA3 00         nop
4BA4 00         nop
4BA5 00         nop
4BA6 00         nop
4BA7 00         nop
4BA8 00         nop
4BA9 00         nop
4BAA 00         nop
4BAB 00         nop
4BAC 00         nop
4BAD 00         nop
4BAE 00         nop
4BAF 00         nop
4BB0 00         nop
4BB1 00         nop
4BB2 00         nop
4BB3 00         nop
4BB4 00         nop
4BB5 00         nop
4BB6 00         nop
4BB7 00         nop
4BB8 00         nop
4BB9 00         nop
4BBA 00         nop
4BBB 00         nop
4BBC 00         nop
4BBD 00         nop
4BBE 00         nop
4BBF 00         nop
4BC0 00         nop
4BC1 00         nop
4BC2 00         nop
4BC3 00         nop
4BC4 00         nop
4BC5 00         nop
4BC6 00         nop
4BC7 00         nop
4BC8 00         nop
4BC9 00         nop
4BCA 00         nop
4BCB 00         nop
4BCC 00         nop
4BCD 00         nop
4BCE 00         nop
4BCF 00         nop
4BD0 00         nop
4BD1 00         nop
4BD2 00         nop
4BD3 00         nop
4BD4 00         nop
4BD5 00         nop
4BD6 00         nop
4BD7 00         nop
4BD8 00         nop
4BD9 00         nop
4BDA 00         nop
4BDB 00         nop
4BDC 00         nop
4BDD 00         nop
4BDE 00         nop
4BDF 00         nop
4BE0 00         nop
4BE1 00         nop
4BE2 00         nop
4BE3 00         nop
4BE4 00         nop
4BE5 00         nop
4BE6 00         nop
4BE7 00         nop
4BE8 00         nop
4BE9 00         nop
4BEA 00         nop
4BEB 00         nop
4BEC 00         nop
4BED 00         nop
4BEE 00         nop
4BEF 00         nop
4BF0 00         nop
4BF1 00         nop
4BF2 00         nop
4BF3 00         nop
4BF4 00         nop
4BF5 00         nop
4BF6 00         nop
4BF7 00         nop
4BF8 00         nop
4BF9 00         nop
4BFA 00         nop
4BFB 00         nop
4BFC 00         nop
4BFD 00         nop
4BFE 00         nop
4BFF 00         nop
4C00 00         nop
4C01 00         nop
4C02 00         nop
4C03 00         nop
4C04 00         nop
4C05 00         nop
4C06 00         nop
4C07 00         nop
4C08 00         nop
4C09 00         nop
4C0A 00         nop
4C0B 00         nop
4C0C 00         nop
4C0D 00         nop
4C0E 00         nop
4C0F 00         nop
4C10 00         nop
4C11 00         nop
4C12 00         nop
4C13 00         nop
4C14 00         nop
4C15 00         nop
4C16 00         nop
4C17 00         nop
4C18 00         nop
4C19 00         nop
4C1A 00         nop
4C1B 00         nop
4C1C 00         nop
4C1D 00         nop
4C1E 00         nop
4C1F 00         nop
4C20 00         nop
4C21 00         nop
4C22 00         nop
4C23 00         nop
4C24 00         nop
4C25 00         nop
4C26 00         nop
4C27 00         nop
4C28 00         nop
4C29 00         nop
4C2A 00         nop
4C2B 00         nop
4C2C 00         nop
4C2D 00         nop
4C2E 00         nop
4C2F 00         nop
4C30 00         nop
4C31 00         nop
4C32 00         nop
4C33 00         nop
4C34 00         nop
4C35 00         nop
4C36 00         nop
4C37 00         nop
4C38 00         nop
4C39 00         nop
4C3A 00         nop
4C3B 00         nop
4C3C 00         nop
4C3D 00         nop
4C3E 00         nop
4C3F 00         nop
4C40 00         nop
4C41 00         nop
4C42 00         nop
4C43 00         nop
4C44 00         nop
4C45 00         nop
4C46 00         nop
4C47 00         nop
4C48 00         nop
4C49 00         nop
4C4A 00         nop
4C4B 00         nop
4C4C 00         nop
4C4D 00         nop
4C4E 00         nop
4C4F 00         nop
4C50 00         nop
4C51 00         nop
4C52 00         nop
4C53 00         nop
4C54 00         nop
4C55 00         nop
4C56 00         nop
4C57 00         nop
4C58 00         nop
4C59 00         nop
4C5A 00         nop
4C5B 00         nop
4C5C 00         nop
4C5D 00         nop
4C5E 00         nop
4C5F 00         nop
4C60 00         nop
4C61 00         nop
4C62 00         nop
4C63 00         nop
4C64 00         nop
4C65 00         nop
4C66 00         nop
4C67 00         nop
4C68 00         nop
4C69 00         nop
4C6A 00         nop
4C6B 00         nop
4C6C 00         nop
4C6D 00         nop
4C6E 00         nop
4C6F 00         nop
4C70 00         nop
4C71 00         nop
4C72 00         nop
4C73 00         nop
4C74 00         nop
4C75 00         nop
4C76 00         nop
4C77 00         nop
4C78 00         nop
4C79 00         nop
4C7A 00         nop
4C7B 00         nop
4C7C 00         nop
4C7D 00         nop
4C7E 00         nop
4C7F 00         nop
4C80 00         nop
4C81 00         nop
4C82 00         nop
4C83 00         nop
4C84 00         nop
4C85 00         nop
4C86 00         nop
4C87 00         nop
4C88 00         nop
4C89 00         nop
4C8A 00         nop
4C8B 00         nop
4C8C 00         nop
4C8D 00         nop
4C8E 00         nop
4C8F 00         nop
4C90 00         nop
4C91 00         nop
4C92 00         nop
4C93 00         nop
4C94 00         nop
4C95 00         nop
4C96 00         nop
4C97 00         nop
4C98 00         nop
4C99 00         nop
4C9A 00         nop
4C9B 00         nop
4C9C 00         nop
4C9D 00         nop
4C9E 00         nop
4C9F 00         nop
4CA0 00         nop
4CA1 00         nop
4CA2 00         nop
4CA3 00         nop
4CA4 00         nop
4CA5 00         nop
4CA6 00         nop
4CA7 00         nop
4CA8 00         nop
4CA9 00         nop
4CAA 00         nop
4CAB 00         nop
4CAC 00         nop
4CAD 00         nop
4CAE 00         nop
4CAF 00         nop
4CB0 00         nop
4CB1 00         nop
4CB2 00         nop
4CB3 00         nop
4CB4 00         nop
4CB5 00         nop
4CB6 00         nop
4CB7 00         nop
4CB8 00         nop
4CB9 00         nop
4CBA 00         nop
4CBB 00         nop
4CBC 00         nop
4CBD 00         nop
4CBE 00         nop
4CBF 00         nop
4CC0 00         nop
4CC1 00         nop
4CC2 00         nop
4CC3 00         nop
4CC4 00         nop
4CC5 00         nop
4CC6 00         nop
4CC7 00         nop
4CC8 00         nop
4CC9 00         nop
4CCA 00         nop
4CCB 00         nop
4CCC 00         nop
4CCD 00         nop
4CCE 00         nop
4CCF 00         nop
4CD0 00         nop
4CD1 00         nop
4CD2 00         nop
4CD3 00         nop
4CD4 00         nop
4CD5 00         nop
4CD6 00         nop
4CD7 00         nop
4CD8 00         nop
4CD9 00         nop
4CDA 00         nop
4CDB 00         nop
4CDC 00         nop
4CDD 00         nop
4CDE 00         nop
4CDF 00         nop
4CE0 00         nop
4CE1 00         nop
4CE2 00         nop
4CE3 00         nop
4CE4 00         nop
4CE5 00         nop
4CE6 00         nop
4CE7 00         nop
4CE8 00         nop
4CE9 00         nop
4CEA 00         nop
4CEB 00         nop
4CEC 00         nop
4CED 00         nop
4CEE 00         nop
4CEF 00         nop
4CF0 00         nop
4CF1 00         nop
4CF2 00         nop
4CF3 00         nop
4CF4 00         nop
4CF5 00         nop
4CF6 00         nop
4CF7 00         nop
4CF8 00         nop
4CF9 00         nop
4CFA 00         nop
4CFB 00         nop
4CFC 00         nop
4CFD 00         nop
4CFE 00         nop
4CFF 00         nop
4D00 00         nop
4D01 00         nop
4D02 00         nop
4D03 00         nop
4D04 00         nop
4D05 00         nop
4D06 00         nop
4D07 00         nop
4D08 00         nop
4D09 00         nop
4D0A 00         nop
4D0B 00         nop
4D0C 00         nop
4D0D 00         nop
4D0E 00         nop
4D0F 00         nop
4D10 00         nop
4D11 00         nop
4D12 00         nop
4D13 00         nop
4D14 00         nop
4D15 00         nop
4D16 00         nop
4D17 00         nop
4D18 00         nop
4D19 00         nop
4D1A 00         nop
4D1B 00         nop
4D1C 00         nop
4D1D 00         nop
4D1E 00         nop
4D1F 00         nop
4D20 00         nop
4D21 00         nop
4D22 00         nop
4D23 00         nop
4D24 00         nop
4D25 00         nop
4D26 00         nop
4D27 00         nop
4D28 00         nop
4D29 00         nop
4D2A 00         nop
4D2B 00         nop
4D2C 00         nop
4D2D 00         nop
4D2E 00         nop
4D2F 00         nop
4D30 00         nop
4D31 00         nop
4D32 00         nop
4D33 00         nop
4D34 00         nop
4D35 00         nop
4D36 00         nop
4D37 00         nop
4D38 00         nop
4D39 00         nop
4D3A 00         nop
4D3B 00         nop
4D3C 00         nop
4D3D 00         nop
4D3E 00         nop
4D3F 00         nop
4D40 00         nop
4D41 00         nop
4D42 00         nop
4D43 00         nop
4D44 00         nop
4D45 00         nop
4D46 00         nop
4D47 00         nop
4D48 00         nop
4D49 00         nop
4D4A 00         nop
4D4B 00         nop
4D4C 00         nop
4D4D 00         nop
4D4E 00         nop
4D4F 00         nop
4D50 00         nop
4D51 00         nop
4D52 00         nop
4D53 00         nop
4D54 00         nop
4D55 00         nop
4D56 00         nop
4D57 00         nop
4D58 00         nop
4D59 00         nop
4D5A 00         nop
4D5B 00         nop
4D5C 00         nop
4D5D 00         nop
4D5E 00         nop
4D5F 00         nop
4D60 00         nop
4D61 00         nop
4D62 00         nop
4D63 00         nop
4D64 00         nop
4D65 00         nop
4D66 00         nop
4D67 00         nop
4D68 00         nop
4D69 00         nop
4D6A 00         nop
4D6B 00         nop
4D6C 00         nop
4D6D 00         nop
4D6E 00         nop
4D6F 00         nop
4D70 00         nop
4D71 00         nop
4D72 00         nop
4D73 00         nop
4D74 00         nop
4D75 00         nop
4D76 00         nop
4D77 00         nop
4D78 00         nop
4D79 00         nop
4D7A 00         nop
4D7B 00         nop
4D7C 00         nop
4D7D 00         nop
4D7E 00         nop
4D7F 00         nop
4D80 00         nop
4D81 00         nop
4D82 00         nop
4D83 00         nop
4D84 00         nop
4D85 00         nop
4D86 00         nop
4D87 00         nop
4D88 00         nop
4D89 00         nop
4D8A 00         nop
4D8B 00         nop
4D8C 00         nop
4D8D 00         nop
4D8E 00         nop
4D8F 00         nop
4D90 00         nop
4D91 00         nop
4D92 00         nop
4D93 00         nop
4D94 00         nop
4D95 00         nop
4D96 00         nop
4D97 00         nop
4D98 00         nop
4D99 00         nop
4D9A 00         nop
4D9B 00         nop
4D9C 00         nop
4D9D 00         nop
4D9E 00         nop
4D9F 00         nop
4DA0 00         nop
4DA1 00         nop
4DA2 00         nop
4DA3 00         nop
4DA4 00         nop
4DA5 00         nop
4DA6 00         nop
4DA7 00         nop
4DA8 00         nop
4DA9 00         nop
4DAA 00         nop
4DAB 00         nop
4DAC 00         nop
4DAD 00         nop
4DAE 00         nop
4DAF 00         nop
4DB0 00         nop
4DB1 00         nop
4DB2 00         nop
4DB3 00         nop
4DB4 00         nop
4DB5 00         nop
4DB6 00         nop
4DB7 00         nop
4DB8 00         nop
4DB9 00         nop
4DBA 00         nop
4DBB 00         nop
4DBC 00         nop
4DBD 00         nop
4DBE 00         nop
4DBF 00         nop
4DC0 00         nop
4DC1 00         nop
4DC2 00         nop
4DC3 00         nop
4DC4 00         nop
4DC5 00         nop
4DC6 00         nop
4DC7 00         nop
4DC8 00         nop
4DC9 00         nop
4DCA 00         nop
4DCB 00         nop
4DCC 00         nop
4DCD 00         nop
4DCE 00         nop
4DCF 00         nop
4DD0 00         nop
4DD1 00         nop
4DD2 00         nop
4DD3 00         nop
4DD4 00         nop
4DD5 00         nop
4DD6 00         nop
4DD7 00         nop
4DD8 00         nop
4DD9 00         nop
4DDA 00         nop
4DDB 00         nop
4DDC 00         nop
4DDD 00         nop
4DDE 00         nop
4DDF 00         nop
4DE0 00         nop
4DE1 00         nop
4DE2 00         nop
4DE3 00         nop
4DE4 00         nop
4DE5 00         nop
4DE6 00         nop
4DE7 00         nop
4DE8 00         nop
4DE9 00         nop
4DEA 00         nop
4DEB 00         nop
4DEC 00         nop
4DED 00         nop
4DEE 00         nop
4DEF 00         nop
4DF0 00         nop
4DF1 00         nop
4DF2 00         nop
4DF3 00         nop
4DF4 00         nop
4DF5 00         nop
4DF6 00         nop
4DF7 00         nop
4DF8 00         nop
4DF9 00         nop
4DFA 00         nop
4DFB 00         nop
4DFC 00         nop
4DFD 00         nop
4DFE 00         nop
4DFF 00         nop
4E00 00         nop
4E01 00         nop
4E02 00         nop
4E03 00         nop
4E04 00         nop
4E05 00         nop
4E06 00         nop
4E07 00         nop
4E08 00         nop
4E09 00         nop
4E0A 00         nop
4E0B 00         nop
4E0C 00         nop
4E0D 00         nop
4E0E 00         nop
4E0F 00         nop
4E10 00         nop
4E11 00         nop
4E12 00         nop
4E13 00         nop
4E14 00         nop
4E15 00         nop
4E16 00         nop
4E17 00         nop
4E18 00         nop
4E19 00         nop
4E1A 00         nop
4E1B 00         nop
4E1C 00         nop
4E1D 00         nop
4E1E 00         nop
4E1F 00         nop
4E20 00         nop
4E21 00         nop
4E22 00         nop
4E23 00         nop
4E24 00         nop
4E25 00         nop
4E26 00         nop
4E27 00         nop
4E28 00         nop
4E29 00         nop
4E2A 00         nop
4E2B 00         nop
4E2C 00         nop
4E2D 00         nop
4E2E 00         nop
4E2F 00         nop
4E30 00         nop
4E31 00         nop
4E32 00         nop
4E33 00         nop
4E34 00         nop
4E35 00         nop
4E36 00         nop
4E37 00         nop
4E38 00         nop
4E39 00         nop
4E3A 00         nop
4E3B 00         nop
4E3C 00         nop
4E3D 00         nop
4E3E 00         nop
4E3F 00         nop
4E40 00         nop
4E41 00         nop
4E42 00         nop
4E43 00         nop
4E44 00         nop
4E45 00         nop
4E46 00         nop
4E47 00         nop
4E48 00         nop
4E49 00         nop
4E4A 00         nop
4E4B 00         nop
4E4C 00         nop
4E4D 00         nop
4E4E 00         nop
4E4F 00         nop
4E50 00         nop
4E51 00         nop
4E52 00         nop
4E53 00         nop
4E54 00         nop
4E55 00         nop
4E56 00         nop
4E57 00         nop
4E58 00         nop
4E59 00         nop
4E5A 00         nop
4E5B 00         nop
4E5C 00         nop
4E5D 00         nop
4E5E 00         nop
4E5F 00         nop
4E60 00         nop
4E61 00         nop
4E62 00         nop
4E63 00         nop
4E64 00         nop
4E65 00         nop
4E66 00         nop
4E67 00         nop
4E68 00         nop
4E69 00         nop
4E6A 00         nop
4E6B 00         nop
4E6C 00         nop
4E6D 00         nop
4E6E 00         nop
4E6F 00         nop
4E70 00         nop
4E71 00         nop
4E72 00         nop
4E73 00         nop
4E74 00         nop
4E75 00         nop
4E76 00         nop
4E77 00         nop
4E78 00         nop
4E79 00         nop
4E7A 00         nop
4E7B 00         nop
4E7C 00         nop
4E7D 00         nop
4E7E 00         nop
4E7F 00         nop
4E80 00         nop
4E81 00         nop
4E82 00         nop
4E83 00         nop
4E84 00         nop
4E85 00         nop
4E86 00         nop
4E87 00         nop
4E88 00         nop
4E89 00         nop
4E8A 00         nop
4E8B 00         nop
4E8C 00         nop
4E8D 00         nop
4E8E 00         nop
4E8F 00         nop
4E90 00         nop
4E91 00         nop
4E92 00         nop
4E93 00         nop
4E94 00         nop
4E95 00         nop
4E96 00         nop
4E97 00         nop
4E98 00         nop
4E99 00         nop
4E9A 00         nop
4E9B 00         nop
4E9C 00         nop
4E9D 00         nop
4E9E 00         nop
4E9F 00         nop
4EA0 00         nop
4EA1 00         nop
4EA2 00         nop
4EA3 00         nop
4EA4 00         nop
4EA5 00         nop
4EA6 00         nop
4EA7 00         nop
4EA8 00         nop
4EA9 00         nop
4EAA 00         nop
4EAB 00         nop
4EAC 00         nop
4EAD 00         nop
4EAE 00         nop
4EAF 00         nop
4EB0 00         nop
4EB1 00         nop
4EB2 00         nop
4EB3 00         nop
4EB4 00         nop
4EB5 00         nop
4EB6 00         nop
4EB7 00         nop
4EB8 00         nop
4EB9 00         nop
4EBA 00         nop
4EBB 00         nop
4EBC 00         nop
4EBD 00         nop
4EBE 00         nop
4EBF 00         nop
4EC0 00         nop
4EC1 00         nop
4EC2 00         nop
4EC3 00         nop
4EC4 00         nop
4EC5 00         nop
4EC6 00         nop
4EC7 00         nop
4EC8 00         nop
4EC9 00         nop
4ECA 00         nop
4ECB 00         nop
4ECC 00         nop
4ECD 00         nop
4ECE 00         nop
4ECF 00         nop
4ED0 00         nop
4ED1 00         nop
4ED2 00         nop
4ED3 00         nop
4ED4 00         nop
4ED5 00         nop
4ED6 00         nop
4ED7 00         nop
4ED8 00         nop
4ED9 00         nop
4EDA 00         nop
4EDB 00         nop
4EDC 00         nop
4EDD 00         nop
4EDE 00         nop
4EDF 00         nop
4EE0 00         nop
4EE1 00         nop
4EE2 00         nop
4EE3 00         nop
4EE4 00         nop
4EE5 00         nop
4EE6 00         nop
4EE7 00         nop
4EE8 00         nop
4EE9 00         nop
4EEA 00         nop
4EEB 00         nop
4EEC 00         nop
4EED 00         nop
4EEE 00         nop
4EEF 00         nop
4EF0 00         nop
4EF1 00         nop
4EF2 00         nop
4EF3 00         nop
4EF4 00         nop
4EF5 00         nop
4EF6 00         nop
4EF7 00         nop
4EF8 00         nop
4EF9 00         nop
4EFA 00         nop
4EFB 00         nop
4EFC 00         nop
4EFD 00         nop
4EFE 00         nop
4EFF 00         nop
4F00 00         nop
4F01 00         nop
4F02 00         nop
4F03 00         nop
4F04 00         nop
4F05 00         nop
4F06 00         nop
4F07 00         nop
4F08 00         nop
4F09 00         nop
4F0A 00         nop
4F0B 00         nop
4F0C 00         nop
4F0D 00         nop
4F0E 00         nop
4F0F 00         nop
4F10 00         nop
4F11 00         nop
4F12 00         nop
4F13 00         nop
4F14 00         nop
4F15 00         nop
4F16 00         nop
4F17 00         nop
4F18 00         nop
4F19 00         nop
4F1A 00         nop
4F1B 00         nop
4F1C 00         nop
4F1D 00         nop
4F1E 00         nop
4F1F 00         nop
4F20 00         nop
4F21 00         nop
4F22 00         nop
4F23 00         nop
4F24 00         nop
4F25 00         nop
4F26 00         nop
4F27 00         nop
4F28 00         nop
4F29 00         nop
4F2A 00         nop
4F2B 00         nop
4F2C 00         nop
4F2D 00         nop
4F2E 00         nop
4F2F 00         nop
4F30 00         nop
4F31 00         nop
4F32 00         nop
4F33 00         nop
4F34 00         nop
4F35 00         nop
4F36 00         nop
4F37 00         nop
4F38 00         nop
4F39 00         nop
4F3A 00         nop
4F3B 00         nop
4F3C 00         nop
4F3D 00         nop
4F3E 00         nop
4F3F 00         nop
4F40 00         nop
4F41 00         nop
4F42 00         nop
4F43 00         nop
4F44 00         nop
4F45 00         nop
4F46 00         nop
4F47 00         nop
4F48 00         nop
4F49 00         nop
4F4A 00         nop
4F4B 00         nop
4F4C 00         nop
4F4D 00         nop
4F4E 00         nop
4F4F 00         nop
4F50 00         nop
4F51 00         nop
4F52 00         nop
4F53 00         nop
4F54 00         nop
4F55 00         nop
4F56 00         nop
4F57 00         nop
4F58 00         nop
4F59 00         nop
4F5A 00         nop
4F5B 00         nop
4F5C 00         nop
4F5D 00         nop
4F5E 00         nop
4F5F 00         nop
4F60 00         nop
4F61 00         nop
4F62 00         nop
4F63 00         nop
4F64 00         nop
4F65 00         nop
4F66 00         nop
4F67 00         nop
4F68 00         nop
4F69 00         nop
4F6A 00         nop
4F6B 00         nop
4F6C 00         nop
4F6D 00         nop
4F6E 00         nop
4F6F 00         nop
4F70 00         nop
4F71 00         nop
4F72 00         nop
4F73 00         nop
4F74 00         nop
4F75 00         nop
4F76 00         nop
4F77 00         nop
4F78 00         nop
4F79 00         nop
4F7A 00         nop
4F7B 00         nop
4F7C 00         nop
4F7D 00         nop
4F7E 00         nop
4F7F 00         nop
4F80 00         nop
4F81 00         nop
4F82 00         nop
4F83 00         nop
4F84 00         nop
4F85 00         nop
4F86 00         nop
4F87 00         nop
4F88 00         nop
4F89 00         nop
4F8A 00         nop
4F8B 00         nop
4F8C 00         nop
4F8D 00         nop
4F8E 00         nop
4F8F 00         nop
4F90 00         nop
4F91 00         nop
4F92 00         nop
4F93 00         nop
4F94 00         nop
4F95 00         nop
4F96 00         nop
4F97 00         nop
4F98 00         nop
4F99 00         nop
4F9A 00         nop
4F9B 00         nop
4F9C 00         nop
4F9D 00         nop
4F9E 00         nop
4F9F 00         nop
4FA0 00         nop
4FA1 00         nop
4FA2 00         nop
4FA3 00         nop
4FA4 00         nop
4FA5 00         nop
4FA6 00         nop
4FA7 00         nop
4FA8 00         nop
4FA9 00         nop
4FAA 00         nop
4FAB 00         nop
4FAC 00         nop
4FAD 00         nop
4FAE 00         nop
4FAF 00         nop
4FB0 00         nop
4FB1 00         nop
4FB2 00         nop
4FB3 00         nop
4FB4 00         nop
4FB5 00         nop
4FB6 00         nop
4FB7 00         nop
4FB8 00         nop
4FB9 00         nop
4FBA 00         nop
4FBB 00         nop
4FBC 00         nop
4FBD 00         nop
4FBE 00         nop
4FBF 00         nop
4FC0 00         nop
4FC1 00         nop
4FC2 00         nop
4FC3 00         nop
4FC4 00         nop
4FC5 00         nop
4FC6 00         nop
4FC7 00         nop
4FC8 00         nop
4FC9 00         nop
4FCA 00         nop
4FCB 00         nop
4FCC 00         nop
4FCD 00         nop
4FCE 00         nop
4FCF 00         nop
4FD0 00         nop
4FD1 00         nop
4FD2 00         nop
4FD3 00         nop
4FD4 00         nop
4FD5 00         nop
4FD6 00         nop
4FD7 00         nop
4FD8 00         nop
4FD9 00         nop
4FDA 00         nop
4FDB 00         nop
4FDC 00         nop
4FDD 00         nop
4FDE 00         nop
4FDF 00         nop
4FE0 00         nop
4FE1 00         nop
4FE2 00         nop
4FE3 00         nop
4FE4 00         nop
4FE5 00         nop
4FE6 00         nop
4FE7 00         nop
4FE8 00         nop
4FE9 00         nop
4FEA 00         nop
4FEB 00         nop
4FEC 00         nop
4FED 00         nop
4FEE 00         nop
4FEF 00         nop
4FF0 00         nop
4FF1 00         nop
4FF2 00         nop
4FF3 00         nop
4FF4 00         nop
4FF5 00         nop
4FF6 00         nop
4FF7 00         nop
4FF8 00         nop
4FF9 00         nop
4FFA 00         nop
4FFB 00         nop
4FFC 00         nop
4FFD 00         nop
4FFE 00         nop
4FFF 00         nop
5000 00         nop
5001 00         nop
5002 00         nop
5003 00         nop
5004 00         nop
5005 00         nop
5006 00         nop
5007 00         nop
5008 00         nop
5009 00         nop
500A 00         nop
500B 00         nop
500C 00         nop
500D 00         nop
500E 00         nop
500F 00         nop
5010 00         nop
5011 00         nop
5012 00         nop
5013 00         nop
5014 00         nop
5015 00         nop
5016 00         nop
5017 00         nop
5018 00         nop
5019 00         nop
501A 00         nop
501B 00         nop
501C 00         nop
501D 00         nop
501E 00         nop
501F 00         nop
5020 00         nop
5021 00         nop
5022 00         nop
5023 00         nop
5024 00         nop
5025 00         nop
5026 00         nop
5027 00         nop
5028 00         nop
5029 00         nop
502A 00         nop
502B 00         nop
502C 00         nop
502D 00         nop
502E 00         nop
502F 00         nop
5030 00         nop
5031 00         nop
5032 00         nop
5033 00         nop
5034 00         nop
5035 00         nop
5036 00         nop
5037 00         nop
5038 00         nop
5039 00         nop
503A 00         nop
503B 00         nop
503C 00         nop
503D 00         nop
503E 00         nop
503F 00         nop
5040 00         nop
5041 00         nop
5042 00         nop
5043 00         nop
5044 00         nop
5045 00         nop
5046 00         nop
5047 00         nop
5048 00         nop
5049 00         nop
504A 00         nop
504B 00         nop
504C 00         nop
504D 00         nop
504E 00         nop
504F 00         nop
5050 00         nop
5051 00         nop
5052 00         nop
5053 00         nop
5054 00         nop
5055 00         nop
5056 00         nop
5057 00         nop
5058 00         nop
5059 00         nop
505A 00         nop
505B 00         nop
505C 00         nop
505D 00         nop
505E 00         nop
505F 00         nop
5060 00         nop
5061 00         nop
5062 00         nop
5063 00         nop
5064 00         nop
5065 00         nop
5066 00         nop
5067 00         nop
5068 00         nop
5069 00         nop
506A 00         nop
506B 00         nop
506C 00         nop
506D 00         nop
506E 00         nop
506F 00         nop
5070 00         nop
5071 00         nop
5072 00         nop
5073 00         nop
5074 00         nop
5075 00         nop
5076 00         nop
5077 00         nop
5078 00         nop
5079 00         nop
507A 00         nop
507B 00         nop
507C 00         nop
507D 00         nop
507E 00         nop
507F 00         nop
5080 00         nop
5081 00         nop
5082 00         nop
5083 00         nop
5084 00         nop
5085 00         nop
5086 00         nop
5087 00         nop
5088 00         nop
5089 00         nop
508A 00         nop
508B 00         nop
508C 00         nop
508D 00         nop
508E 00         nop
508F 00         nop
5090 00         nop
5091 00         nop
5092 00         nop
5093 00         nop
5094 00         nop
5095 00         nop
5096 00         nop
5097 00         nop
5098 00         nop
5099 00         nop
509A 00         nop
509B 00         nop
509C 00         nop
509D 00         nop
509E 00         nop
509F 00         nop
50A0 00         nop
50A1 00         nop
50A2 00         nop
50A3 00         nop
50A4 00         nop
50A5 00         nop
50A6 00         nop
50A7 00         nop
50A8 00         nop
50A9 00         nop
50AA 00         nop
50AB 00         nop
50AC 00         nop
50AD 00         nop
50AE 00         nop
50AF 00         nop
50B0 00         nop
50B1 00         nop
50B2 00         nop
50B3 00         nop
50B4 00         nop
50B5 00         nop
50B6 00         nop
50B7 00         nop
50B8 00         nop
50B9 00         nop
50BA 00         nop
50BB 00         nop
50BC 00         nop
50BD 00         nop
50BE 00         nop
50BF 00         nop
50C0 00         nop
50C1 00         nop
50C2 00         nop
50C3 00         nop
50C4 00         nop
50C5 00         nop
50C6 00         nop
50C7 00         nop
50C8 00         nop
50C9 00         nop
50CA 00         nop
50CB 00         nop
50CC 00         nop
50CD 00         nop
50CE 00         nop
50CF 00         nop
50D0 00         nop
50D1 76         halt
50D2 03         inc bc				:B
									:; REM of end of assembly
