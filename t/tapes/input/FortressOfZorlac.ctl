# BASIC lines
  30    # --------------------------------------------------------------------------------
  30    # Start of introdution
  30    # --------------------------------------------------------------------------------
  30    L Line_StartIntro
  34    # init torpedo position
  40	# show welcome
 100    # clear ship numbers
 
 140    # --------------------------------------------------------------------------------
 140    # Intro wait key
 140    # --------------------------------------------------------------------------------
 140    L Line_IntroWaitKey
 
 190    # --------------------------------------------------------------------------------
 190    # Start of new game
 190    # --------------------------------------------------------------------------------
 190    L Line_StartGame

 195	# W is the ship number init to 0, incremented below to start at 1
 205	# 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT
 595	# 18146 = 18145+1 = argument-lo of LD DE,NN in FORTRESS_MOVE_COUNT

 260    # --------------------------------------------------------------------------------
 260    # Start of new ship
 260    # --------------------------------------------------------------------------------
 260    L Line_StartNewShip
 270	# if incremented ship is >5, exit
 280	# write ship number in the three ship drawings
 
 310	L Line_StartLevelWaitKey
 340 	L Line_LevelWaitKey

 390    # --------------------------------------------------------------------------------
 390 	# Run the game
 390    # --------------------------------------------------------------------------------

 430    # --------------------------------------------------------------------------------
 430    # Ship was hit
 430    # --------------------------------------------------------------------------------

 475	# init next ship

 520    # --------------------------------------------------------------------------------
 520    # Alien was hit
 520    # --------------------------------------------------------------------------------
 520 	L Line_AlienHit

 640    # --------------------------------------------------------------------------------
 640    # Game Over
 640    # --------------------------------------------------------------------------------
 640    L Line_GameOver
 
1000    # --------------------------------------------------------------------------------
1000    # Show intructions
1000    # --------------------------------------------------------------------------------
1000    L Line_Instructions
1070	L Line_IntructionsWaitKey1
1130	L Line_IntructionsWaitKey2

2000    # --------------------------------------------------------------------------------
2000    # Select skill level
2000    # --------------------------------------------------------------------------------
2000	L Line_SelectLevel
2080	L Line_WaitSkillSelection

2105 	# selected skill is in V

2150	# 5 instructions in score_multiply (18353+1..28353+5) are replaced by 
2150	# NOP (0) or ADD HL,DE (25=$19) depending on the selected level 
2150	# 1 ADDs for level 1, 5 ADDs for level 5

3000    # --------------------------------------------------------------------------------
3000    # Save and autostart routine
3000    # --------------------------------------------------------------------------------

# ASM lines

$4082   # --------------------------------------------------------------------------------
$4082   # Fortress drawing
$4082   # --------------------------------------------------------------------------------

$4082 57 T FORTRESS1
$40BB    T SAVE_FORTRESS1
$40BC 92 T FORTRESS2

$4118   W addr_fortress
$411A   W addr_fortress_save
$411C   B fortress_col
$411D   B fortress_row
$411E   B fortress_rows
$411F   B fortress_cols
$4120   B fortress_blocks

$4121 5 B DEAD01

$4212   C SCR_POS
$4212   # --------------------------------------------------------------------------------
$4212   #  Compute screen position of row-col in BC to HL 
$4212   #  Preserves all other registers
$4212   # --------------------------------------------------------------------------------

$4212   ; save registers
$421C   C SCR_POS_next_row
$4223   C SCR_POS_row_found
$4226   C SCR_POS_next_col
$422D   C SCR_POS_col_found

$4230 2 B DEAD03

$412C   # --------------------------------------------------------------------------------
$412C   # Draw fortress at fortress_row/fortress_col
$412C   # --------------------------------------------------------------------------------
$412C   C DRAW_FORTRESS

$4138   ; 20 rows
$413D   ; 9 columns
$4142   ; 57 blocks
$415B   C rotate_inner_layer

