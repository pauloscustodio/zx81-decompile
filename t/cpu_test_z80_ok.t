#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(make_p);
use Data::Dump 'dump';

my $input_lis_file = "t/input/cpu_test_z80_ok.lis";
my $input_bas_file = "t/input/cpu_test_z80_ok.bas";
my $input_p_file   = "t/input/cpu_test_z80_ok.p";

# read cpu_test_z80_ok.lis built by z88dk-z80asm
my @asm_lines = path($input_lis_file)->lines;

# create [address, bytes, asm]
my @asm_entries;
for my $line (@asm_lines) {

    # file name ':'
    if ( $line =~ /^ \S .* : /xi ) {
        next;
    }

    # org directive
    if ( $line =~ /^ \s+ \d+ \s+ org /xi ) {
        next;
    }

    # last address line
    if ( $line =~ /^ \s{2} [0-9A-F]{4} \s* $/xi ) {
        next;
    }

    # normal asm line
    if (
        $line =~ /^ \s+ \d+                     # line number
                    \s+ ([0-9A-F]{4})           # address
                    \s+ ( (?: [0-9A-F]{2} )+ )  # bytes
                    \s+ (.*)/xi
        )
    {
        my ( $address, $bytes, $asm ) = ( hex($1), $2, $3 );
        $asm =~ s/\s*;.*//;
        push @asm_entries, [ $address, $bytes, $asm ];
        next;
    }

    # continuation line
    if (
        $line =~ /^ \s+ ( (?: [0-9A-F]{2} )+ )  # bytes
                    \s+ $/xi
        )
    {
        my $bytes = $1;
        die "no previous asm entry to continue" unless @asm_entries;
        $asm_entries[-1][1] .= $bytes;
        next;
    }

    die "cannot parse line: $line";
}

# replace bytes string by list of values
for my $entry (@asm_entries) {
    $entry->[1] = [ map { hex($_) } $entry->[1] =~ /[0-9A-F]{2}/ig ];
}

# build bas file
open( my $fh, '>', $input_bas_file ) or die "Cannot open output file: $!";
say $fh "#ASM";
for my $entry (@asm_entries) {
    my ( $address, $bytes, $asm ) = @$entry;
    printf $fh "    %-24s; %04x %s\n",
        $asm, 0x4082 + $address,
        join( ' ', map { sprintf "%02x", $_ } @$bytes );
}
say $fh "#ENDASM";
close($fh);

# build .p file with ZX81::Tape
open( $fh, '>', "$test.bas" ) or die "Cannot open output file: $!";
print $fh "0 REM ";
for my $entry (@asm_entries) {
    my ( $address, $bytes, $asm ) = @$entry;
    printf $fh "\\%02X", $_ for @$bytes;
}
print $fh "\n";
close($fh);

make_p( "$test.bas", $input_p_file );

# build .p file from .bas file
run_ok("zx81_compile -o $test.p $input_bas_file");
compare_p( $input_p_file, "$test.p" );

# round-trip
# make control file to force disassembly
open( $fh, '>', "$test.ctl" ) or die "Cannot open output file: $!";
for my $entry (@asm_entries) {
    my ( $address, $bytes, $asm ) = @$entry;
    printf $fh "0x%04X C\n", 0x4082 + $address;
}
close($fh);

run_ok("zx81_decompile -c $test.ctl -o $test.bas $input_p_file");
run_ok("zx81_compile -o $test.p $test.bas");
compare_p( $input_p_file, "$test.p" );

unlink_testfiles;
done_testing;
