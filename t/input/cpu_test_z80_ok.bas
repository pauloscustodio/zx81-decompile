#ASM
    adc (hl)                ; 4082 8e
    adc (hl+)               ; 4083 8e 23
    adc (hl-)               ; 4085 8e 2b
    adc (ix)                ; 4087 dd 8e 00
    adc (ix+0)              ; 408a dd 8e 00
    adc (ix+126)            ; 408d dd 8e 7e
    adc (ix-128)            ; 4090 dd 8e 80
    adc (iy)                ; 4093 fd 8e 00
    adc (iy+0)              ; 4096 fd 8e 00
    adc (iy+126)            ; 4099 fd 8e 7e
    adc (iy-128)            ; 409c fd 8e 80
    adc 0                   ; 409f ce 00
    adc 127                 ; 40a1 ce 7f
    adc 255                 ; 40a3 ce ff
    adc a                   ; 40a5 8f
    adc a, (hl)             ; 40a6 8e
    adc a, (hl+)            ; 40a7 8e 23
    adc a, (hl-)            ; 40a9 8e 2b
    adc a, (ix)             ; 40ab dd 8e 00
    adc a, (ix+0)           ; 40ae dd 8e 00
    adc a, (ix+126)         ; 40b1 dd 8e 7e
    adc a, (ix-128)         ; 40b4 dd 8e 80
    adc a, (iy)             ; 40b7 fd 8e 00
    adc a, (iy+0)           ; 40ba fd 8e 00
    adc a, (iy+126)         ; 40bd fd 8e 7e
    adc a, (iy-128)         ; 40c0 fd 8e 80
    adc a, 0                ; 40c3 ce 00
    adc a, 127              ; 40c5 ce 7f
    adc a, 255              ; 40c7 ce ff
    adc a, a                ; 40c9 8f
    adc a, b                ; 40ca 88
    adc a, c                ; 40cb 89
    adc a, d                ; 40cc 8a
    adc a, e                ; 40cd 8b
    adc a, h                ; 40ce 8c
    adc a, ixh              ; 40cf dd 8c
    adc a, ixl              ; 40d1 dd 8d
    adc a, iyh              ; 40d3 fd 8c
    adc a, iyl              ; 40d5 fd 8d
    adc a, l                ; 40d7 8d
    adc b                   ; 40d8 88
    adc c                   ; 40d9 89
    adc d                   ; 40da 8a
    adc e                   ; 40db 8b
    adc h                   ; 40dc 8c
    adc hl, bc              ; 40dd ed 4a
    adc hl, de              ; 40df ed 5a
    adc hl, hl              ; 40e1 ed 6a
    adc hl, sp              ; 40e3 ed 7a
    adc ixh                 ; 40e5 dd 8c
    adc ixl                 ; 40e7 dd 8d
    adc iyh                 ; 40e9 fd 8c
    adc iyl                 ; 40eb fd 8d
    adc l                   ; 40ed 8d
    add (hl)                ; 40ee 86
    add (hl+)               ; 40ef 86 23
    add (hl-)               ; 40f1 86 2b
    add (ix)                ; 40f3 dd 86 00
    add (ix+0)              ; 40f6 dd 86 00
    add (ix+126)            ; 40f9 dd 86 7e
    add (ix-128)            ; 40fc dd 86 80
    add (iy)                ; 40ff fd 86 00
    add (iy+0)              ; 4102 fd 86 00
    add (iy+126)            ; 4105 fd 86 7e
    add (iy-128)            ; 4108 fd 86 80
    add 0                   ; 410b c6 00
    add 127                 ; 410d c6 7f
    add 255                 ; 410f c6 ff
    add a                   ; 4111 87
    add a, (hl)             ; 4112 86
    add a, (hl+)            ; 4113 86 23
    add a, (hl-)            ; 4115 86 2b
    add a, (ix)             ; 4117 dd 86 00
    add a, (ix+0)           ; 411a dd 86 00
    add a, (ix+126)         ; 411d dd 86 7e
    add a, (ix-128)         ; 4120 dd 86 80
    add a, (iy)             ; 4123 fd 86 00
    add a, (iy+0)           ; 4126 fd 86 00
    add a, (iy+126)         ; 4129 fd 86 7e
    add a, (iy-128)         ; 412c fd 86 80
    add a, 0                ; 412f c6 00
    add a, 127              ; 4131 c6 7f
    add a, 255              ; 4133 c6 ff
    add a, a                ; 4135 87
    add a, b                ; 4136 80
    add a, c                ; 4137 81
    add a, d                ; 4138 82
    add a, e                ; 4139 83
    add a, h                ; 413a 84
    add a, ixh              ; 413b dd 84
    add a, ixl              ; 413d dd 85
    add a, iyh              ; 413f fd 84
    add a, iyl              ; 4141 fd 85
    add a, l                ; 4143 85
    add b                   ; 4144 80
    add bc, 0x1234          ; 4145 e5 21 34 12 09 44 4d e1
    add c                   ; 414d 81
    add d                   ; 414e 82
    add de, 0x1234          ; 414f e5 21 34 12 19 54 5d e1
    add e                   ; 4157 83
    add h                   ; 4158 84
    add hl, 0x1234          ; 4159 d5 11 34 12 19 d1
    add hl, bc              ; 415f 09
    add hl, de              ; 4160 19
    add hl, hl              ; 4161 29
    add hl, sp              ; 4162 39
    add ix, bc              ; 4163 dd 09
    add ix, de              ; 4165 dd 19
    add ix, ix              ; 4167 dd 29
    add ix, sp              ; 4169 dd 39
    add ixh                 ; 416b dd 84
    add ixl                 ; 416d dd 85
    add iy, bc              ; 416f fd 09
    add iy, de              ; 4171 fd 19
    add iy, iy              ; 4173 fd 29
    add iy, sp              ; 4175 fd 39
    add iyh                 ; 4177 fd 84
    add iyl                 ; 4179 fd 85
    add l                   ; 417b 85
    and (hl)                ; 417c a6
    and (hl+)               ; 417d a6 23
    and (hl-)               ; 417f a6 2b
    and (ix)                ; 4181 dd a6 00
    and (ix+0)              ; 4184 dd a6 00
    and (ix+126)            ; 4187 dd a6 7e
    and (ix-128)            ; 418a dd a6 80
    and (iy)                ; 418d fd a6 00
    and (iy+0)              ; 4190 fd a6 00
    and (iy+126)            ; 4193 fd a6 7e
    and (iy-128)            ; 4196 fd a6 80
    and 0                   ; 4199 e6 00
    and 127                 ; 419b e6 7f
    and 255                 ; 419d e6 ff
    and a                   ; 419f a7
    and a, (hl)             ; 41a0 a6
    and a, (hl+)            ; 41a1 a6 23
    and a, (hl-)            ; 41a3 a6 2b
    and a, (ix)             ; 41a5 dd a6 00
    and a, (ix+0)           ; 41a8 dd a6 00
    and a, (ix+126)         ; 41ab dd a6 7e
    and a, (ix-128)         ; 41ae dd a6 80
    and a, (iy)             ; 41b1 fd a6 00
    and a, (iy+0)           ; 41b4 fd a6 00
    and a, (iy+126)         ; 41b7 fd a6 7e
    and a, (iy-128)         ; 41ba fd a6 80
    and a, 0                ; 41bd e6 00
    and a, 127              ; 41bf e6 7f
    and a, 255              ; 41c1 e6 ff
    and a, a                ; 41c3 a7
    and a, b                ; 41c4 a0
    and a, c                ; 41c5 a1
    and a, d                ; 41c6 a2
    and a, e                ; 41c7 a3
    and a, h                ; 41c8 a4
    and a, ixh              ; 41c9 dd a4
    and a, ixl              ; 41cb dd a5
    and a, iyh              ; 41cd fd a4
    and a, iyl              ; 41cf fd a5
    and a, l                ; 41d1 a5
    and b                   ; 41d2 a0
    and c                   ; 41d3 a1
    and d                   ; 41d4 a2
    and e                   ; 41d5 a3
    and h                   ; 41d6 a4
    and hl, bc              ; 41d7 f5 7c a0 67 7d a1 6f f1
    and hl, de              ; 41df f5 7c a2 67 7d a3 6f f1
    and ix, bc              ; 41e7 f5 dd 7c a0 dd 67 dd 7d a1 dd 6f f1
    and ix, de              ; 41f3 f5 dd 7c a2 dd 67 dd 7d a3 dd 6f f1
    and ixh                 ; 41ff dd a4
    and ixl                 ; 4201 dd a5
    and iy, bc              ; 4203 f5 fd 7c a0 fd 67 fd 7d a1 fd 6f f1
    and iy, de              ; 420f f5 fd 7c a2 fd 67 fd 7d a3 fd 6f f1
    and iyh                 ; 421b fd a4
    and iyl                 ; 421d fd a5
    and l                   ; 421f a5
    bit 0, (hl)             ; 4220 cb 46
    bit 0, (ix)             ; 4222 dd cb 00 46
    bit 0, (ix+0)           ; 4226 dd cb 00 46
    bit 0, (ix+126)         ; 422a dd cb 7e 46
    bit 0, (ix-128)         ; 422e dd cb 80 46
    bit 0, (iy)             ; 4232 fd cb 00 46
    bit 0, (iy+0)           ; 4236 fd cb 00 46
    bit 0, (iy+126)         ; 423a fd cb 7e 46
    bit 0, (iy-128)         ; 423e fd cb 80 46
    bit 0, a                ; 4242 cb 47
    bit 0, b                ; 4244 cb 40
    bit 0, c                ; 4246 cb 41
    bit 0, d                ; 4248 cb 42
    bit 0, e                ; 424a cb 43
    bit 0, h                ; 424c cb 44
    bit 0, l                ; 424e cb 45
    bit 1, (hl)             ; 4250 cb 4e
    bit 1, (ix)             ; 4252 dd cb 00 4e
    bit 1, (ix+0)           ; 4256 dd cb 00 4e
    bit 1, (ix+126)         ; 425a dd cb 7e 4e
    bit 1, (ix-128)         ; 425e dd cb 80 4e
    bit 1, (iy)             ; 4262 fd cb 00 4e
    bit 1, (iy+0)           ; 4266 fd cb 00 4e
    bit 1, (iy+126)         ; 426a fd cb 7e 4e
    bit 1, (iy-128)         ; 426e fd cb 80 4e
    bit 1, a                ; 4272 cb 4f
    bit 1, b                ; 4274 cb 48
    bit 1, c                ; 4276 cb 49
    bit 1, d                ; 4278 cb 4a
    bit 1, e                ; 427a cb 4b
    bit 1, h                ; 427c cb 4c
    bit 1, l                ; 427e cb 4d
    bit 2, (hl)             ; 4280 cb 56
    bit 2, (ix)             ; 4282 dd cb 00 56
    bit 2, (ix+0)           ; 4286 dd cb 00 56
    bit 2, (ix+126)         ; 428a dd cb 7e 56
    bit 2, (ix-128)         ; 428e dd cb 80 56
    bit 2, (iy)             ; 4292 fd cb 00 56
    bit 2, (iy+0)           ; 4296 fd cb 00 56
    bit 2, (iy+126)         ; 429a fd cb 7e 56
    bit 2, (iy-128)         ; 429e fd cb 80 56
    bit 2, a                ; 42a2 cb 57
    bit 2, b                ; 42a4 cb 50
    bit 2, c                ; 42a6 cb 51
    bit 2, d                ; 42a8 cb 52
    bit 2, e                ; 42aa cb 53
    bit 2, h                ; 42ac cb 54
    bit 2, l                ; 42ae cb 55
    bit 3, (hl)             ; 42b0 cb 5e
    bit 3, (ix)             ; 42b2 dd cb 00 5e
    bit 3, (ix+0)           ; 42b6 dd cb 00 5e
    bit 3, (ix+126)         ; 42ba dd cb 7e 5e
    bit 3, (ix-128)         ; 42be dd cb 80 5e
    bit 3, (iy)             ; 42c2 fd cb 00 5e
    bit 3, (iy+0)           ; 42c6 fd cb 00 5e
    bit 3, (iy+126)         ; 42ca fd cb 7e 5e
    bit 3, (iy-128)         ; 42ce fd cb 80 5e
    bit 3, a                ; 42d2 cb 5f
    bit 3, b                ; 42d4 cb 58
    bit 3, c                ; 42d6 cb 59
    bit 3, d                ; 42d8 cb 5a
    bit 3, e                ; 42da cb 5b
    bit 3, h                ; 42dc cb 5c
    bit 3, l                ; 42de cb 5d
    bit 4, (hl)             ; 42e0 cb 66
    bit 4, (ix)             ; 42e2 dd cb 00 66
    bit 4, (ix+0)           ; 42e6 dd cb 00 66
    bit 4, (ix+126)         ; 42ea dd cb 7e 66
    bit 4, (ix-128)         ; 42ee dd cb 80 66
    bit 4, (iy)             ; 42f2 fd cb 00 66
    bit 4, (iy+0)           ; 42f6 fd cb 00 66
    bit 4, (iy+126)         ; 42fa fd cb 7e 66
    bit 4, (iy-128)         ; 42fe fd cb 80 66
    bit 4, a                ; 4302 cb 67
    bit 4, b                ; 4304 cb 60
    bit 4, c                ; 4306 cb 61
    bit 4, d                ; 4308 cb 62
    bit 4, e                ; 430a cb 63
    bit 4, h                ; 430c cb 64
    bit 4, l                ; 430e cb 65
    bit 5, (hl)             ; 4310 cb 6e
    bit 5, (ix)             ; 4312 dd cb 00 6e
    bit 5, (ix+0)           ; 4316 dd cb 00 6e
    bit 5, (ix+126)         ; 431a dd cb 7e 6e
    bit 5, (ix-128)         ; 431e dd cb 80 6e
    bit 5, (iy)             ; 4322 fd cb 00 6e
    bit 5, (iy+0)           ; 4326 fd cb 00 6e
    bit 5, (iy+126)         ; 432a fd cb 7e 6e
    bit 5, (iy-128)         ; 432e fd cb 80 6e
    bit 5, a                ; 4332 cb 6f
    bit 5, b                ; 4334 cb 68
    bit 5, c                ; 4336 cb 69
    bit 5, d                ; 4338 cb 6a
    bit 5, e                ; 433a cb 6b
    bit 5, h                ; 433c cb 6c
    bit 5, l                ; 433e cb 6d
    bit 6, (hl)             ; 4340 cb 76
    bit 6, (ix)             ; 4342 dd cb 00 76
    bit 6, (ix+0)           ; 4346 dd cb 00 76
    bit 6, (ix+126)         ; 434a dd cb 7e 76
    bit 6, (ix-128)         ; 434e dd cb 80 76
    bit 6, (iy)             ; 4352 fd cb 00 76
    bit 6, (iy+0)           ; 4356 fd cb 00 76
    bit 6, (iy+126)         ; 435a fd cb 7e 76
    bit 6, (iy-128)         ; 435e fd cb 80 76
    bit 6, a                ; 4362 cb 77
    bit 6, b                ; 4364 cb 70
    bit 6, c                ; 4366 cb 71
    bit 6, d                ; 4368 cb 72
    bit 6, e                ; 436a cb 73
    bit 6, h                ; 436c cb 74
    bit 6, l                ; 436e cb 75
    bit 7, (hl)             ; 4370 cb 7e
    bit 7, (ix)             ; 4372 dd cb 00 7e
    bit 7, (ix+0)           ; 4376 dd cb 00 7e
    bit 7, (ix+126)         ; 437a dd cb 7e 7e
    bit 7, (ix-128)         ; 437e dd cb 80 7e
    bit 7, (iy)             ; 4382 fd cb 00 7e
    bit 7, (iy+0)           ; 4386 fd cb 00 7e
    bit 7, (iy+126)         ; 438a fd cb 7e 7e
    bit 7, (iy-128)         ; 438e fd cb 80 7e
    bit 7, a                ; 4392 cb 7f
    bit 7, b                ; 4394 cb 78
    bit 7, c                ; 4396 cb 79
    bit 7, d                ; 4398 cb 7a
    bit 7, e                ; 439a cb 7b
    bit 7, h                ; 439c cb 7c
    bit 7, l                ; 439e cb 7d
    bool hl                 ; 43a0 f5 7c b5 28 03 21 01 00 f1
    bool ix                 ; 43a9 f5 dd 7c dd b5 28 04 dd 21 01 00 f1
    bool iy                 ; 43b5 f5 fd 7c fd b5 28 04 fd 21 01 00 f1
    call 0x1234             ; 43c1 cd 34 12
    call c, 0x1234          ; 43c4 dc 34 12
    call eq, 0x1234         ; 43c7 cc 34 12
    call geu, 0x1234        ; 43ca d4 34 12
    call gtu, 0x1234        ; 43cd 28 03 d4 34 12
    call leu, 0x1234        ; 43d2 28 02 30 03 cd 34 12
    call ltu, 0x1234        ; 43d9 dc 34 12
    call m, 0x1234          ; 43dc fc 34 12
    call nc, 0x1234         ; 43df d4 34 12
    call ne, 0x1234         ; 43e2 c4 34 12
    call nv, 0x1234         ; 43e5 e4 34 12
    call nz, 0x1234         ; 43e8 c4 34 12
    call p, 0x1234          ; 43eb f4 34 12
    call pe, 0x1234         ; 43ee ec 34 12
    call po, 0x1234         ; 43f1 e4 34 12
    call v, 0x1234          ; 43f4 ec 34 12
    call z, 0x1234          ; 43f7 cc 34 12
    ccf                     ; 43fa 3f
    clr (hl)                ; 43fb 36 00
    clr (ix)                ; 43fd dd 36 00 00
    clr (ix+0)              ; 4401 dd 36 00 00
    clr (ix+126)            ; 4405 dd 36 7e 00
    clr (ix-128)            ; 4409 dd 36 80 00
    clr (iy)                ; 440d fd 36 00 00
    clr (iy+0)              ; 4411 fd 36 00 00
    clr (iy+126)            ; 4415 fd 36 7e 00
    clr (iy-128)            ; 4419 fd 36 80 00
    clr a                   ; 441d 3e 00
    clr b                   ; 441f 06 00
    clr bc                  ; 4421 01 00 00
    clr c                   ; 4424 0e 00
    clr d                   ; 4426 16 00
    clr de                  ; 4428 11 00 00
    clr e                   ; 442b 1e 00
    clr h                   ; 442d 26 00
    clr hl                  ; 442f 21 00 00
    clr ix                  ; 4432 dd 21 00 00
    clr ixh                 ; 4436 dd 26 00
    clr ixl                 ; 4439 dd 2e 00
    clr iy                  ; 443c fd 21 00 00
    clr iyh                 ; 4440 fd 26 00
    clr iyl                 ; 4443 fd 2e 00
    clr l                   ; 4446 2e 00
    cp (hl)                 ; 4448 be
    cp (hl+)                ; 4449 be 23
    cp (hl-)                ; 444b be 2b
    cp (ix)                 ; 444d dd be 00
    cp (ix+0)               ; 4450 dd be 00
    cp (ix+126)             ; 4453 dd be 7e
    cp (ix-128)             ; 4456 dd be 80
    cp (iy)                 ; 4459 fd be 00
    cp (iy+0)               ; 445c fd be 00
    cp (iy+126)             ; 445f fd be 7e
    cp (iy-128)             ; 4462 fd be 80
    cp 0                    ; 4465 fe 00
    cp 127                  ; 4467 fe 7f
    cp 255                  ; 4469 fe ff
    cp a                    ; 446b bf
    cp a, (hl)              ; 446c be
    cp a, (hl+)             ; 446d be 23
    cp a, (hl-)             ; 446f be 2b
    cp a, (ix)              ; 4471 dd be 00
    cp a, (ix+0)            ; 4474 dd be 00
    cp a, (ix+126)          ; 4477 dd be 7e
    cp a, (ix-128)          ; 447a dd be 80
    cp a, (iy)              ; 447d fd be 00
    cp a, (iy+0)            ; 4480 fd be 00
    cp a, (iy+126)          ; 4483 fd be 7e
    cp a, (iy-128)          ; 4486 fd be 80
    cp a, 0                 ; 4489 fe 00
    cp a, 127               ; 448b fe 7f
    cp a, 255               ; 448d fe ff
    cp a, a                 ; 448f bf
    cp a, b                 ; 4490 b8
    cp a, c                 ; 4491 b9
    cp a, d                 ; 4492 ba
    cp a, e                 ; 4493 bb
    cp a, h                 ; 4494 bc
    cp a, ixh               ; 4495 dd bc
    cp a, ixl               ; 4497 dd bd
    cp a, iyh               ; 4499 fd bc
    cp a, iyl               ; 449b fd bd
    cp a, l                 ; 449d bd
    cp b                    ; 449e b8
    cp c                    ; 449f b9
    cp d                    ; 44a0 ba
    cp e                    ; 44a1 bb
    cp h                    ; 44a2 bc
    cp ixh                  ; 44a3 dd bc
    cp ixl                  ; 44a5 dd bd
    cp iyh                  ; 44a7 fd bc
    cp iyl                  ; 44a9 fd bd
    cp l                    ; 44ab bd
    cpd                     ; 44ac ed a9
    cpdr                    ; 44ae ed b9
    cpi                     ; 44b0 ed a1
    cpir                    ; 44b2 ed b1
    cpl                     ; 44b4 2f
    daa                     ; 44b5 27
    dec (hl)                ; 44b6 35
    dec (hl+)               ; 44b7 35 23
    dec (hl-)               ; 44b9 35 2b
    dec (ix)                ; 44bb dd 35 00
    dec (ix+0)              ; 44be dd 35 00
    dec (ix+126)            ; 44c1 dd 35 7e
    dec (ix-128)            ; 44c4 dd 35 80
    dec (iy)                ; 44c7 fd 35 00
    dec (iy+0)              ; 44ca fd 35 00
    dec (iy+126)            ; 44cd fd 35 7e
    dec (iy-128)            ; 44d0 fd 35 80
    dec a                   ; 44d3 3d
    dec b                   ; 44d4 05
    dec bc                  ; 44d5 0b
    dec c                   ; 44d6 0d
    dec d                   ; 44d7 15
    dec de                  ; 44d8 1b
    dec e                   ; 44d9 1d
    dec h                   ; 44da 25
    dec hl                  ; 44db 2b
    dec ix                  ; 44dc dd 2b
    dec ixh                 ; 44de dd 25
    dec ixl                 ; 44e0 dd 2d
    dec iy                  ; 44e2 fd 2b
    dec iyh                 ; 44e4 fd 25
    dec iyl                 ; 44e6 fd 2d
    dec l                   ; 44e8 2d
    dec sp                  ; 44e9 3b
    di                      ; 44ea f3
    djnz ASMPC              ; 44eb 10 fe
    ei                      ; 44ed fb
    ex (sp), hl             ; 44ee e3
    ex (sp), ix             ; 44ef dd e3
    ex (sp), iy             ; 44f1 fd e3
    ex af', af              ; 44f3 08
    ex af, af               ; 44f4 08
    ex af, af'              ; 44f5 08
    ex bc, hl               ; 44f6 e5 c5 e1 c1
    ex de, hl               ; 44fa eb
    ex hl, bc               ; 44fb e5 c5 e1 c1
    ex hl, de               ; 44ff eb
    exx                     ; 4500 d9
    halt                    ; 4501 76
    im 0                    ; 4502 ed 46
    im 1                    ; 4504 ed 56
    im 2                    ; 4506 ed 5e
    in a, (0)               ; 4508 db 00
    in a, (127)             ; 450a db 7f
    in a, (255)             ; 450c db ff
    in a, (c)               ; 450e ed 78
    in b, (c)               ; 4510 ed 40
    in c, (c)               ; 4512 ed 48
    in d, (c)               ; 4514 ed 50
    in e, (c)               ; 4516 ed 58
    in h, (c)               ; 4518 ed 60
    in l, (c)               ; 451a ed 68
    inc (hl)                ; 451c 34
    inc (hl+)               ; 451d 34 23
    inc (hl-)               ; 451f 34 2b
    inc (ix)                ; 4521 dd 34 00
    inc (ix+0)              ; 4524 dd 34 00
    inc (ix+126)            ; 4527 dd 34 7e
    inc (ix-128)            ; 452a dd 34 80
    inc (iy)                ; 452d fd 34 00
    inc (iy+0)              ; 4530 fd 34 00
    inc (iy+126)            ; 4533 fd 34 7e
    inc (iy-128)            ; 4536 fd 34 80
    inc a                   ; 4539 3c
    inc b                   ; 453a 04
    inc bc                  ; 453b 03
    inc c                   ; 453c 0c
    inc d                   ; 453d 14
    inc de                  ; 453e 13
    inc e                   ; 453f 1c
    inc h                   ; 4540 24
    inc hl                  ; 4541 23
    inc ix                  ; 4542 dd 23
    inc ixh                 ; 4544 dd 24
    inc ixl                 ; 4546 dd 2c
    inc iy                  ; 4548 fd 23
    inc iyh                 ; 454a fd 24
    inc iyl                 ; 454c fd 2c
    inc l                   ; 454e 2c
    inc sp                  ; 454f 33
    ind                     ; 4550 ed aa
    indr                    ; 4552 ed ba
    ini                     ; 4554 ed a2
    inir                    ; 4556 ed b2
    jp (bc)                 ; 4558 c5 c9
    jp (de)                 ; 455a d5 c9
    jp (hl)                 ; 455c e9
    jp (ix)                 ; 455d dd e9
    jp (iy)                 ; 455f fd e9
    jp 0x1234               ; 4561 c3 34 12
    jp c, 0x1234            ; 4564 da 34 12
    jp eq, 0x1234           ; 4567 ca 34 12
    jp geu, 0x1234          ; 456a d2 34 12
    jp gtu, 0x1234          ; 456d 28 03 d2 34 12
    jp leu, 0x1234          ; 4572 ca 34 12 da 34 12
    jp ltu, 0x1234          ; 4578 da 34 12
    jp m, 0x1234            ; 457b fa 34 12
    jp nc, 0x1234           ; 457e d2 34 12
    jp ne, 0x1234           ; 4581 c2 34 12
    jp nv, 0x1234           ; 4584 e2 34 12
    jp nz, 0x1234           ; 4587 c2 34 12
    jp p, 0x1234            ; 458a f2 34 12
    jp pe, 0x1234           ; 458d ea 34 12
    jp po, 0x1234           ; 4590 e2 34 12
    jp v, 0x1234            ; 4593 ea 34 12
    jp z, 0x1234            ; 4596 ca 34 12
    jr ASMPC                ; 4599 18 fe
    jr c, ASMPC             ; 459b 38 fe
    jr eq, ASMPC            ; 459d 28 fe
    jr geu, ASMPC           ; 459f 30 fe
    jr gtu, ASMPC           ; 45a1 28 02 30 fc
    jr leu, ASMPC           ; 45a5 28 fe 38 fc
    jr ltu, ASMPC           ; 45a9 38 fe
    jr nc, ASMPC            ; 45ab 30 fe
    jr ne, ASMPC            ; 45ad 20 fe
    jr nz, ASMPC            ; 45af 20 fe
    jr z, ASMPC             ; 45b1 28 fe
    ld (0x1234), a          ; 45b3 32 34 12
    ld (0x1234), bc         ; 45b6 ed 43 34 12
    ld (0x1234), de         ; 45ba ed 53 34 12
    ld (0x1234), hl         ; 45be 22 34 12
    ld (0x1234), ix         ; 45c1 dd 22 34 12
    ld (0x1234), iy         ; 45c5 fd 22 34 12
    ld (0x1234), sp         ; 45c9 ed 73 34 12
    ld (bc), a              ; 45cd 02
    ld (bc+), a             ; 45ce 02 03
    ld (bc-), a             ; 45d0 02 0b
    ld (de), 0              ; 45d2 eb 36 00 eb
    ld (de), 127            ; 45d6 eb 36 7f eb
    ld (de), 255            ; 45da eb 36 ff eb
    ld (de), a              ; 45de 12
    ld (de), b              ; 45df eb 70 eb
    ld (de), c              ; 45e2 eb 71 eb
    ld (de), d              ; 45e5 eb 74 eb
    ld (de), e              ; 45e8 eb 75 eb
    ld (de), h              ; 45eb eb 72 eb
    ld (de), hl             ; 45ee eb 73 23 72 2b eb
    ld (de), l              ; 45f4 eb 73 eb
    ld (de+), 0             ; 45f7 eb 36 00 eb 13
    ld (de+), 127           ; 45fc eb 36 7f eb 13
    ld (de+), 255           ; 4601 eb 36 ff eb 13
    ld (de+), a             ; 4606 12 13
    ld (de+), b             ; 4608 eb 70 eb 13
    ld (de+), c             ; 460c eb 71 eb 13
    ld (de+), d             ; 4610 eb 74 eb 13
    ld (de+), e             ; 4614 eb 75 eb 13
    ld (de+), h             ; 4618 eb 72 eb 13
    ld (de+), l             ; 461c eb 73 eb 13
    ld (de-), 0             ; 4620 eb 36 00 eb 1b
    ld (de-), 127           ; 4625 eb 36 7f eb 1b
    ld (de-), 255           ; 462a eb 36 ff eb 1b
    ld (de-), a             ; 462f 12 1b
    ld (de-), b             ; 4631 eb 70 eb 1b
    ld (de-), c             ; 4635 eb 71 eb 1b
    ld (de-), d             ; 4639 eb 74 eb 1b
    ld (de-), e             ; 463d eb 75 eb 1b
    ld (de-), h             ; 4641 eb 72 eb 1b
    ld (de-), l             ; 4645 eb 73 eb 1b
    ld (hl), 0              ; 4649 36 00
    ld (hl), 127            ; 464b 36 7f
    ld (hl), 255            ; 464d 36 ff
    ld (hl), a              ; 464f 77
    ld (hl), b              ; 4650 70
    ld (hl), bc             ; 4651 71 23 70 2b
    ld (hl), c              ; 4655 71
    ld (hl), d              ; 4656 72
    ld (hl), de             ; 4657 73 23 72 2b
    ld (hl), e              ; 465b 73
    ld (hl), h              ; 465c 74
    ld (hl), hl             ; 465d f5 7c 75 23 77 f1 2b
    ld (hl), ix             ; 4664 d5 dd e5 d1 73 23 72 2b d1
    ld (hl), iy             ; 466d d5 fd e5 d1 73 23 72 2b d1
    ld (hl), l              ; 4676 75
    ld (hl+), 0             ; 4677 36 00 23
    ld (hl+), 127           ; 467a 36 7f 23
    ld (hl+), 255           ; 467d 36 ff 23
    ld (hl+), a             ; 4680 77 23
    ld (hl+), b             ; 4682 70 23
    ld (hl+), bc            ; 4684 71 23 70 23
    ld (hl+), c             ; 4688 71 23
    ld (hl+), d             ; 468a 72 23
    ld (hl+), de            ; 468c 73 23 72 23
    ld (hl+), e             ; 4690 73 23
    ld (hl+), h             ; 4692 74 23
    ld (hl+), hl            ; 4694 f5 7c 75 23 77 f1 23
    ld (hl+), ix            ; 469b d5 dd e5 d1 73 23 72 23 d1
    ld (hl+), iy            ; 46a4 d5 fd e5 d1 73 23 72 23 d1
    ld (hl+), l             ; 46ad 75 23
    ld (hl-), 0             ; 46af 36 00 2b
    ld (hl-), 127           ; 46b2 36 7f 2b
    ld (hl-), 255           ; 46b5 36 ff 2b
    ld (hl-), a             ; 46b8 77 2b
    ld (hl-), b             ; 46ba 70 2b
    ld (hl-), c             ; 46bc 71 2b
    ld (hl-), d             ; 46be 72 2b
    ld (hl-), e             ; 46c0 73 2b
    ld (hl-), h             ; 46c2 74 2b
    ld (hl-), l             ; 46c4 75 2b
    ld (ix), 0              ; 46c6 dd 36 00 00
    ld (ix), 127            ; 46ca dd 36 00 7f
    ld (ix), 255            ; 46ce dd 36 00 ff
    ld (ix), a              ; 46d2 dd 77 00
    ld (ix), b              ; 46d5 dd 70 00
    ld (ix), bc             ; 46d8 dd 71 00 dd 70 01
    ld (ix), c              ; 46de dd 71 00
    ld (ix), d              ; 46e1 dd 72 00
    ld (ix), de             ; 46e4 dd 73 00 dd 72 01
    ld (ix), e              ; 46ea dd 73 00
    ld (ix), h              ; 46ed dd 74 00
    ld (ix), hl             ; 46f0 dd 75 00 dd 74 01
    ld (ix), ix             ; 46f6 dd e5 e3 dd 75 00 dd 74 01 e3 dd e1
    ld (ix), iy             ; 4702 fd e5 e3 dd 75 00 dd 74 01 e3 fd e1
    ld (ix), l              ; 470e dd 75 00
    ld (ix+0), 0            ; 4711 dd 36 00 00
    ld (ix+0), 127          ; 4715 dd 36 00 7f
    ld (ix+0), 255          ; 4719 dd 36 00 ff
    ld (ix+0), a            ; 471d dd 77 00
    ld (ix+0), b            ; 4720 dd 70 00
    ld (ix+0), bc           ; 4723 dd 71 00 dd 70 01
    ld (ix+0), c            ; 4729 dd 71 00
    ld (ix+0), d            ; 472c dd 72 00
    ld (ix+0), de           ; 472f dd 73 00 dd 72 01
    ld (ix+0), e            ; 4735 dd 73 00
    ld (ix+0), h            ; 4738 dd 74 00
    ld (ix+0), hl           ; 473b dd 75 00 dd 74 01
    ld (ix+0), ix           ; 4741 dd e5 e3 dd 75 00 dd 74 01 e3 dd e1
    ld (ix+0), iy           ; 474d fd e5 e3 dd 75 00 dd 74 01 e3 fd e1
    ld (ix+0), l            ; 4759 dd 75 00
    ld (ix+126), 0          ; 475c dd 36 7e 00
    ld (ix+126), 127        ; 4760 dd 36 7e 7f
    ld (ix+126), 255        ; 4764 dd 36 7e ff
    ld (ix+126), a          ; 4768 dd 77 7e
    ld (ix+126), b          ; 476b dd 70 7e
    ld (ix+126), bc         ; 476e dd 71 7e dd 70 7f
    ld (ix+126), c          ; 4774 dd 71 7e
    ld (ix+126), d          ; 4777 dd 72 7e
    ld (ix+126), de         ; 477a dd 73 7e dd 72 7f
    ld (ix+126), e          ; 4780 dd 73 7e
    ld (ix+126), h          ; 4783 dd 74 7e
    ld (ix+126), hl         ; 4786 dd 75 7e dd 74 7f
    ld (ix+126), ix         ; 478c dd e5 e3 dd 75 7e dd 74 7f e3 dd e1
    ld (ix+126), iy         ; 4798 fd e5 e3 dd 75 7e dd 74 7f e3 fd e1
    ld (ix+126), l          ; 47a4 dd 75 7e
    ld (ix-128), 0          ; 47a7 dd 36 80 00
    ld (ix-128), 127        ; 47ab dd 36 80 7f
    ld (ix-128), 255        ; 47af dd 36 80 ff
    ld (ix-128), a          ; 47b3 dd 77 80
    ld (ix-128), b          ; 47b6 dd 70 80
    ld (ix-128), bc         ; 47b9 dd 71 80 dd 70 81
    ld (ix-128), c          ; 47bf dd 71 80
    ld (ix-128), d          ; 47c2 dd 72 80
    ld (ix-128), de         ; 47c5 dd 73 80 dd 72 81
    ld (ix-128), e          ; 47cb dd 73 80
    ld (ix-128), h          ; 47ce dd 74 80
    ld (ix-128), hl         ; 47d1 dd 75 80 dd 74 81
    ld (ix-128), ix         ; 47d7 dd e5 e3 dd 75 80 dd 74 81 e3 dd e1
    ld (ix-128), iy         ; 47e3 fd e5 e3 dd 75 80 dd 74 81 e3 fd e1
    ld (ix-128), l          ; 47ef dd 75 80
    ld (iy), 0              ; 47f2 fd 36 00 00
    ld (iy), 127            ; 47f6 fd 36 00 7f
    ld (iy), 255            ; 47fa fd 36 00 ff
    ld (iy), a              ; 47fe fd 77 00
    ld (iy), b              ; 4801 fd 70 00
    ld (iy), bc             ; 4804 fd 71 00 fd 70 01
    ld (iy), c              ; 480a fd 71 00
    ld (iy), d              ; 480d fd 72 00
    ld (iy), de             ; 4810 fd 73 00 fd 72 01
    ld (iy), e              ; 4816 fd 73 00
    ld (iy), h              ; 4819 fd 74 00
    ld (iy), hl             ; 481c fd 75 00 fd 74 01
    ld (iy), ix             ; 4822 dd e5 e3 fd 75 00 fd 74 01 e3 dd e1
    ld (iy), iy             ; 482e fd e5 e3 fd 75 00 fd 74 01 e3 fd e1
    ld (iy), l              ; 483a fd 75 00
    ld (iy+0), 0            ; 483d fd 36 00 00
    ld (iy+0), 127          ; 4841 fd 36 00 7f
    ld (iy+0), 255          ; 4845 fd 36 00 ff
    ld (iy+0), a            ; 4849 fd 77 00
    ld (iy+0), b            ; 484c fd 70 00
    ld (iy+0), bc           ; 484f fd 71 00 fd 70 01
    ld (iy+0), c            ; 4855 fd 71 00
    ld (iy+0), d            ; 4858 fd 72 00
    ld (iy+0), de           ; 485b fd 73 00 fd 72 01
    ld (iy+0), e            ; 4861 fd 73 00
    ld (iy+0), h            ; 4864 fd 74 00
    ld (iy+0), hl           ; 4867 fd 75 00 fd 74 01
    ld (iy+0), ix           ; 486d dd e5 e3 fd 75 00 fd 74 01 e3 dd e1
    ld (iy+0), iy           ; 4879 fd e5 e3 fd 75 00 fd 74 01 e3 fd e1
    ld (iy+0), l            ; 4885 fd 75 00
    ld (iy+126), 0          ; 4888 fd 36 7e 00
    ld (iy+126), 127        ; 488c fd 36 7e 7f
    ld (iy+126), 255        ; 4890 fd 36 7e ff
    ld (iy+126), a          ; 4894 fd 77 7e
    ld (iy+126), b          ; 4897 fd 70 7e
    ld (iy+126), bc         ; 489a fd 71 7e fd 70 7f
    ld (iy+126), c          ; 48a0 fd 71 7e
    ld (iy+126), d          ; 48a3 fd 72 7e
    ld (iy+126), de         ; 48a6 fd 73 7e fd 72 7f
    ld (iy+126), e          ; 48ac fd 73 7e
    ld (iy+126), h          ; 48af fd 74 7e
    ld (iy+126), hl         ; 48b2 fd 75 7e fd 74 7f
    ld (iy+126), ix         ; 48b8 dd e5 e3 fd 75 7e fd 74 7f e3 dd e1
    ld (iy+126), iy         ; 48c4 fd e5 e3 fd 75 7e fd 74 7f e3 fd e1
    ld (iy+126), l          ; 48d0 fd 75 7e
    ld (iy-128), 0          ; 48d3 fd 36 80 00
    ld (iy-128), 127        ; 48d7 fd 36 80 7f
    ld (iy-128), 255        ; 48db fd 36 80 ff
    ld (iy-128), a          ; 48df fd 77 80
    ld (iy-128), b          ; 48e2 fd 70 80
    ld (iy-128), bc         ; 48e5 fd 71 80 fd 70 81
    ld (iy-128), c          ; 48eb fd 71 80
    ld (iy-128), d          ; 48ee fd 72 80
    ld (iy-128), de         ; 48f1 fd 73 80 fd 72 81
    ld (iy-128), e          ; 48f7 fd 73 80
    ld (iy-128), h          ; 48fa fd 74 80
    ld (iy-128), hl         ; 48fd fd 75 80 fd 74 81
    ld (iy-128), ix         ; 4903 dd e5 e3 fd 75 80 fd 74 81 e3 dd e1
    ld (iy-128), iy         ; 490f fd e5 e3 fd 75 80 fd 74 81 e3 fd e1
    ld (iy-128), l          ; 491b fd 75 80
    ld a, (0x1234)          ; 491e 3a 34 12
    ld a, (bc)              ; 4921 0a
    ld a, (bc+)             ; 4922 0a 03
    ld a, (bc-)             ; 4924 0a 0b
    ld a, (de)              ; 4926 1a
    ld a, (de+)             ; 4927 1a 13
    ld a, (de-)             ; 4929 1a 1b
    ld a, (hl)              ; 492b 7e
    ld a, (hl+)             ; 492c 7e 23
    ld a, (hl-)             ; 492e 7e 2b
    ld a, (ix)              ; 4930 dd 7e 00
    ld a, (ix+0)            ; 4933 dd 7e 00
    ld a, (ix+126)          ; 4936 dd 7e 7e
    ld a, (ix-128)          ; 4939 dd 7e 80
    ld a, (iy)              ; 493c fd 7e 00
    ld a, (iy+0)            ; 493f fd 7e 00
    ld a, (iy+126)          ; 4942 fd 7e 7e
    ld a, (iy-128)          ; 4945 fd 7e 80
    ld a, 0                 ; 4948 3e 00
    ld a, 127               ; 494a 3e 7f
    ld a, 255               ; 494c 3e ff
    ld a, a                 ; 494e 7f
    ld a, b                 ; 494f 78
    ld a, c                 ; 4950 79
    ld a, d                 ; 4951 7a
    ld a, e                 ; 4952 7b
    ld a, h                 ; 4953 7c
    ld a, i                 ; 4954 ed 57
    ld a, ixh               ; 4956 dd 7c
    ld a, ixl               ; 4958 dd 7d
    ld a, iyh               ; 495a fd 7c
    ld a, iyl               ; 495c fd 7d
    ld a, l                 ; 495e 7d
    ld a, r                 ; 495f ed 5f
    ld b, (de)              ; 4961 eb 46 eb
    ld b, (de+)             ; 4964 eb 46 eb 13
    ld b, (de-)             ; 4968 eb 46 eb 1b
    ld b, (hl)              ; 496c 46
    ld b, (hl+)             ; 496d 46 23
    ld b, (hl-)             ; 496f 46 2b
    ld b, (ix)              ; 4971 dd 46 00
    ld b, (ix+0)            ; 4974 dd 46 00
    ld b, (ix+126)          ; 4977 dd 46 7e
    ld b, (ix-128)          ; 497a dd 46 80
    ld b, (iy)              ; 497d fd 46 00
    ld b, (iy+0)            ; 4980 fd 46 00
    ld b, (iy+126)          ; 4983 fd 46 7e
    ld b, (iy-128)          ; 4986 fd 46 80
    ld b, 0                 ; 4989 06 00
    ld b, 127               ; 498b 06 7f
    ld b, 255               ; 498d 06 ff
    ld b, a                 ; 498f 47
    ld b, b                 ; 4990 40
    ld b, c                 ; 4991 41
    ld b, d                 ; 4992 42
    ld b, e                 ; 4993 43
    ld b, h                 ; 4994 44
    ld b, ixh               ; 4995 dd 44
    ld b, ixl               ; 4997 dd 45
    ld b, iyh               ; 4999 fd 44
    ld b, iyl               ; 499b fd 45
    ld b, l                 ; 499d 45
    ld bc', de'             ; 499e d9 42 4b d9
    ld bc', hl'             ; 49a2 d9 44 4d d9
    ld bc, (0x1234)         ; 49a6 ed 4b 34 12
    ld bc, (hl)             ; 49aa 4e 23 46 2b
    ld bc, (hl+)            ; 49ae 4e 23 46 23
    ld bc, (ix)             ; 49b2 dd 4e 00 dd 46 01
    ld bc, (ix+0)           ; 49b8 dd 4e 00 dd 46 01
    ld bc, (ix+126)         ; 49be dd 4e 7e dd 46 7f
    ld bc, (ix-128)         ; 49c4 dd 4e 80 dd 46 81
    ld bc, (iy)             ; 49ca fd 4e 00 fd 46 01
    ld bc, (iy+0)           ; 49d0 fd 4e 00 fd 46 01
    ld bc, (iy+126)         ; 49d6 fd 4e 7e fd 46 7f
    ld bc, (iy-128)         ; 49dc fd 4e 80 fd 46 81
    ld bc, 0x1234           ; 49e2 01 34 12
    ld bc, bc'              ; 49e5 d9 c5 d9 c1
    ld bc, de               ; 49e9 42 4b
    ld bc, de'              ; 49eb d9 d5 d9 c1
    ld bc, hl               ; 49ef 44 4d
    ld bc, hl'              ; 49f1 d9 e5 d9 c1
    ld bc, ix               ; 49f5 dd 44 dd 4d
    ld bc, iy               ; 49f9 fd 44 fd 4d
    ld c, (de)              ; 49fd eb 4e eb
    ld c, (de+)             ; 4a00 eb 4e eb 13
    ld c, (de-)             ; 4a04 eb 4e eb 1b
    ld c, (hl)              ; 4a08 4e
    ld c, (hl+)             ; 4a09 4e 23
    ld c, (hl-)             ; 4a0b 4e 2b
    ld c, (ix)              ; 4a0d dd 4e 00
    ld c, (ix+0)            ; 4a10 dd 4e 00
    ld c, (ix+126)          ; 4a13 dd 4e 7e
    ld c, (ix-128)          ; 4a16 dd 4e 80
    ld c, (iy)              ; 4a19 fd 4e 00
    ld c, (iy+0)            ; 4a1c fd 4e 00
    ld c, (iy+126)          ; 4a1f fd 4e 7e
    ld c, (iy-128)          ; 4a22 fd 4e 80
    ld c, 0                 ; 4a25 0e 00
    ld c, 127               ; 4a27 0e 7f
    ld c, 255               ; 4a29 0e ff
    ld c, a                 ; 4a2b 4f
    ld c, b                 ; 4a2c 48
    ld c, c                 ; 4a2d 49
    ld c, d                 ; 4a2e 4a
    ld c, e                 ; 4a2f 4b
    ld c, h                 ; 4a30 4c
    ld c, ixh               ; 4a31 dd 4c
    ld c, ixl               ; 4a33 dd 4d
    ld c, iyh               ; 4a35 fd 4c
    ld c, iyl               ; 4a37 fd 4d
    ld c, l                 ; 4a39 4d
    ld d, (de)              ; 4a3a eb 66 eb
    ld d, (de+)             ; 4a3d eb 66 eb 13
    ld d, (de-)             ; 4a41 eb 66 eb 1b
    ld d, (hl)              ; 4a45 56
    ld d, (hl+)             ; 4a46 56 23
    ld d, (hl-)             ; 4a48 56 2b
    ld d, (ix)              ; 4a4a dd 56 00
    ld d, (ix+0)            ; 4a4d dd 56 00
    ld d, (ix+126)          ; 4a50 dd 56 7e
    ld d, (ix-128)          ; 4a53 dd 56 80
    ld d, (iy)              ; 4a56 fd 56 00
    ld d, (iy+0)            ; 4a59 fd 56 00
    ld d, (iy+126)          ; 4a5c fd 56 7e
    ld d, (iy-128)          ; 4a5f fd 56 80
    ld d, 0                 ; 4a62 16 00
    ld d, 127               ; 4a64 16 7f
    ld d, 255               ; 4a66 16 ff
    ld d, a                 ; 4a68 57
    ld d, b                 ; 4a69 50
    ld d, c                 ; 4a6a 51
    ld d, d                 ; 4a6b 52
    ld d, e                 ; 4a6c 53
    ld d, h                 ; 4a6d 54
    ld d, ixh               ; 4a6e dd 54
    ld d, ixl               ; 4a70 dd 55
    ld d, iyh               ; 4a72 fd 54
    ld d, iyl               ; 4a74 fd 55
    ld d, l                 ; 4a76 55
    ld de', bc'             ; 4a77 d9 50 59 d9
    ld de', hl'             ; 4a7b d9 54 5d d9
    ld de, (0x1234)         ; 4a7f ed 5b 34 12
    ld de, (hl)             ; 4a83 5e 23 56 2b
    ld de, (hl+)            ; 4a87 5e 23 56 23
    ld de, (ix)             ; 4a8b dd 5e 00 dd 56 01
    ld de, (ix+0)           ; 4a91 dd 5e 00 dd 56 01
    ld de, (ix+126)         ; 4a97 dd 5e 7e dd 56 7f
    ld de, (ix-128)         ; 4a9d dd 5e 80 dd 56 81
    ld de, (iy)             ; 4aa3 fd 5e 00 fd 56 01
    ld de, (iy+0)           ; 4aa9 fd 5e 00 fd 56 01
    ld de, (iy+126)         ; 4aaf fd 5e 7e fd 56 7f
    ld de, (iy-128)         ; 4ab5 fd 5e 80 fd 56 81
    ld de, 0x1234           ; 4abb 11 34 12
    ld de, bc               ; 4abe 50 59
    ld de, bc'              ; 4ac0 d9 c5 d9 d1
    ld de, de'              ; 4ac4 d9 d5 d9 d1
    ld de, hl               ; 4ac8 54 5d
    ld de, hl'              ; 4aca d9 e5 d9 d1
    ld de, hl+0             ; 4ace e5 11 00 00 19 eb e1
    ld de, hl+127           ; 4ad5 e5 11 7f 00 19 eb e1
    ld de, hl+255           ; 4adc e5 11 ff 00 19 eb e1
    ld de, ix               ; 4ae3 dd 54 dd 5d
    ld de, iy               ; 4ae7 fd 54 fd 5d
    ld de, sp               ; 4aeb eb 21 00 00 39 eb
    ld de, sp+0             ; 4af1 eb 21 00 00 39 eb
    ld de, sp+127           ; 4af7 eb 21 7f 00 39 eb
    ld de, sp+255           ; 4afd eb 21 ff 00 39 eb
    ld e, (de)              ; 4b03 eb 6e eb
    ld e, (de+)             ; 4b06 eb 6e eb 13
    ld e, (de-)             ; 4b0a eb 6e eb 1b
    ld e, (hl)              ; 4b0e 5e
    ld e, (hl+)             ; 4b0f 5e 23
    ld e, (hl-)             ; 4b11 5e 2b
    ld e, (ix)              ; 4b13 dd 5e 00
    ld e, (ix+0)            ; 4b16 dd 5e 00
    ld e, (ix+126)          ; 4b19 dd 5e 7e
    ld e, (ix-128)          ; 4b1c dd 5e 80
    ld e, (iy)              ; 4b1f fd 5e 00
    ld e, (iy+0)            ; 4b22 fd 5e 00
    ld e, (iy+126)          ; 4b25 fd 5e 7e
    ld e, (iy-128)          ; 4b28 fd 5e 80
    ld e, 0                 ; 4b2b 1e 00
    ld e, 127               ; 4b2d 1e 7f
    ld e, 255               ; 4b2f 1e ff
    ld e, a                 ; 4b31 5f
    ld e, b                 ; 4b32 58
    ld e, c                 ; 4b33 59
    ld e, d                 ; 4b34 5a
    ld e, e                 ; 4b35 5b
    ld e, h                 ; 4b36 5c
    ld e, ixh               ; 4b37 dd 5c
    ld e, ixl               ; 4b39 dd 5d
    ld e, iyh               ; 4b3b fd 5c
    ld e, iyl               ; 4b3d fd 5d
    ld e, l                 ; 4b3f 5d
    ld h, (de)              ; 4b40 eb 56 eb
    ld h, (de+)             ; 4b43 eb 56 eb 13
    ld h, (de-)             ; 4b47 eb 56 eb 1b
    ld h, (hl)              ; 4b4b 66
    ld h, (hl+)             ; 4b4c 66 23
    ld h, (hl-)             ; 4b4e 66 2b
    ld h, (ix)              ; 4b50 dd 66 00
    ld h, (ix+0)            ; 4b53 dd 66 00
    ld h, (ix+126)          ; 4b56 dd 66 7e
    ld h, (ix-128)          ; 4b59 dd 66 80
    ld h, (iy)              ; 4b5c fd 66 00
    ld h, (iy+0)            ; 4b5f fd 66 00
    ld h, (iy+126)          ; 4b62 fd 66 7e
    ld h, (iy-128)          ; 4b65 fd 66 80
    ld h, 0                 ; 4b68 26 00
    ld h, 127               ; 4b6a 26 7f
    ld h, 255               ; 4b6c 26 ff
    ld h, a                 ; 4b6e 67
    ld h, b                 ; 4b6f 60
    ld h, c                 ; 4b70 61
    ld h, d                 ; 4b71 62
    ld h, e                 ; 4b72 63
    ld h, h                 ; 4b73 64
    ld h, l                 ; 4b74 65
    ld hl', bc'             ; 4b75 d9 60 69 d9
    ld hl', de'             ; 4b79 d9 62 6b d9
    ld hl, (0x1234)         ; 4b7d 2a 34 12
    ld hl, (de)             ; 4b80 eb 5e 23 56 2b eb
    ld hl, (hl)             ; 4b86 f5 7e 23 66 6f f1
    ld hl, (ix)             ; 4b8c dd 6e 00 dd 66 01
    ld hl, (ix+0)           ; 4b92 dd 6e 00 dd 66 01
    ld hl, (ix+126)         ; 4b98 dd 6e 7e dd 66 7f
    ld hl, (ix-128)         ; 4b9e dd 6e 80 dd 66 81
    ld hl, (iy)             ; 4ba4 fd 6e 00 fd 66 01
    ld hl, (iy+0)           ; 4baa fd 6e 00 fd 66 01
    ld hl, (iy+126)         ; 4bb0 fd 6e 7e fd 66 7f
    ld hl, (iy-128)         ; 4bb6 fd 6e 80 fd 66 81
    ld hl, 0x1234           ; 4bbc 21 34 12
    ld hl, bc               ; 4bbf 60 69
    ld hl, bc'              ; 4bc1 d9 c5 d9 e1
    ld hl, de               ; 4bc5 62 6b
    ld hl, de'              ; 4bc7 d9 d5 d9 e1
    ld hl, hl'              ; 4bcb d9 e5 d9 e1
    ld hl, ix               ; 4bcf dd e5 e1
    ld hl, iy               ; 4bd2 fd e5 e1
    ld hl, sp               ; 4bd5 21 00 00 39
    ld hl, sp+0             ; 4bd9 21 00 00 39
    ld hl, sp+126           ; 4bdd 21 7e 00 39
    ld hl, sp-128           ; 4be1 21 80 ff 39
    ld i, a                 ; 4be5 ed 47
    ld ix, (0x1234)         ; 4be7 dd 2a 34 12
    ld ix, (hl)             ; 4beb d5 5e 23 56 2b d5 dd e1 d1
    ld ix, (hl+)            ; 4bf4 d5 5e 23 56 23 d5 dd e1 d1
    ld ix, (ix)             ; 4bfd dd e5 e3 dd 6e 00 dd 66 01 e3 dd e1
    ld ix, (ix+0)           ; 4c09 dd e5 e3 dd 6e 00 dd 66 01 e3 dd e1
    ld ix, (ix+126)         ; 4c15 dd e5 e3 dd 6e 7e dd 66 7f e3 dd e1
    ld ix, (ix-128)         ; 4c21 dd e5 e3 dd 6e 80 dd 66 81 e3 dd e1
    ld ix, (iy)             ; 4c2d dd e5 e3 fd 6e 00 fd 66 01 e3 dd e1
    ld ix, (iy+0)           ; 4c39 dd e5 e3 fd 6e 00 fd 66 01 e3 dd e1
    ld ix, (iy+126)         ; 4c45 dd e5 e3 fd 6e 7e fd 66 7f e3 dd e1
    ld ix, (iy-128)         ; 4c51 dd e5 e3 fd 6e 80 fd 66 81 e3 dd e1
    ld ix, 0x1234           ; 4c5d dd 21 34 12
    ld ix, bc               ; 4c61 dd 60 dd 69
    ld ix, bc'              ; 4c65 d9 c5 d9 dd e1
    ld ix, de               ; 4c6a dd 62 dd 6b
    ld ix, de'              ; 4c6e d9 d5 d9 dd e1
    ld ix, hl               ; 4c73 e5 dd e1
    ld ix, hl'              ; 4c76 d9 e5 d9 dd e1
    ld ix, iy               ; 4c7b fd e5 dd e1
    ld ixh, 0               ; 4c7f dd 26 00
    ld ixh, 127             ; 4c82 dd 26 7f
    ld ixh, 255             ; 4c85 dd 26 ff
    ld ixh, a               ; 4c88 dd 67
    ld ixh, b               ; 4c8a dd 60
    ld ixh, c               ; 4c8c dd 61
    ld ixh, d               ; 4c8e dd 62
    ld ixh, e               ; 4c90 dd 63
    ld ixh, ixh             ; 4c92 dd 64
    ld ixh, ixl             ; 4c94 dd 65
    ld ixl, 0               ; 4c96 dd 2e 00
    ld ixl, 127             ; 4c99 dd 2e 7f
    ld ixl, 255             ; 4c9c dd 2e ff
    ld ixl, a               ; 4c9f dd 6f
    ld ixl, b               ; 4ca1 dd 68
    ld ixl, c               ; 4ca3 dd 69
    ld ixl, d               ; 4ca5 dd 6a
    ld ixl, e               ; 4ca7 dd 6b
    ld ixl, ixh             ; 4ca9 dd 6c
    ld ixl, ixl             ; 4cab dd 6d
    ld iy, (0x1234)         ; 4cad fd 2a 34 12
    ld iy, (hl)             ; 4cb1 d5 5e 23 56 2b d5 fd e1 d1
    ld iy, (hl+)            ; 4cba d5 5e 23 56 23 d5 fd e1 d1
    ld iy, (ix)             ; 4cc3 fd e5 e3 dd 6e 00 dd 66 01 e3 fd e1
    ld iy, (ix+0)           ; 4ccf fd e5 e3 dd 6e 00 dd 66 01 e3 fd e1
    ld iy, (ix+126)         ; 4cdb fd e5 e3 dd 6e 7e dd 66 7f e3 fd e1
    ld iy, (ix-128)         ; 4ce7 fd e5 e3 dd 6e 80 dd 66 81 e3 fd e1
    ld iy, (iy)             ; 4cf3 fd e5 e3 fd 6e 00 fd 66 01 e3 fd e1
    ld iy, (iy+0)           ; 4cff fd e5 e3 fd 6e 00 fd 66 01 e3 fd e1
    ld iy, (iy+126)         ; 4d0b fd e5 e3 fd 6e 7e fd 66 7f e3 fd e1
    ld iy, (iy-128)         ; 4d17 fd e5 e3 fd 6e 80 fd 66 81 e3 fd e1
    ld iy, 0x1234           ; 4d23 fd 21 34 12
    ld iy, bc               ; 4d27 fd 60 fd 69
    ld iy, bc'              ; 4d2b d9 c5 d9 fd e1
    ld iy, de               ; 4d30 fd 62 fd 6b
    ld iy, de'              ; 4d34 d9 d5 d9 fd e1
    ld iy, hl               ; 4d39 e5 fd e1
    ld iy, hl'              ; 4d3c d9 e5 d9 fd e1
    ld iy, ix               ; 4d41 dd e5 fd e1
    ld iyh, 0               ; 4d45 fd 26 00
    ld iyh, 127             ; 4d48 fd 26 7f
    ld iyh, 255             ; 4d4b fd 26 ff
    ld iyh, a               ; 4d4e fd 67
    ld iyh, b               ; 4d50 fd 60
    ld iyh, c               ; 4d52 fd 61
    ld iyh, d               ; 4d54 fd 62
    ld iyh, e               ; 4d56 fd 63
    ld iyh, iyh             ; 4d58 fd 64
    ld iyh, iyl             ; 4d5a fd 65
    ld iyl, 0               ; 4d5c fd 2e 00
    ld iyl, 127             ; 4d5f fd 2e 7f
    ld iyl, 255             ; 4d62 fd 2e ff
    ld iyl, a               ; 4d65 fd 6f
    ld iyl, b               ; 4d67 fd 68
    ld iyl, c               ; 4d69 fd 69
    ld iyl, d               ; 4d6b fd 6a
    ld iyl, e               ; 4d6d fd 6b
    ld iyl, iyh             ; 4d6f fd 6c
    ld iyl, iyl             ; 4d71 fd 6d
    ld l, (de)              ; 4d73 eb 5e eb
    ld l, (de+)             ; 4d76 eb 5e eb 13
    ld l, (de-)             ; 4d7a eb 5e eb 1b
    ld l, (hl)              ; 4d7e 6e
    ld l, (hl+)             ; 4d7f 6e 23
    ld l, (hl-)             ; 4d81 6e 2b
    ld l, (ix)              ; 4d83 dd 6e 00
    ld l, (ix+0)            ; 4d86 dd 6e 00
    ld l, (ix+126)          ; 4d89 dd 6e 7e
    ld l, (ix-128)          ; 4d8c dd 6e 80
    ld l, (iy)              ; 4d8f fd 6e 00
    ld l, (iy+0)            ; 4d92 fd 6e 00
    ld l, (iy+126)          ; 4d95 fd 6e 7e
    ld l, (iy-128)          ; 4d98 fd 6e 80
    ld l, 0                 ; 4d9b 2e 00
    ld l, 127               ; 4d9d 2e 7f
    ld l, 255               ; 4d9f 2e ff
    ld l, a                 ; 4da1 6f
    ld l, b                 ; 4da2 68
    ld l, c                 ; 4da3 69
    ld l, d                 ; 4da4 6a
    ld l, e                 ; 4da5 6b
    ld l, h                 ; 4da6 6c
    ld l, l                 ; 4da7 6d
    ld r, a                 ; 4da8 ed 4f
    ld sp, (0x1234)         ; 4daa ed 7b 34 12
    ld sp, 0x1234           ; 4dae 31 34 12
    ld sp, hl               ; 4db1 f9
    ld sp, ix               ; 4db2 dd f9
    ld sp, iy               ; 4db4 fd f9
    ldd                     ; 4db6 ed a8
    ldd (bc), a             ; 4db8 02 0b
    ldd (de), 0             ; 4dba eb 36 00 eb 1b
    ldd (de), 127           ; 4dbf eb 36 7f eb 1b
    ldd (de), 255           ; 4dc4 eb 36 ff eb 1b
    ldd (de), a             ; 4dc9 12 1b
    ldd (de), b             ; 4dcb eb 70 eb 1b
    ldd (de), c             ; 4dcf eb 71 eb 1b
    ldd (de), d             ; 4dd3 eb 74 eb 1b
    ldd (de), e             ; 4dd7 eb 75 eb 1b
    ldd (de), h             ; 4ddb eb 72 eb 1b
    ldd (de), l             ; 4ddf eb 73 eb 1b
    ldd (hl), 0             ; 4de3 36 00 2b
    ldd (hl), 127           ; 4de6 36 7f 2b
    ldd (hl), 255           ; 4de9 36 ff 2b
    ldd (hl), a             ; 4dec 77 2b
    ldd (hl), b             ; 4dee 70 2b
    ldd (hl), c             ; 4df0 71 2b
    ldd (hl), d             ; 4df2 72 2b
    ldd (hl), e             ; 4df4 73 2b
    ldd (hl), h             ; 4df6 74 2b
    ldd (hl), l             ; 4df8 75 2b
    ldd a, (bc)             ; 4dfa 0a 0b
    ldd a, (de)             ; 4dfc 1a 1b
    ldd a, (hl)             ; 4dfe 7e 2b
    ldd b, (de)             ; 4e00 eb 46 eb 1b
    ldd b, (hl)             ; 4e04 46 2b
    ldd c, (de)             ; 4e06 eb 4e eb 1b
    ldd c, (hl)             ; 4e0a 4e 2b
    ldd d, (de)             ; 4e0c eb 66 eb 1b
    ldd d, (hl)             ; 4e10 56 2b
    ldd e, (de)             ; 4e12 eb 6e eb 1b
    ldd e, (hl)             ; 4e16 5e 2b
    ldd h, (de)             ; 4e18 eb 56 eb 1b
    ldd h, (hl)             ; 4e1c 66 2b
    ldd l, (de)             ; 4e1e eb 5e eb 1b
    ldd l, (hl)             ; 4e22 6e 2b
    lddr                    ; 4e24 ed b8
    ldi                     ; 4e26 ed a0
    ldi (bc), a             ; 4e28 02 03
    ldi (de), 0             ; 4e2a eb 36 00 eb 13
    ldi (de), 127           ; 4e2f eb 36 7f eb 13
    ldi (de), 255           ; 4e34 eb 36 ff eb 13
    ldi (de), a             ; 4e39 12 13
    ldi (de), b             ; 4e3b eb 70 eb 13
    ldi (de), c             ; 4e3f eb 71 eb 13
    ldi (de), d             ; 4e43 eb 74 eb 13
    ldi (de), e             ; 4e47 eb 75 eb 13
    ldi (de), h             ; 4e4b eb 72 eb 13
    ldi (de), l             ; 4e4f eb 73 eb 13
    ldi (hl), 0             ; 4e53 36 00 23
    ldi (hl), 127           ; 4e56 36 7f 23
    ldi (hl), 255           ; 4e59 36 ff 23
    ldi (hl), a             ; 4e5c 77 23
    ldi (hl), b             ; 4e5e 70 23
    ldi (hl), bc            ; 4e60 71 23 70 23
    ldi (hl), c             ; 4e64 71 23
    ldi (hl), d             ; 4e66 72 23
    ldi (hl), de            ; 4e68 73 23 72 23
    ldi (hl), e             ; 4e6c 73 23
    ldi (hl), h             ; 4e6e 74 23
    ldi (hl), hl            ; 4e70 f5 7c 75 23 77 f1 23
    ldi (hl), ix            ; 4e77 d5 dd e5 d1 73 23 72 23 d1
    ldi (hl), iy            ; 4e80 d5 fd e5 d1 73 23 72 23 d1
    ldi (hl), l             ; 4e89 75 23
    ldi a, (bc)             ; 4e8b 0a 03
    ldi a, (de)             ; 4e8d 1a 13
    ldi a, (hl)             ; 4e8f 7e 23
    ldi b, (de)             ; 4e91 eb 46 eb 13
    ldi b, (hl)             ; 4e95 46 23
    ldi bc, (hl)            ; 4e97 4e 23 46 23
    ldi c, (de)             ; 4e9b eb 4e eb 13
    ldi c, (hl)             ; 4e9f 4e 23
    ldi d, (de)             ; 4ea1 eb 66 eb 13
    ldi d, (hl)             ; 4ea5 56 23
    ldi de, (hl)            ; 4ea7 5e 23 56 23
    ldi e, (de)             ; 4eab eb 6e eb 13
    ldi e, (hl)             ; 4eaf 5e 23
    ldi h, (de)             ; 4eb1 eb 56 eb 13
    ldi h, (hl)             ; 4eb5 66 23
    ldi ix, (hl)            ; 4eb7 d5 5e 23 56 23 d5 dd e1 d1
    ldi iy, (hl)            ; 4ec0 d5 5e 23 56 23 d5 fd e1 d1
    ldi l, (de)             ; 4ec9 eb 5e eb 13
    ldi l, (hl)             ; 4ecd 6e 23
    ldir                    ; 4ecf ed b0
    neg                     ; 4ed1 ed 44
    neg a                   ; 4ed3 ed 44
    neg b                   ; 4ed5 f5 78 2f 47 04 f1
    neg bc                  ; 4edb f5 78 2f 47 79 2f 4f 03 f1
    neg c                   ; 4ee4 f5 79 2f 4f 0c f1
    neg d                   ; 4eea f5 7a 2f 57 14 f1
    neg de                  ; 4ef0 f5 7a 2f 57 7b 2f 5f 13 f1
    neg e                   ; 4ef9 f5 7b 2f 5f 1c f1
    neg h                   ; 4eff f5 7c 2f 67 24 f1
    neg hl                  ; 4f05 f5 7c 2f 67 7d 2f 6f 23 f1
    neg ix                  ; 4f0e f5 dd 7c 2f dd 67 dd 7d 2f dd 6f dd 23 f1
    neg iy                  ; 4f1c f5 fd 7c 2f fd 67 fd 7d 2f fd 6f fd 23 f1
    neg l                   ; 4f2a f5 7d 2f 6f 2c f1
    nop                     ; 4f30 00
    or (hl)                 ; 4f31 b6
    or (hl+)                ; 4f32 b6 23
    or (hl-)                ; 4f34 b6 2b
    or (ix)                 ; 4f36 dd b6 00
    or (ix+0)               ; 4f39 dd b6 00
    or (ix+126)             ; 4f3c dd b6 7e
    or (ix-128)             ; 4f3f dd b6 80
    or (iy)                 ; 4f42 fd b6 00
    or (iy+0)               ; 4f45 fd b6 00
    or (iy+126)             ; 4f48 fd b6 7e
    or (iy-128)             ; 4f4b fd b6 80
    or 0                    ; 4f4e f6 00
    or 127                  ; 4f50 f6 7f
    or 255                  ; 4f52 f6 ff
    or a                    ; 4f54 b7
    or a, (hl)              ; 4f55 b6
    or a, (hl+)             ; 4f56 b6 23
    or a, (hl-)             ; 4f58 b6 2b
    or a, (ix)              ; 4f5a dd b6 00
    or a, (ix+0)            ; 4f5d dd b6 00
    or a, (ix+126)          ; 4f60 dd b6 7e
    or a, (ix-128)          ; 4f63 dd b6 80
    or a, (iy)              ; 4f66 fd b6 00
    or a, (iy+0)            ; 4f69 fd b6 00
    or a, (iy+126)          ; 4f6c fd b6 7e
    or a, (iy-128)          ; 4f6f fd b6 80
    or a, 0                 ; 4f72 f6 00
    or a, 127               ; 4f74 f6 7f
    or a, 255               ; 4f76 f6 ff
    or a, a                 ; 4f78 b7
    or a, b                 ; 4f79 b0
    or a, c                 ; 4f7a b1
    or a, d                 ; 4f7b b2
    or a, e                 ; 4f7c b3
    or a, h                 ; 4f7d b4
    or a, ixh               ; 4f7e dd b4
    or a, ixl               ; 4f80 dd b5
    or a, iyh               ; 4f82 fd b4
    or a, iyl               ; 4f84 fd b5
    or a, l                 ; 4f86 b5
    or b                    ; 4f87 b0
    or c                    ; 4f88 b1
    or d                    ; 4f89 b2
    or e                    ; 4f8a b3
    or h                    ; 4f8b b4
    or hl, bc               ; 4f8c f5 7c b0 67 7d b1 6f f1
    or hl, de               ; 4f94 f5 7c b2 67 7d b3 6f f1
    or ix, bc               ; 4f9c f5 dd 7c b0 dd 67 dd 7d b1 dd 6f f1
    or ix, de               ; 4fa8 f5 dd 7c b2 dd 67 dd 7d b3 dd 6f f1
    or ixh                  ; 4fb4 dd b4
    or ixl                  ; 4fb6 dd b5
    or iy, bc               ; 4fb8 f5 fd 7c b0 fd 67 fd 7d b1 fd 6f f1
    or iy, de               ; 4fc4 f5 fd 7c b2 fd 67 fd 7d b3 fd 6f f1
    or iyh                  ; 4fd0 fd b4
    or iyl                  ; 4fd2 fd b5
    or l                    ; 4fd4 b5
    otdr                    ; 4fd5 ed bb
    otir                    ; 4fd7 ed b3
    out (0), a              ; 4fd9 d3 00
    out (127), a            ; 4fdb d3 7f
    out (255), a            ; 4fdd d3 ff
    out (c), a              ; 4fdf ed 79
    out (c), b              ; 4fe1 ed 41
    out (c), c              ; 4fe3 ed 49
    out (c), d              ; 4fe5 ed 51
    out (c), e              ; 4fe7 ed 59
    out (c), h              ; 4fe9 ed 61
    out (c), l              ; 4feb ed 69
    outd                    ; 4fed ed ab
    outi                    ; 4fef ed a3
    pop af                  ; 4ff1 f1
    pop bc                  ; 4ff2 c1
    pop de                  ; 4ff3 d1
    pop hl                  ; 4ff4 e1
    pop ix                  ; 4ff5 dd e1
    pop iy                  ; 4ff7 fd e1
    push af                 ; 4ff9 f5
    push bc                 ; 4ffa c5
    push bc'                ; 4ffb d9 c5 d9
    push de                 ; 4ffe d5
    push de'                ; 4fff d9 d5 d9
    push hl                 ; 5002 e5
    push hl'                ; 5003 d9 e5 d9
    push ix                 ; 5006 dd e5
    push iy                 ; 5008 fd e5
    res 0, (hl)             ; 500a cb 86
    res 0, (ix)             ; 500c dd cb 00 86
    res 0, (ix+0)           ; 5010 dd cb 00 86
    res 0, (ix+126)         ; 5014 dd cb 7e 86
    res 0, (ix-128)         ; 5018 dd cb 80 86
    res 0, (iy)             ; 501c fd cb 00 86
    res 0, (iy+0)           ; 5020 fd cb 00 86
    res 0, (iy+126)         ; 5024 fd cb 7e 86
    res 0, (iy-128)         ; 5028 fd cb 80 86
    res 0, a                ; 502c cb 87
    res 0, b                ; 502e cb 80
    res 0, c                ; 5030 cb 81
    res 0, d                ; 5032 cb 82
    res 0, e                ; 5034 cb 83
    res 0, h                ; 5036 cb 84
    res 0, l                ; 5038 cb 85
    res 1, (hl)             ; 503a cb 8e
    res 1, (ix)             ; 503c dd cb 00 8e
    res 1, (ix+0)           ; 5040 dd cb 00 8e
    res 1, (ix+126)         ; 5044 dd cb 7e 8e
    res 1, (ix-128)         ; 5048 dd cb 80 8e
    res 1, (iy)             ; 504c fd cb 00 8e
    res 1, (iy+0)           ; 5050 fd cb 00 8e
    res 1, (iy+126)         ; 5054 fd cb 7e 8e
    res 1, (iy-128)         ; 5058 fd cb 80 8e
    res 1, a                ; 505c cb 8f
    res 1, b                ; 505e cb 88
    res 1, c                ; 5060 cb 89
    res 1, d                ; 5062 cb 8a
    res 1, e                ; 5064 cb 8b
    res 1, h                ; 5066 cb 8c
    res 1, l                ; 5068 cb 8d
    res 2, (hl)             ; 506a cb 96
    res 2, (ix)             ; 506c dd cb 00 96
    res 2, (ix+0)           ; 5070 dd cb 00 96
    res 2, (ix+126)         ; 5074 dd cb 7e 96
    res 2, (ix-128)         ; 5078 dd cb 80 96
    res 2, (iy)             ; 507c fd cb 00 96
    res 2, (iy+0)           ; 5080 fd cb 00 96
    res 2, (iy+126)         ; 5084 fd cb 7e 96
    res 2, (iy-128)         ; 5088 fd cb 80 96
    res 2, a                ; 508c cb 97
    res 2, b                ; 508e cb 90
    res 2, c                ; 5090 cb 91
    res 2, d                ; 5092 cb 92
    res 2, e                ; 5094 cb 93
    res 2, h                ; 5096 cb 94
    res 2, l                ; 5098 cb 95
    res 3, (hl)             ; 509a cb 9e
    res 3, (ix)             ; 509c dd cb 00 9e
    res 3, (ix+0)           ; 50a0 dd cb 00 9e
    res 3, (ix+126)         ; 50a4 dd cb 7e 9e
    res 3, (ix-128)         ; 50a8 dd cb 80 9e
    res 3, (iy)             ; 50ac fd cb 00 9e
    res 3, (iy+0)           ; 50b0 fd cb 00 9e
    res 3, (iy+126)         ; 50b4 fd cb 7e 9e
    res 3, (iy-128)         ; 50b8 fd cb 80 9e
    res 3, a                ; 50bc cb 9f
    res 3, b                ; 50be cb 98
    res 3, c                ; 50c0 cb 99
    res 3, d                ; 50c2 cb 9a
    res 3, e                ; 50c4 cb 9b
    res 3, h                ; 50c6 cb 9c
    res 3, l                ; 50c8 cb 9d
    res 4, (hl)             ; 50ca cb a6
    res 4, (ix)             ; 50cc dd cb 00 a6
    res 4, (ix+0)           ; 50d0 dd cb 00 a6
    res 4, (ix+126)         ; 50d4 dd cb 7e a6
    res 4, (ix-128)         ; 50d8 dd cb 80 a6
    res 4, (iy)             ; 50dc fd cb 00 a6
    res 4, (iy+0)           ; 50e0 fd cb 00 a6
    res 4, (iy+126)         ; 50e4 fd cb 7e a6
    res 4, (iy-128)         ; 50e8 fd cb 80 a6
    res 4, a                ; 50ec cb a7
    res 4, b                ; 50ee cb a0
    res 4, c                ; 50f0 cb a1
    res 4, d                ; 50f2 cb a2
    res 4, e                ; 50f4 cb a3
    res 4, h                ; 50f6 cb a4
    res 4, l                ; 50f8 cb a5
    res 5, (hl)             ; 50fa cb ae
    res 5, (ix)             ; 50fc dd cb 00 ae
    res 5, (ix+0)           ; 5100 dd cb 00 ae
    res 5, (ix+126)         ; 5104 dd cb 7e ae
    res 5, (ix-128)         ; 5108 dd cb 80 ae
    res 5, (iy)             ; 510c fd cb 00 ae
    res 5, (iy+0)           ; 5110 fd cb 00 ae
    res 5, (iy+126)         ; 5114 fd cb 7e ae
    res 5, (iy-128)         ; 5118 fd cb 80 ae
    res 5, a                ; 511c cb af
    res 5, b                ; 511e cb a8
    res 5, c                ; 5120 cb a9
    res 5, d                ; 5122 cb aa
    res 5, e                ; 5124 cb ab
    res 5, h                ; 5126 cb ac
    res 5, l                ; 5128 cb ad
    res 6, (hl)             ; 512a cb b6
    res 6, (ix)             ; 512c dd cb 00 b6
    res 6, (ix+0)           ; 5130 dd cb 00 b6
    res 6, (ix+126)         ; 5134 dd cb 7e b6
    res 6, (ix-128)         ; 5138 dd cb 80 b6
    res 6, (iy)             ; 513c fd cb 00 b6
    res 6, (iy+0)           ; 5140 fd cb 00 b6
    res 6, (iy+126)         ; 5144 fd cb 7e b6
    res 6, (iy-128)         ; 5148 fd cb 80 b6
    res 6, a                ; 514c cb b7
    res 6, b                ; 514e cb b0
    res 6, c                ; 5150 cb b1
    res 6, d                ; 5152 cb b2
    res 6, e                ; 5154 cb b3
    res 6, h                ; 5156 cb b4
    res 6, l                ; 5158 cb b5
    res 7, (hl)             ; 515a cb be
    res 7, (ix)             ; 515c dd cb 00 be
    res 7, (ix+0)           ; 5160 dd cb 00 be
    res 7, (ix+126)         ; 5164 dd cb 7e be
    res 7, (ix-128)         ; 5168 dd cb 80 be
    res 7, (iy)             ; 516c fd cb 00 be
    res 7, (iy+0)           ; 5170 fd cb 00 be
    res 7, (iy+126)         ; 5174 fd cb 7e be
    res 7, (iy-128)         ; 5178 fd cb 80 be
    res 7, a                ; 517c cb bf
    res 7, b                ; 517e cb b8
    res 7, c                ; 5180 cb b9
    res 7, d                ; 5182 cb ba
    res 7, e                ; 5184 cb bb
    res 7, h                ; 5186 cb bc
    res 7, l                ; 5188 cb bd
    ret                     ; 518a c9
    ret c                   ; 518b d8
    ret eq                  ; 518c c8
    ret geu                 ; 518d d0
    ret gtu                 ; 518e 28 01 d0
    ret leu                 ; 5191 c8 d8
    ret ltu                 ; 5193 d8
    ret m                   ; 5194 f8
    ret nc                  ; 5195 d0
    ret ne                  ; 5196 c0
    ret nv                  ; 5197 e0
    ret nz                  ; 5198 c0
    ret p                   ; 5199 f0
    ret pe                  ; 519a e8
    ret po                  ; 519b e0
    ret v                   ; 519c e8
    ret z                   ; 519d c8
    reti                    ; 519e ed 4d
    retn                    ; 51a0 ed 45
    rl (hl)                 ; 51a2 cb 16
    rl (ix)                 ; 51a4 dd cb 00 16
    rl (ix+0)               ; 51a8 dd cb 00 16
    rl (ix+126)             ; 51ac dd cb 7e 16
    rl (ix-128)             ; 51b0 dd cb 80 16
    rl (iy)                 ; 51b4 fd cb 00 16
    rl (iy+0)               ; 51b8 fd cb 00 16
    rl (iy+126)             ; 51bc fd cb 7e 16
    rl (iy-128)             ; 51c0 fd cb 80 16
    rl a                    ; 51c4 cb 17
    rl b                    ; 51c6 cb 10
    rl bc                   ; 51c8 cb 11 cb 10
    rl c                    ; 51cc cb 11
    rl d                    ; 51ce cb 12
    rl de                   ; 51d0 cb 13 cb 12
    rl e                    ; 51d4 cb 13
    rl h                    ; 51d6 cb 14
    rl hl                   ; 51d8 cb 15 cb 14
    rl l                    ; 51dc cb 15
    rla                     ; 51de 17
    rlc (hl)                ; 51df cb 06
    rlc (ix)                ; 51e1 dd cb 00 06
    rlc (ix+0)              ; 51e5 dd cb 00 06
    rlc (ix+126)            ; 51e9 dd cb 7e 06
    rlc (ix-128)            ; 51ed dd cb 80 06
    rlc (iy)                ; 51f1 fd cb 00 06
    rlc (iy+0)              ; 51f5 fd cb 00 06
    rlc (iy+126)            ; 51f9 fd cb 7e 06
    rlc (iy-128)            ; 51fd fd cb 80 06
    rlc a                   ; 5201 cb 07
    rlc b                   ; 5203 cb 00
    rlc c                   ; 5205 cb 01
    rlc d                   ; 5207 cb 02
    rlc e                   ; 5209 cb 03
    rlc h                   ; 520b cb 04
    rlc l                   ; 520d cb 05
    rlca                    ; 520f 07
    rld                     ; 5210 ed 6f
    rr (hl)                 ; 5212 cb 1e
    rr (ix)                 ; 5214 dd cb 00 1e
    rr (ix+0)               ; 5218 dd cb 00 1e
    rr (ix+126)             ; 521c dd cb 7e 1e
    rr (ix-128)             ; 5220 dd cb 80 1e
    rr (iy)                 ; 5224 fd cb 00 1e
    rr (iy+0)               ; 5228 fd cb 00 1e
    rr (iy+126)             ; 522c fd cb 7e 1e
    rr (iy-128)             ; 5230 fd cb 80 1e
    rr a                    ; 5234 cb 1f
    rr b                    ; 5236 cb 18
    rr bc                   ; 5238 cb 18 cb 19
    rr c                    ; 523c cb 19
    rr d                    ; 523e cb 1a
    rr de                   ; 5240 cb 1a cb 1b
    rr e                    ; 5244 cb 1b
    rr h                    ; 5246 cb 1c
    rr hl                   ; 5248 cb 1c cb 1d
    rr l                    ; 524c cb 1d
    rra                     ; 524e 1f
    rrc (hl)                ; 524f cb 0e
    rrc (ix)                ; 5251 dd cb 00 0e
    rrc (ix+0)              ; 5255 dd cb 00 0e
    rrc (ix+126)            ; 5259 dd cb 7e 0e
    rrc (ix-128)            ; 525d dd cb 80 0e
    rrc (iy)                ; 5261 fd cb 00 0e
    rrc (iy+0)              ; 5265 fd cb 00 0e
    rrc (iy+126)            ; 5269 fd cb 7e 0e
    rrc (iy-128)            ; 526d fd cb 80 0e
    rrc a                   ; 5271 cb 0f
    rrc b                   ; 5273 cb 08
    rrc c                   ; 5275 cb 09
    rrc d                   ; 5277 cb 0a
    rrc e                   ; 5279 cb 0b
    rrc h                   ; 527b cb 0c
    rrc l                   ; 527d cb 0d
    rrca                    ; 527f 0f
    rrd                     ; 5280 ed 67
    rst 0                   ; 5282 c7
    rst 16                  ; 5283 d7
    rst 24                  ; 5284 df
    rst 32                  ; 5285 e7
    rst 40                  ; 5286 ef
    rst 48                  ; 5287 f7
    rst 56                  ; 5288 ff
    rst 8                   ; 5289 cf
    sbc (hl)                ; 528a 9e
    sbc (hl+)               ; 528b 9e 23
    sbc (hl-)               ; 528d 9e 2b
    sbc (ix)                ; 528f dd 9e 00
    sbc (ix+0)              ; 5292 dd 9e 00
    sbc (ix+126)            ; 5295 dd 9e 7e
    sbc (ix-128)            ; 5298 dd 9e 80
    sbc (iy)                ; 529b fd 9e 00
    sbc (iy+0)              ; 529e fd 9e 00
    sbc (iy+126)            ; 52a1 fd 9e 7e
    sbc (iy-128)            ; 52a4 fd 9e 80
    sbc 0                   ; 52a7 de 00
    sbc 127                 ; 52a9 de 7f
    sbc 255                 ; 52ab de ff
    sbc a                   ; 52ad 9f
    sbc a, (hl)             ; 52ae 9e
    sbc a, (hl+)            ; 52af 9e 23
    sbc a, (hl-)            ; 52b1 9e 2b
    sbc a, (ix)             ; 52b3 dd 9e 00
    sbc a, (ix+0)           ; 52b6 dd 9e 00
    sbc a, (ix+126)         ; 52b9 dd 9e 7e
    sbc a, (ix-128)         ; 52bc dd 9e 80
    sbc a, (iy)             ; 52bf fd 9e 00
    sbc a, (iy+0)           ; 52c2 fd 9e 00
    sbc a, (iy+126)         ; 52c5 fd 9e 7e
    sbc a, (iy-128)         ; 52c8 fd 9e 80
    sbc a, 0                ; 52cb de 00
    sbc a, 127              ; 52cd de 7f
    sbc a, 255              ; 52cf de ff
    sbc a, a                ; 52d1 9f
    sbc a, b                ; 52d2 98
    sbc a, c                ; 52d3 99
    sbc a, d                ; 52d4 9a
    sbc a, e                ; 52d5 9b
    sbc a, h                ; 52d6 9c
    sbc a, ixh              ; 52d7 dd 9c
    sbc a, ixl              ; 52d9 dd 9d
    sbc a, iyh              ; 52db fd 9c
    sbc a, iyl              ; 52dd fd 9d
    sbc a, l                ; 52df 9d
    sbc b                   ; 52e0 98
    sbc c                   ; 52e1 99
    sbc d                   ; 52e2 9a
    sbc e                   ; 52e3 9b
    sbc h                   ; 52e4 9c
    sbc hl, bc              ; 52e5 ed 42
    sbc hl, de              ; 52e7 ed 52
    sbc hl, hl              ; 52e9 ed 62
    sbc hl, sp              ; 52eb ed 72
    sbc ixh                 ; 52ed dd 9c
    sbc ixl                 ; 52ef dd 9d
    sbc iyh                 ; 52f1 fd 9c
    sbc iyl                 ; 52f3 fd 9d
    sbc l                   ; 52f5 9d
    scf                     ; 52f6 37
    set 0, (hl)             ; 52f7 cb c6
    set 0, (ix)             ; 52f9 dd cb 00 c6
    set 0, (ix+0)           ; 52fd dd cb 00 c6
    set 0, (ix+126)         ; 5301 dd cb 7e c6
    set 0, (ix-128)         ; 5305 dd cb 80 c6
    set 0, (iy)             ; 5309 fd cb 00 c6
    set 0, (iy+0)           ; 530d fd cb 00 c6
    set 0, (iy+126)         ; 5311 fd cb 7e c6
    set 0, (iy-128)         ; 5315 fd cb 80 c6
    set 0, a                ; 5319 cb c7
    set 0, b                ; 531b cb c0
    set 0, c                ; 531d cb c1
    set 0, d                ; 531f cb c2
    set 0, e                ; 5321 cb c3
    set 0, h                ; 5323 cb c4
    set 0, l                ; 5325 cb c5
    set 1, (hl)             ; 5327 cb ce
    set 1, (ix)             ; 5329 dd cb 00 ce
    set 1, (ix+0)           ; 532d dd cb 00 ce
    set 1, (ix+126)         ; 5331 dd cb 7e ce
    set 1, (ix-128)         ; 5335 dd cb 80 ce
    set 1, (iy)             ; 5339 fd cb 00 ce
    set 1, (iy+0)           ; 533d fd cb 00 ce
    set 1, (iy+126)         ; 5341 fd cb 7e ce
    set 1, (iy-128)         ; 5345 fd cb 80 ce
    set 1, a                ; 5349 cb cf
    set 1, b                ; 534b cb c8
    set 1, c                ; 534d cb c9
    set 1, d                ; 534f cb ca
    set 1, e                ; 5351 cb cb
    set 1, h                ; 5353 cb cc
    set 1, l                ; 5355 cb cd
    set 2, (hl)             ; 5357 cb d6
    set 2, (ix)             ; 5359 dd cb 00 d6
    set 2, (ix+0)           ; 535d dd cb 00 d6
    set 2, (ix+126)         ; 5361 dd cb 7e d6
    set 2, (ix-128)         ; 5365 dd cb 80 d6
    set 2, (iy)             ; 5369 fd cb 00 d6
    set 2, (iy+0)           ; 536d fd cb 00 d6
    set 2, (iy+126)         ; 5371 fd cb 7e d6
    set 2, (iy-128)         ; 5375 fd cb 80 d6
    set 2, a                ; 5379 cb d7
    set 2, b                ; 537b cb d0
    set 2, c                ; 537d cb d1
    set 2, d                ; 537f cb d2
    set 2, e                ; 5381 cb d3
    set 2, h                ; 5383 cb d4
    set 2, l                ; 5385 cb d5
    set 3, (hl)             ; 5387 cb de
    set 3, (ix)             ; 5389 dd cb 00 de
    set 3, (ix+0)           ; 538d dd cb 00 de
    set 3, (ix+126)         ; 5391 dd cb 7e de
    set 3, (ix-128)         ; 5395 dd cb 80 de
    set 3, (iy)             ; 5399 fd cb 00 de
    set 3, (iy+0)           ; 539d fd cb 00 de
    set 3, (iy+126)         ; 53a1 fd cb 7e de
    set 3, (iy-128)         ; 53a5 fd cb 80 de
    set 3, a                ; 53a9 cb df
    set 3, b                ; 53ab cb d8
    set 3, c                ; 53ad cb d9
    set 3, d                ; 53af cb da
    set 3, e                ; 53b1 cb db
    set 3, h                ; 53b3 cb dc
    set 3, l                ; 53b5 cb dd
    set 4, (hl)             ; 53b7 cb e6
    set 4, (ix)             ; 53b9 dd cb 00 e6
    set 4, (ix+0)           ; 53bd dd cb 00 e6
    set 4, (ix+126)         ; 53c1 dd cb 7e e6
    set 4, (ix-128)         ; 53c5 dd cb 80 e6
    set 4, (iy)             ; 53c9 fd cb 00 e6
    set 4, (iy+0)           ; 53cd fd cb 00 e6
    set 4, (iy+126)         ; 53d1 fd cb 7e e6
    set 4, (iy-128)         ; 53d5 fd cb 80 e6
    set 4, a                ; 53d9 cb e7
    set 4, b                ; 53db cb e0
    set 4, c                ; 53dd cb e1
    set 4, d                ; 53df cb e2
    set 4, e                ; 53e1 cb e3
    set 4, h                ; 53e3 cb e4
    set 4, l                ; 53e5 cb e5
    set 5, (hl)             ; 53e7 cb ee
    set 5, (ix)             ; 53e9 dd cb 00 ee
    set 5, (ix+0)           ; 53ed dd cb 00 ee
    set 5, (ix+126)         ; 53f1 dd cb 7e ee
    set 5, (ix-128)         ; 53f5 dd cb 80 ee
    set 5, (iy)             ; 53f9 fd cb 00 ee
    set 5, (iy+0)           ; 53fd fd cb 00 ee
    set 5, (iy+126)         ; 5401 fd cb 7e ee
    set 5, (iy-128)         ; 5405 fd cb 80 ee
    set 5, a                ; 5409 cb ef
    set 5, b                ; 540b cb e8
    set 5, c                ; 540d cb e9
    set 5, d                ; 540f cb ea
    set 5, e                ; 5411 cb eb
    set 5, h                ; 5413 cb ec
    set 5, l                ; 5415 cb ed
    set 6, (hl)             ; 5417 cb f6
    set 6, (ix)             ; 5419 dd cb 00 f6
    set 6, (ix+0)           ; 541d dd cb 00 f6
    set 6, (ix+126)         ; 5421 dd cb 7e f6
    set 6, (ix-128)         ; 5425 dd cb 80 f6
    set 6, (iy)             ; 5429 fd cb 00 f6
    set 6, (iy+0)           ; 542d fd cb 00 f6
    set 6, (iy+126)         ; 5431 fd cb 7e f6
    set 6, (iy-128)         ; 5435 fd cb 80 f6
    set 6, a                ; 5439 cb f7
    set 6, b                ; 543b cb f0
    set 6, c                ; 543d cb f1
    set 6, d                ; 543f cb f2
    set 6, e                ; 5441 cb f3
    set 6, h                ; 5443 cb f4
    set 6, l                ; 5445 cb f5
    set 7, (hl)             ; 5447 cb fe
    set 7, (ix)             ; 5449 dd cb 00 fe
    set 7, (ix+0)           ; 544d dd cb 00 fe
    set 7, (ix+126)         ; 5451 dd cb 7e fe
    set 7, (ix-128)         ; 5455 dd cb 80 fe
    set 7, (iy)             ; 5459 fd cb 00 fe
    set 7, (iy+0)           ; 545d fd cb 00 fe
    set 7, (iy+126)         ; 5461 fd cb 7e fe
    set 7, (iy-128)         ; 5465 fd cb 80 fe
    set 7, a                ; 5469 cb ff
    set 7, b                ; 546b cb f8
    set 7, c                ; 546d cb f9
    set 7, d                ; 546f cb fa
    set 7, e                ; 5471 cb fb
    set 7, h                ; 5473 cb fc
    set 7, l                ; 5475 cb fd
    sla (hl)                ; 5477 cb 26
    sla (ix)                ; 5479 dd cb 00 26
    sla (ix+0)              ; 547d dd cb 00 26
    sla (ix+126)            ; 5481 dd cb 7e 26
    sla (ix-128)            ; 5485 dd cb 80 26
    sla (iy)                ; 5489 fd cb 00 26
    sla (iy+0)              ; 548d fd cb 00 26
    sla (iy+126)            ; 5491 fd cb 7e 26
    sla (iy-128)            ; 5495 fd cb 80 26
    sla a                   ; 5499 cb 27
    sla b                   ; 549b cb 20
    sla c                   ; 549d cb 21
    sla d                   ; 549f cb 22
    sla e                   ; 54a1 cb 23
    sla h                   ; 54a3 cb 24
    sla l                   ; 54a5 cb 25
    sli (hl)                ; 54a7 cb 36
    sli (ix)                ; 54a9 dd cb 00 36
    sli (ix+0)              ; 54ad dd cb 00 36
    sli (ix+126)            ; 54b1 dd cb 7e 36
    sli (ix-128)            ; 54b5 dd cb 80 36
    sli (iy)                ; 54b9 fd cb 00 36
    sli (iy+0)              ; 54bd fd cb 00 36
    sli (iy+126)            ; 54c1 fd cb 7e 36
    sli (iy-128)            ; 54c5 fd cb 80 36
    sli a                   ; 54c9 cb 37
    sli b                   ; 54cb cb 30
    sli c                   ; 54cd cb 31
    sli d                   ; 54cf cb 32
    sli e                   ; 54d1 cb 33
    sli h                   ; 54d3 cb 34
    sli l                   ; 54d5 cb 35
    sll (hl)                ; 54d7 cb 36
    sll (ix)                ; 54d9 dd cb 00 36
    sll (ix+0)              ; 54dd dd cb 00 36
    sll (ix+126)            ; 54e1 dd cb 7e 36
    sll (ix-128)            ; 54e5 dd cb 80 36
    sll (iy)                ; 54e9 fd cb 00 36
    sll (iy+0)              ; 54ed fd cb 00 36
    sll (iy+126)            ; 54f1 fd cb 7e 36
    sll (iy-128)            ; 54f5 fd cb 80 36
    sll a                   ; 54f9 cb 37
    sll b                   ; 54fb cb 30
    sll c                   ; 54fd cb 31
    sll d                   ; 54ff cb 32
    sll e                   ; 5501 cb 33
    sll h                   ; 5503 cb 34
    sll l                   ; 5505 cb 35
    sls (hl)                ; 5507 cb 36
    sls (ix)                ; 5509 dd cb 00 36
    sls (ix+0)              ; 550d dd cb 00 36
    sls (ix+126)            ; 5511 dd cb 7e 36
    sls (ix-128)            ; 5515 dd cb 80 36
    sls (iy)                ; 5519 fd cb 00 36
    sls (iy+0)              ; 551d fd cb 00 36
    sls (iy+126)            ; 5521 fd cb 7e 36
    sls (iy-128)            ; 5525 fd cb 80 36
    sls a                   ; 5529 cb 37
    sls b                   ; 552b cb 30
    sls c                   ; 552d cb 31
    sls d                   ; 552f cb 32
    sls e                   ; 5531 cb 33
    sls h                   ; 5533 cb 34
    sls l                   ; 5535 cb 35
    sra (hl)                ; 5537 cb 2e
    sra (ix)                ; 5539 dd cb 00 2e
    sra (ix+0)              ; 553d dd cb 00 2e
    sra (ix+126)            ; 5541 dd cb 7e 2e
    sra (ix-128)            ; 5545 dd cb 80 2e
    sra (iy)                ; 5549 fd cb 00 2e
    sra (iy+0)              ; 554d fd cb 00 2e
    sra (iy+126)            ; 5551 fd cb 7e 2e
    sra (iy-128)            ; 5555 fd cb 80 2e
    sra a                   ; 5559 cb 2f
    sra b                   ; 555b cb 28
    sra bc                  ; 555d cb 28 cb 19
    sra c                   ; 5561 cb 29
    sra d                   ; 5563 cb 2a
    sra de                  ; 5565 cb 2a cb 1b
    sra e                   ; 5569 cb 2b
    sra h                   ; 556b cb 2c
    sra hl                  ; 556d cb 2c cb 1d
    sra l                   ; 5571 cb 2d
    srl (hl)                ; 5573 cb 3e
    srl (ix)                ; 5575 dd cb 00 3e
    srl (ix+0)              ; 5579 dd cb 00 3e
    srl (ix+126)            ; 557d dd cb 7e 3e
    srl (ix-128)            ; 5581 dd cb 80 3e
    srl (iy)                ; 5585 fd cb 00 3e
    srl (iy+0)              ; 5589 fd cb 00 3e
    srl (iy+126)            ; 558d fd cb 7e 3e
    srl (iy-128)            ; 5591 fd cb 80 3e
    srl a                   ; 5595 cb 3f
    srl b                   ; 5597 cb 38
    srl c                   ; 5599 cb 39
    srl d                   ; 559b cb 3a
    srl e                   ; 559d cb 3b
    srl h                   ; 559f cb 3c
    srl l                   ; 55a1 cb 3d
    sub (hl)                ; 55a3 96
    sub (hl+)               ; 55a4 96 23
    sub (hl-)               ; 55a6 96 2b
    sub (ix)                ; 55a8 dd 96 00
    sub (ix+0)              ; 55ab dd 96 00
    sub (ix+126)            ; 55ae dd 96 7e
    sub (ix-128)            ; 55b1 dd 96 80
    sub (iy)                ; 55b4 fd 96 00
    sub (iy+0)              ; 55b7 fd 96 00
    sub (iy+126)            ; 55ba fd 96 7e
    sub (iy-128)            ; 55bd fd 96 80
    sub 0                   ; 55c0 d6 00
    sub 127                 ; 55c2 d6 7f
    sub 255                 ; 55c4 d6 ff
    sub a                   ; 55c6 97
    sub a, (hl)             ; 55c7 96
    sub a, (hl+)            ; 55c8 96 23
    sub a, (hl-)            ; 55ca 96 2b
    sub a, (ix)             ; 55cc dd 96 00
    sub a, (ix+0)           ; 55cf dd 96 00
    sub a, (ix+126)         ; 55d2 dd 96 7e
    sub a, (ix-128)         ; 55d5 dd 96 80
    sub a, (iy)             ; 55d8 fd 96 00
    sub a, (iy+0)           ; 55db fd 96 00
    sub a, (iy+126)         ; 55de fd 96 7e
    sub a, (iy-128)         ; 55e1 fd 96 80
    sub a, 0                ; 55e4 d6 00
    sub a, 127              ; 55e6 d6 7f
    sub a, 255              ; 55e8 d6 ff
    sub a, a                ; 55ea 97
    sub a, b                ; 55eb 90
    sub a, c                ; 55ec 91
    sub a, d                ; 55ed 92
    sub a, e                ; 55ee 93
    sub a, h                ; 55ef 94
    sub a, ixh              ; 55f0 dd 94
    sub a, ixl              ; 55f2 dd 95
    sub a, iyh              ; 55f4 fd 94
    sub a, iyl              ; 55f6 fd 95
    sub a, l                ; 55f8 95
    sub b                   ; 55f9 90
    sub c                   ; 55fa 91
    sub d                   ; 55fb 92
    sub e                   ; 55fc 93
    sub h                   ; 55fd 94
    sub ixh                 ; 55fe dd 94
    sub ixl                 ; 5600 dd 95
    sub iyh                 ; 5602 fd 94
    sub iyl                 ; 5604 fd 95
    sub l                   ; 5606 95
    xor (hl)                ; 5607 ae
    xor (hl+)               ; 5608 ae 23
    xor (hl-)               ; 560a ae 2b
    xor (ix)                ; 560c dd ae 00
    xor (ix+0)              ; 560f dd ae 00
    xor (ix+126)            ; 5612 dd ae 7e
    xor (ix-128)            ; 5615 dd ae 80
    xor (iy)                ; 5618 fd ae 00
    xor (iy+0)              ; 561b fd ae 00
    xor (iy+126)            ; 561e fd ae 7e
    xor (iy-128)            ; 5621 fd ae 80
    xor 0                   ; 5624 ee 00
    xor 127                 ; 5626 ee 7f
    xor 255                 ; 5628 ee ff
    xor a                   ; 562a af
    xor a, (hl)             ; 562b ae
    xor a, (hl+)            ; 562c ae 23
    xor a, (hl-)            ; 562e ae 2b
    xor a, (ix)             ; 5630 dd ae 00
    xor a, (ix+0)           ; 5633 dd ae 00
    xor a, (ix+126)         ; 5636 dd ae 7e
    xor a, (ix-128)         ; 5639 dd ae 80
    xor a, (iy)             ; 563c fd ae 00
    xor a, (iy+0)           ; 563f fd ae 00
    xor a, (iy+126)         ; 5642 fd ae 7e
    xor a, (iy-128)         ; 5645 fd ae 80
    xor a, 0                ; 5648 ee 00
    xor a, 127              ; 564a ee 7f
    xor a, 255              ; 564c ee ff
    xor a, a                ; 564e af
    xor a, b                ; 564f a8
    xor a, c                ; 5650 a9
    xor a, d                ; 5651 aa
    xor a, e                ; 5652 ab
    xor a, h                ; 5653 ac
    xor a, ixh              ; 5654 dd ac
    xor a, ixl              ; 5656 dd ad
    xor a, iyh              ; 5658 fd ac
    xor a, iyl              ; 565a fd ad
    xor a, l                ; 565c ad
    xor b                   ; 565d a8
    xor c                   ; 565e a9
    xor d                   ; 565f aa
    xor e                   ; 5660 ab
    xor h                   ; 5661 ac
    xor hl, bc              ; 5662 f5 7c a8 67 7d a9 6f f1
    xor hl, de              ; 566a f5 7c aa 67 7d ab 6f f1
    xor ix, bc              ; 5672 f5 dd 7c a8 dd 67 dd 7d a9 dd 6f f1
    xor ix, de              ; 567e f5 dd 7c aa dd 67 dd 7d ab dd 6f f1
    xor ixh                 ; 568a dd ac
    xor ixl                 ; 568c dd ad
    xor iy, bc              ; 568e f5 fd 7c a8 fd 67 fd 7d a9 fd 6f f1
    xor iy, de              ; 569a f5 fd 7c aa fd 67 fd 7d ab fd 6f f1
    xor iyh                 ; 56a6 fd ac
    xor iyl                 ; 56a8 fd ad
    xor l                   ; 56aa ad
    ret                     ; 56ab c9
#ENDASM