$4269 5 B DEAD04

$418A   C return
$4191   C rotate_outer_layer
$4191   ; point to save block
$4195   ; get block to be saved to A
$4196   ; get address of save
$4199   ; minus 1=last block
$419A   ; BC will be number of blocks
$419C   ; save end block
$419D   ; get number of blocks
$41A0   ; to BC
$41A1   ; A is block to be saved from end
$41A2   ; move all blocks 1 step
$41A4   ; get first address
$41A7   ; and store saved char
$41A8   ; save address of fortress
$41A9   ; coords of fortress to BC
$41AD   ; screen address to HL
$41B0   ; screen address to DE
$41B1   ; HL=blocks address
$41B2   ; save coords
$41B3   ; A=number of rows
$41B6   ; distance between rows
$41B9   C rot_blocks_left_column
$41B9   ; copy one
$41BB   ; restore BC
$41BC   ; count rows
$41BF   ; jump forward if end of rows
$41C2   ; move to next row
$41C6   C rot_block_next1
$41CA   ; BC = number of cols
$41CC   ; copy bottom row
$41CE   ; A = number of rows
$41D4   C rot_blocks_right_column
$41D4   ; copy block
$41D6   ; restore BC
$41D7   ; count rows
$41DA   ; jump forward if finished
$41DE   ; move to previous row
$41E3   C rot_block_next2
$41E3   ; A = number of columns
$41E6   C rot_blocks_top_row
$41E8   ; copy one block
$41ED   ; jump back if not finished
$41F0   ; DE=blocks, HL=screen

$41F2 26 B DEAD02

$4238   C erase_hit_block_from_fortress
$4274   B alien_col
$4275   B alien_row

$4276	B DEAD05

$4277 5 T ALIEN
$427C 5 T
$4281 5 T
$4286 5 T
$428B 5 T
$4290 5 T
$4295 5 T
$429A 5 T
$429F   # --------------------------------------------------------------------------------
$429F   # Draw alien at fortress_row/fortress_col
$429F   # --------------------------------------------------------------------------------
$429F   C DRAW_ALIEN
$42A5   ; fortress column+3 to alien column
$42BE   C store_alien_pos
$42C2   C alien_downwards

$42CF 2 B DEAD06

$42D1   C draw_alien_at_BC
$42D8   ; DE=screen address
$42D9   ; source of alien
$42DC   ; 8 rows
$42DE   C alien_each_row
$42DE   ; 4 cols
$42E1   ; copy one row
$42E6   ; exit afyet 8 rows
$42E8   ; advance to next row
$42EF   B alien_direction

$42F0 4 B DEAD07

$42FA 5 T SHIP1
$42FF 5 T
$4304 2 T
$4306 1 T SHIP1_nr
$4307 2 T
$4309 5 T
$430E 5 T
$4313 5 T SHIP2
$4318 5 T
$431D 2 T
$431F 1 T SHIP2_nr
$4320 2 T
$4322 5 T
$4327 5 T
$432C 5 T SHIP3
$4331 5 T
$4336 2 T
$4338 1 T SHIP3_nr
$4339 2 T
$433B 5 T
$4340 5 T
$4345   B which_ship
$4346   B ship_col
$4347   B ship_row
$4348   # --------------------------------------------------------------------------------
$4348   # Draw ship at row-col ship_row/ship_col
$4348   # --------------------------------------------------------------------------------
$4348   C DRAW_SHIP
$434C   # --------------------------------------------------------------------------------
$434C   # Draw ship at row-col BC
$434C   # --------------------------------------------------------------------------------
$434C   C DRAW_SHIP_BC
$434C   ; HL=screen position                                    
$434F   ; DE=screen position
$4361   C not_ship1
$436F   C not_ship2
$4377   C draw_ship_hl
$4377   # DE is screen address, HL is ship
$4377   ; 5 rows
$4379   C draw_ship_row
$4379   ; 5 columns
$437C   ; copy one row
$4381   ; return when 5 rows are printed
$4383   ; DE+=33-5=28

