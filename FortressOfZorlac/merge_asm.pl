#!/usr/bin/perl

# Copy disassembled data into basic file

use Modern::Perl;
use Path::Tiny;
use File::Copy;

@ARGV==2 or die "Usage: $0 file.bas file.asm\n";
my($bas_file, $asm_file) = @ARGV;

# remove assembly from start of file
my @bas_lines = path($bas_file)->lines;

while (@bas_lines && $bas_lines[0] !~ /#ENDASM/) {
    shift @bas_lines;
}
shift @bas_lines;       # remove #ENDASM

while (@bas_lines && $bas_lines[0] !~ /\S/) {
    shift @bas_lines;   # remove blank lines
}

die "#ENDASM not found" unless @bas_lines;

# collect disassembled code
my @asm_lines = path($asm_file)->lines;
my @equ_lines = grep {/^\w+\s+equ\s+/} @asm_lines;

# remove bytes before REM
while (@asm_lines && $asm_lines[0] !~ /; REM of start of assembly/) {
    shift @asm_lines;
}

shift @asm_lines;       # ; REM of start of assembly
shift @asm_lines;       # defb $EA

# remove bytes after end
while (@asm_lines && $asm_lines[-1] !~ /; REM of end of assembly/) {
    pop @asm_lines;
}

pop @asm_lines;         # ; REM of end of assembly

# merge
@bas_lines = ("#ASM\n\n", @equ_lines, "\n\n", @asm_lines, "\n\n#ENDASM\n\n", @bas_lines);
copy($bas_file, "$bas_file.bak");
path($bas_file)->spew(@bas_lines);

