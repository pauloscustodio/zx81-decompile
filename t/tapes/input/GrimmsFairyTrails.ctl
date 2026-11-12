# ASM lines

$4088	C PUT_CHAR_AT
$4088	# --------------------------------------------------------------------------------
$4088	# input is BC with col-row, A with char to print
$4088	# output is HL with screen address and D with previous char
$4088	# --------------------------------------------------------------------------------
$409A	; HL=C*33
$409C	; HL=C*33+B
$409D	; get display file
$40A2	; add one
$40A6	; read previous char
$40A7	; write char

$40E0	# --------------------------------------------------------------------------------
$40E0	# Fill 22 center lines of screen with character in BACKGROUND+1
$40E0	# --------------------------------------------------------------------------------
$40E0	C FILL_SCR
$40E0	; C=22 lines
$40E6	; size of row
$40E9	C fill_row
$40E9	; B=32 chars
$40EB	; move to next row
$40F0	C BACKGROUND                   

$40FF	W fill_line_addr

$4102	# --------------------------------------------------------------------------------
$4102	# Fill a rectangle of dots before the maze walls are drawn
$4102	# --------------------------------------------------------------------------------
$4102	C DRAW_DOTS
$4102	; B=30 column 30
$4104	; C=15 row 15
$4106	C draw_next_dot
$4106	; A=dot
$4108	; put a dot
$4111	; C=row 15

$4118	# --------------------------------------------------------------------------------
$4118	# Draw walls
$4118	# --------------------------------------------------------------------------------
$4118	C DRAW_WALLS1
$4118	; C=row 10
$411A	; B=column 29 
$411C	C wall_next_dot1
$411C	; char=solid block
$411E	; put a char
$4125	; repeat until row 6
$4127	; C=row 10
$412A	; C=column -= 2
$412C	; repeat until column 15
$4130	; C=row 10
$4132	C wall_next_dot2
$4132	; B=column 14
$4134	C wall_next_dot3
$4134	; char=solid block
$4136	; put a char
$413D	; repeat until row 6
$413F	; C=row 10
$414C	C wall_next_dot4
$415B	C wall_next_dot5

$416E 	C DRAW_WALLS2
$4170 	C code2
$4172 	C wall_next_dot6
$417E 	C code5
$4182 	C code3
$4184 	C code4
$4186 	C wall_next_dot7
$4192 	C code6

$41A2	B var4
$41A3	B var5
$41AC	B var3         
$41AE	B var2
$41B0	B var1

$4674	C LEVEL_DELAY
$4674	; POKEd from basic 5 for fastest, 30 for slowest
$47A9	B BANNER_CHAR
$47AA	B BANNER_COL

$47AE 16 B banner_bits_lu

$47C2	# --------------------------------------------------------------------------------
$47C2	# Display one character in big format
$47C2	# --------------------------------------------------------------------------------
$47C2	C DISPLAY_BANNER
$47C2	; address of screen
$47C6	C SCR_OFFSET
$47C6	; input offset POKEd from BASIC
$47CE	; skip if column not 1
$47D1	; for column 1 add input offset
$47D6	C skip_setting_df_cc
$47D6	; HL points at char to print
$47D9	; A has char to print
$47DC	; multiply by 4
$47DD	; exit if bit 6 was 1
$47DE	; multiply by 8
$47E1	; carry into D
$47E3	; DE=char*8
$47E4	; base of CHARS table
$47E7	; HL=address of char bitmap
$47E8	; 4 rows
$47EA	C banner_row
$47EA	; 4 columns
$47EC	; get bitmap of two rows into DE
$47F0	; save char table pointer
$47F1	C banner_col
$47F1	; do some magic
$47FE	; lookup table of bitmaps
$4802	; HL points to correct bitmap
$4803	; A has the bitmap
$4804	; HL has the print position
$4807	; print the character
$4808	; advance print position
$4809	; and store it
$480C	; next column
$480E	; save bitmap
$480F	; offset to next row
$4812	; add to DF_CC
$4813	; and store
$4816	; restore bitmap
$4817	; restore chars pointer
$4819	; next row
$481B	; offset to next character
$481E	; add to DF_CC
$4822	; and store it
$4825	; read screen char; Note: not needed