$438A 5 B DEAD08

$4395 45 B DEAD09

$43C2   # --------------------------------------------------------------------------------
$43C2   # Called in response to key, BC has ship coords
$43C2   # --------------------------------------------------------------------------------
$43C2   C MOVE_SHIP_RIGHT
$43C2   ; A=fortress column
$43C5   ; -8 (5 for ship + 2 for cannons + 1)
$43C8   ; reached limit
$43CD   ; move right
$43D0   # --------------------------------------------------------------------------------
$43D0   # Called in response to key, BC has ship coords
$43D0   # --------------------------------------------------------------------------------
$43D0   C MOVE_SHIP_DOWN
$43D3   ; reached limit
$43D8   ; move down
$43DB   # --------------------------------------------------------------------------------
$43DB   # Called in response to key, BC has ship coords
$43DB   # --------------------------------------------------------------------------------
$43DB   C MOVE_SHIP_UP
$43DE   ; reached limit
$43E3   ; move up
$43E6   # --------------------------------------------------------------------------------
$43E6   # Called in response to key, BC has ship coords
$43E6   # --------------------------------------------------------------------------------
$43E6   C MOVE_SHIP_LEFT
$43E8   ; reached limit
$43ED   ; move right

$43F0 5 B DEAD10
$43FD 2 B DEAD11

$43F5   C save_ship_pos
$4405   # --------------------------------------------------------------------------------
$4405   # Delete ship before moving it
$4405   # --------------------------------------------------------------------------------
$4405   C DELETE_SHIP
$4405   ; save coords
$440A   ; HL=screen address
$440D   ; 5 rows
$440F   ; distance in screen between rows
$4412   C delete_row
$4412   ; 5 cols
$4414   C delete_col
$4414   ; delete char
$4419   ; next row
$441F   ; restore coords

$4421 5 B DEAD12

$442C   B torpedo_running
$442D   B torpedo_col
$442E   B torpedo_row
$442F   W topedo_pos0
$4431   # --------------------------------------------------------------------------------
$4431   # Called in response to key, BC has ship coords
$4431   # --------------------------------------------------------------------------------
$4431   C FIRE_TORPEDO
$4431   ; is a torpedo on the way?
$4436   ; return if yes
$4440   ; torpedo coord is mouth of ship
$4443   ; make a copy of the cooords - why?
$4448   ; HL=screen position
$444B   ; draw torpedo
$444F   ; signal torpedo on the way

$4453 5 B DEAD13

$445E   B WALL_HIT
$445F   B ALIEN_HIT
$4460   # --------------------------------------------------------------------------------
$4460   # moves torpedo if it has been created
$4460   # --------------------------------------------------------------------------------
$4460   C MOVE_TORPEDO
$446C   ; torpedo was not fired, return
$4471   ; HL=torpedo coord
$4476   ; is there a torpedo?
$4477   ; jump forward if not
$4479   ; erase it
$447B   C topedo_erased
$4482   ; torpedo col == fortress col -1
$4484   ; torpedo col >  fortress col -1
$4486   ; move torpedo
$4487   C draw_torpedo_store_pos
$4487   ; draw it
$448A   ; store new coords
$448F   C torpedo_hit
$4491   ; A=fortress-col+4 (alien)
$4494   ; inside alien - stop torpedo
$4497   ; hit space - continue
$449C   ; hit black border
$44A1   ; hit gray border
$44AB   C hit_fortress
$44AC   ; erase block
$44B6   C stop_torpedo

$44BB 9 B DEAD14

$44CA 3 T BLOCK1
$44CA   ; WBB block
$44CD   T save_block1

$44CE 3 B DEAD20

