# control file for disassemble of FortressOfZorlac

$4082 B(57)	FORTRESS1 #; Fortress bricks
$40bb B(1) 	SAVE_FORTRESS1
$40bc B(92)	FORTRESS2
$4118 W		addr_fortress
$411a W		addr_fortress_save
$411c B		fortress_col
$411d B		fortress_row
$411e B 	fortress_rows
$411f B		fortress_cols
$4120 B		fortress_blocks
$412c C		DRAW_FORTRESS
$412c #;--------------------------------------------------------------------------------
$412c #; Draw fortress at fortress_row/fortress_col
$412c #;--------------------------------------------------------------------------------
$4138 ;20 rows
$413d ;9 columns
$4142 ;57 blocks
$4191 C 	rotate_outer_layer
$415b C		rotate_inner_layer
$418a C		return
