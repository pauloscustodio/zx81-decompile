# disassembly control file for FortressOfZorlac.p

basic   30 C Restart		#Start the game
basic   30 					;clear screen
basic 1000 C Instructions 	#Show game instructions
basic 3000 C SaveRestart 	#Save program and restart

asm 0x4212 #------------------------------------------------------------------------------
asm 0x4212 #Compute screen position of row-col in BC to HL
asm 0x4212 #Preserves all other registers
asm 0x4212 #------------------------------------------------------------------------------
asm 0x4212 C SCR_POS
asm 0x421C C SCR_POS_next_row
asm 0x4223 C SCR_POS_row_found
asm 0x4226 C SCR_POS_next_col
asm 0x422D C SCR_POS_col_found

asm 0x4082(56) 	M FORTRESS1
asm 0x40BB		M SAVE_FORTRESS1
asm 0x40BC(91)	M FORTRESS2

asm 0x4118 W addr_fortress
asm 0x411A W addr_fortress_save
asm 0x411C B fortress_col
asm 0x411D B fortress_row
asm 0x411E B fortress_rows
asm 0x411F B fortress_cols
asm 0x4120 B fortress_blocks

asm 0x412C #------------------------------------------------------------------------------
asm 0x412C #Draw fortress at fortress_row/fortress_col
asm 0x412C #------------------------------------------------------------------------------
asm 0x412C C DRAW_FORTRESS
asm 0x4138 C ;20 rows
asm 0x413D C ;9 columns
asm 0x4142 C ;57 blocks
asm 0x415B C rotate_inner_layer
asm 0x418A C return