$44D1   # --------------------------------------------------------------------------------
$44D1   # Draw rotating border and ship
$44D1   # --------------------------------------------------------------------------------
$44D1   C INTRO_SCREEN
$44D1   ; row 3, col 1
$44D4   ; HL=screen address
$44D7   ; DE=screen address
$44D8   ; 10 times+1 for one LDI decrementing B
$44DA   C draw_top_bar
$44DD   ; copy block to screen
$44DF   ; three times
$44E3   ; 30 chars copied (LDI decs BC, DJNZ decs B)
$44E5   ; copy 5 blocks of 3 chars
$44E7   C draw_right_bar
$44EC   C each_right_bar_block
$44EC   ; save BC
$44ED   ; add 32 to DE, LDI with add 1 more
$44F1   ; HL+=32
$44F3   ; DE+=32
$44F4   ; DE+=33
$44F6   ; copy 3 blocks
$44F8   ; times 5
$44F9   ; NOTE: not needed
$44FD   ; add 32 to DE
$4502   ; DE+=32
$4503   ; 10 blocks of 3 = 30
$4505   C draw_bottom_bar
$4508   ; copy 3 chars
$4512   ; NOTE: not needed
$4514   ; repeat 10 times
$4516   ; subtract 32 from DE
$451B   ; DE-=32
$451E   ; copy 5 blocks of 3 chars
$4520   C draw_left_bar
$4525   C each_left_bar_block
$4525   ; copy one char
$452A   ; subtract 33 from address
$452F   ; DE-=33
$4534   ; copy 3 blocks
$4539   ; times 5
$453B   # rotate block
$4553   ; line 11, column 13
$455F 5 T ROTATED_SHIP1
$4564 5 T
$4569 5 T
$456E 5 T
$4573 5 T
$4578 5 T ROTATED_SHIP2
$457D 5 T
$4582 5 T
$4587 5 T
$458C 5 T
$4591 5 T ROTATED_SHIP3
$4596 5 T
$459B 5 T
$45A0 5 T
$45A5 5 T
$45AA 5 T ROTATED_SHIP4
$45AF 5 T
$45B4 5 T
$45B9 5 T
$45BE 5 T
$45C3   B rotate_count
$45C4   C ROTATE_SHIP
$45C4   ; 5*5=25 bytes per ship
$45C7   ; counts 1 to 4
$45D1   C store_count
$45D4   ; address of ship1-25 bytes
$45D8   C add_25
$45DB   ; save source address of ship
$45DC   ; get ship position
$45E0   ; HL = screen position
$45E3   ; DE = screen position
$45E4   ; HL = source of rotated ship

$45E8 5 B DEAD15

$45F3 17 T torpedo_path
$4604   B intro_torpedo_pos_lo
$4605   B intro_torpedo_pos_hi
$4606   # --------------------------------------------------------------------------------
$4606   # Fire intro torpedo
$4606   # --------------------------------------------------------------------------------
$4606   C INTRO_FIRE_TORPEDO
$4609   ; move back one pos
$4612   ; reached start, go back to end
$4617   C restore_torpedo_pos
$4618   C draw_torpedo_path
$461C   ; row 13, col 18
$461F   ; screen pos in HL
$4623   ; DE=screen, HL=torpedo path
$4624   ; 1 torpedo and 7 blanks
$4627   ; draw
$4630   # --------------------------------------------------------------------------------
$4630   # Draw the 4 cannons in front of the fortress
$4630   # --------------------------------------------------------------------------------
$4630   C DRAW_CANONS
$4633   ; fortress column - 2
$4636   ; row 5
$4647   C draw_cannon
$4647   ; draw cannon at roc-col BC
$464A   ; top part of cannon
$464F   ; add 33
$4650   ; bottom part of cannon
$4652   ; leave 4 rows interval

$4657 3 B DEAD16

