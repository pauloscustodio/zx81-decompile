#------------------------------------------------------------------------------
# zx-81 decompile
# Copyright (C) Paulo Custodio, 2023-2024
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

all: GrimmsFairyTrails.p GrimmsFairyTrails2.p

GrimmsFairyTrails.p : GrimmsFairyTrails.bas ../make_p.pl \
					../t/GrimmsFairyTrails.p GrimmsFairyTrails.ctl disassemble.pl \
					merge_asm.pl Makefile
	perl disassemble.pl
	perl merge_asm.pl GrimmsFairyTrails.bas GrimmsFairyTrails.asm
	perl ../make_p.pl GrimmsFairyTrails.bas

GrimmsFairyTrails2.p : GrimmsFairyTrails2.bas ../make_p.pl Makefile
	perl ../make_p.pl GrimmsFairyTrails2.bas

clean:
	$(RM) GrimmsFairyTrails.b81 GrimmsFairyTrails.asm GrimmsFairyTrails.map GrimmsFairyTrails.bin GrimmsFairyTrails.sym
	$(RM) GrimmsFairyTrails2.b81 GrimmsFairyTrails2.asm GrimmsFairyTrails2.map GrimmsFairyTrails2.bin GrimmsFairyTrails2.sym
	$(RM) *.bak