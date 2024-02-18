#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

# assemble z80ops from z80pack with zx81_compile
my @z80ops = path("t/z80ops.asm")->lines;

path("z80ops.b81")->spew(<<END, grep {!/^\s*TITLE/} @z80ops);
#ASM
END

run_ok("zx81_compile z80ops.b81"); # builds z80ops.p

# assemble z80ops with z88dk-z80asm
my @z80asm = (
	'org $4009'."\n",
	'defb $00, $00, $00, $09, $46, $0a, $46, $22, $49, $00, $00, $23, $49, $00, $00, $00, $00, $23, $49, $23, $49, $00, $5d, $40, $00, $02, $00, $00, $ff, $ff, $ff, $37, $09, $46, $00, $00, $00, $00, $00, $00, $00, $00, $00, $ff, $ff, $00, $00, $40, $21, $19, $40, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $76, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00'."\n",
	'defb $00, $00, $88, $05, $ea'."\n",
	(map {s/^(\w+)/$1:/; $_} grep {!/^\s*TITLE/} @z80ops),
	'defb $76'."\n",
	'defb $76'."\n");
for (1..24) {
	push @z80asm, 'defb 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,$76'."\n";
}
push @z80asm, 'defb $80'."\n";

path("z80ops.asm")->spew(@z80asm);
run_ok("z88dk-z80asm -b -l z80ops.asm");

eq_or_dump_diff(
    path("z80ops.p")->slurp_raw,
    path("z80ops.bin")->slurp_raw,
	{ address => 0x4009 },
    "compare z80ops.p z80ops.bin",
);

unlink_testfiles(<z80ops.*>);
done_testing;