$4660   B bomb_col
$4661   B bomb_row
$4662   B bomb_running
$4663   # --------------------------------------------------------------------------------
$4663   # select bomb position from fortress and ship position
$4663   # --------------------------------------------------------------------------------
$4663   C FIRE_BOMB
$4668   ; exit if bomb is still running
$466E   ; L=fortress-col - 3
$4672   ; H=ship row
$4678   ; fire from row 5
$467C   C not_cannon1
$4681   ; fire from row 9
$4685   C not_cannon2
$468A   ; fire from row 13
$468E   C not_cannon3
$468E   ; fire from row 17
$4690   C do_fire_bomb
$469F   B SHIP_HIT
$46A0   C DRAW_BOMB
$46A1   ; clear ship-hit
$46A4   ; get position of bomb
$46A8   ; HL = screen address of bomb
$46AB   ; delete bomb
$46AE   ; compare column with zero
$46AF   ; jump forward if not at col zero
$46B1   ; at col zero, clear bomb-running
$46B5   C move_bomb
$46B5   ; move bomb left in column
$46B6   ; and screen address
$46B7   ; did we hit something?
$46B8   ; jump forward if not
$46BC   ; did we hit the torpedo?
$46BD   ; jump if not, we hit the ship
$46BF   C bomb_no_hit
$46BF   ; draw bomb
$46C6   C bomb_hit_ship
$46C6   ; show rubish on hit-point
$46C7   ; NOTE: should not be here
$46C7   ; writes a $34 on the code address, which is inc (hl)

$46D0 5 B DEAD17

$46DB   W timer_fortress_move
$46DB   ; count times until fortress moves
$46DD   # --------------------------------------------------------------------------------
$46DD   # advance timer and move fortress left if it reaches $C8
$46DD   # --------------------------------------------------------------------------------
$46DD   C CHECK_FORTRESS_MOVE
$46E1   C FORTRESS_MOVE_COUNT
$46E1	; value is POKEd from BASIC
$46ED   ; not time yet to move
$46EE   C do_fortress_move
$46F1   ; reset timer
$46FB   ; move fortress one position left
$4702   ; minus 2=cannons column
$4705   ; 20 lines
$4707   C move_next_row
$4707   ; 13 chars = fortress+cannons
$470A   ; destintion
$470C   ; source=destination+1
$470D   ; move row
$4710   ; delete char to the right
$4712   ; move to next line
$4719   ; next row
$4722   B SCORE_lo
$4723   B SCORE_hi
$4724 5 T NUM_BUFFER
$4729   W N10000
$472B   W N1000
$472D   W N100
$472F   W N10
$4731   W N1
$4733   W PRINT_ACC
$4735   B HAVE_DIGITS
$4736   C SCORE_I2A
$4736   # Note: does not work for 0, outputs all blanks
$473A   ; get SCORE
$473D   ; put in number to print
$4740   ; output buffer
$4743   ; table of divisors
$4746   ; save address of table
$4747   C next_divisor
$4748   ; HL=next divisor address
$474B   ; DE=next divisor
$474C   ; restore HL
$474D   ; save HL
$474E   C subtract_divisor
$474E   ; get accumulator
$4752   ; subtract divisor
$4754   ; store accumulator
$4757   ; jump forward if overflowed
$4759   ; count one more
$475C   C subtraction_overflowed
$475C   ; restore accumulator
$475F   ; jump forward if not zero
$4761   ; if this digit is 0 and have_digits is 0, output a space
$476A   C make_digit
$476A   ; add '0'
$4773   C output_digit
$4773   ; restore accumulator
$4776   ; store digit
$4778   ; advance in divisor table
$477D   ; check for last divisor

$4783 10 B DEAD18

$4793   C INCR_SCORE
$47B2   C score_multiply
$47B2   # these 5 instructions are POKEd in BASIC to either 
$47B2   # NOP or ADD HL,DE depending on the selected skill level
$47B7   ; save score
$47BA   ; convert to digit sequence
$47BD   ; display
$47C0   ; add position of score
$47C4   ; to DE
$47C5   ; HL=number buffer
$47C8   ; 5 digits
$47CA   C out_digit
$47CB   ; is it a blank?
$47CF   ; copy digit
$47D1   ; restore BC
$47D2   ; next digit
$47D5   C skip_blanks
$47D6   ; next digit

