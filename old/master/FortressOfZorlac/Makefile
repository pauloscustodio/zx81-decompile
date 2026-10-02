#------------------------------------------------------------------------------
# zx-81 decompile
# Copyright (C) Paulo Custodio, 2023-2024
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

all: FortressOfZorlac.p FortressOfZorlac2.p

FortressOfZorlac.p : FortressOfZorlac.bas ../make_p.pl \
					../t/FortressOfZorlac.p FortressOfZorlac.ctl disassemble.pl \
					merge_asm.pl Makefile
	perl disassemble.pl
	perl merge_asm.pl FortressOfZorlac.bas FortressOfZorlac.asm
	perl ../make_p.pl FortressOfZorlac.bas

FortressOfZorlac2.p : FortressOfZorlac2.bas ../make_p.pl Makefile
	perl ../make_p.pl FortressOfZorlac2.bas

clean:
	$(RM) FortressOfZorlac.b81 FortressOfZorlac.asm FortressOfZorlac.map FortressOfZorlac.bin FortressOfZorlac.sym
	$(RM) FortressOfZorlac2.b81 FortressOfZorlac2.asm FortressOfZorlac2.map FortressOfZorlac2.bin FortressOfZorlac2.sym
	$(RM) *.bak