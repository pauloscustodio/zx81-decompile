# z88dk-zx81dis - Disassemble a tape image.p

Disassemble a zx81 tape image into a .bas file that can be compiled
again to a .p file. The .bas file is similar to the one accepted 
by z88dk-zx18bas, with the addition of '\00' sequences in the 
BASIC code so signal spaces compiled in the object. z88dk-zx18bas
does not accept these.

## Usage:

    z88dk-zx81dis file.p
	
Generates a file.bas file with the disassembled output.

    z88dk-zx81ass file.bas
	
Reverses the process, generates a file.p.