$47D9 2 B DEAD19

$47E1   C DELETE_TORPEDO
$47E8   ; does position has a torpedo?
$47EB   ; ret if not
$47EC   ; delete torpedo
$47EF   C DELETE_BOMB
$47F6   ; delete bomb
$47FF   # --------------------------------------------------------------------------------
$47FF   # Init restored fortress
$47FF   # --------------------------------------------------------------------------------
$47FF   C INIT_FORTRESS
$47FF   ; 58 chars black
$4804   C fortress_fill_outer_black
$4809   ; 50 chars grey
$480B   C fortress_fill_grey
$4810   ; 42 chars black
$4812   C fortress_fill_inner_black

$483D 2 B DEAD21

$4845 25 T press_any_key_msg
$485E   C PRESS_ANY_KEY
$485E   ; row 23, col 3
$485E   # --------------------------------------------------------------------------------
$485E   # Print message "Press any key"
$485E   # --------------------------------------------------------------------------------
$4864   ; DE=screen address

$486E 3 B DEAD22

$4871   C DELETE_PRESS_ANY_KEY
$4871   ; row 23, col 3
$4879   C put_space

$487F  3 B DEAD23
$4888 46 B DEAD24

$48B6   C FLASH_ALIEN
$48BC   ; fortress-col+3
$48BD   ; row=5
$48BF   ; HL=screen address
$48C2   ; distance between rows
$48C5   ; 14 lines
$48C7   C flash_next_line
$48C7   ; save line counter
$48C8   ; 5 columns
$48CA   C flash_next_col
$48CA   ; get screen char
$48CB   ; invert it
$48CD   ; and store
$48CF   ; loop for all columns
$48D1   ; next row
$48D2   ; get row count
$48D6   ; loop for next row

$48D9 72 B DEAD25
$4927 34 B DEAD26

$494F   B LEVEL_DELAY_lo
$4950   B LEVEL_DELAY_hi
$4951   # --------------------------------------------------------------------------------
$4951   # Game loop
$4951   # --------------------------------------------------------------------------------
$4951   C GAME
$497E   C game_delay

$4988  7 B DEAD27
$4995 24 B DEAD28

$49B3   C CLEAR_GAME_AREA
$49B3   ; row 2 column 0
$49B6   ; HL = screen address
$49B9   ; 20 rows
$49BB   C clear_next_row
$49BB   ; 32 chars
$49BD   ; clear char
$49C0   ; loop for columns
$49C6   ; loop for rows
$49CF   W pressed_key
$49D1   # --------------------------------------------------------------------------------
$49D1   # Test keyboard and do actions
$49D1   # --------------------------------------------------------------------------------
$49D1   C TEST_KEYBOARD
$49D1   ; scan keyboard, H=column, L=half-row
$49D4   C
$49E1   ; no key pressed, return
$49E5   ; check top right half-row
$49E7   ; key pressed - fire torpedo
$49F1   ; check 2nd right half-row
$49F3   ; key pressed - fire torpedo
$49FD   ; check top left half-row
$49FF   ; key pressed - move ship up
$4A02   ; check second left half-row
$4A04   ; key pressed - move ship up
$4A07   ; check bottom left half-row
$4A09   ; key pressed - move ship down
$4A0C   ; check third left half-row
$4A0E   ; key pressed - move ship down
$4A11   ; check thrird right half-row
$4A13   ; key pressed - move ship left or right
$4A15   ; check bottom right half-row
$4A17   ; not pressed - return
$4A18   C check_left_right
$4A1D   ; check move ship left
$4A22   ; check move ship left
$4A27   ; check move ship left
$4A2C   ; check move ship right
$4A31   ; check move ship right

$4A37 6 B DEAD29
