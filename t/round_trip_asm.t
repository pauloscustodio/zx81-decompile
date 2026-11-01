#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(dump_p);

for my $input_bas_file ( path("t/asm/input")->children(qr/\.bas$/) ) {
    note "$input_bas_file";
    my $basename     = $input_bas_file->basename('.bas');
    my $exp_p_file   = path( "t/asm/expected", "$basename.p" );
    my $exp_map_file = path( "t/asm/expected", "$basename.map" );
    my $exp_txt_file = path( "t/asm/expected", "$basename.txt" );
    my $exp_bas_file = path( "t/asm/expected", "$basename.bas" );

    # make .p file and .map file
    run_ok("zx81_compile -o $exp_p_file -m $test.map $input_bas_file");
    check_text_file( $exp_map_file, "$test.map" );

    # make .txt file
    dump_p( $exp_p_file, "$test.txt" );
    check_text_file( $exp_txt_file, "$test.txt" );

    # make .bas file
    run_ok("zx81_decompile -o $test.bas $exp_p_file");
    check_text_file( $exp_bas_file, "$test.bas" );

    # round-trip
    run_ok("zx81_compile -o $test.p $test.bas");
    compare_p( $exp_p_file, "$test.p" );
}

unlink_testfiles;
done_testing;
