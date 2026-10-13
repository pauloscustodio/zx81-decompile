#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

# z80ops from z80pack
my @z80ops = grep { !/^\s*TITLE/ }
    map { s/^IND\b/IND_/ir }
    map { s/\+IND\b/+IND_/ir }
    map { s/^M\b/M_/ir }
    map { s/\bDEFS\s+2/DEFW 0/ir } path("t/input/z80ops.asm")->lines;

# assemble zx81_compile
path("$test.bas")->spew( "#ASM\n", @z80ops );
run_ok("build/Debug/zx81_compile -o $test.1.p $test.bas");

# assemble with make_p.pl
run_ok("perl make_p.pl -o $test.2.p $test.bas");

# patch sysvars that are different in zx81_compile and t2p
for my $file ( "$test.1.p", "$test.2.p" ) {
    patch(
        $file,
        [ W => 0x400A, 0 ],         # E_PPC
        [ W => 0x4016, 0 ],         # CH_ADD
        [ W => 0x401A, 0x5000 ],    # STKBOT
        [ W => 0x401C, 0x5000 ],    # STKTOP
        [ B => 0x4027, 0xFF ],      # DEBOUNCE
        [ W => 0x4030, 0 ],         # T_ADDR
        [ W => 0x4034, 0xFFFF ],    # FRAMES
        [ B => 0x4038, 0x3C ],      # PR_CC
        [ W => 0x4039, 0x1921 ],    # SPOSN
    );
}

# compare
eq_or_dump_diff(
    path("$test.1.p")->slurp_raw, path("$test.2.p")->slurp_raw,
    { address => 0x4009 },        "compare $test.1.p $test.2.p",
);

unlink_testfiles;
done_testing;

sub patch {
    my ( $file, @patches ) = @_;
    my @bytes = map { ord } split //, path($file)->slurp_raw;
    for (@patches) {
        my ( $type, $addr, $value ) = @$_;
        if ( $type eq 'B' ) {
            $bytes[ $addr - 0x4009 ] = $value & 0xFF;
        }
        elsif ( $type eq 'W' ) {
            $bytes[ $addr - 0x4009 ] = $value & 0xFF;
            $bytes[ $addr - 0x4009 + 1 ] = ( $value >> 8 ) & 0xFF;
        }
        else {
            die "expected B, W";
        }
    }
    path($file)->spew_raw( join '', map { chr } @bytes );
}
