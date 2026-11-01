#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(dump_p);

for my $input_p_file ( path("t/tapes")->children(qr/\.p$/) ) {
    note "$input_p_file";
    my $basename       = $input_p_file->basename('.p');
    my $input_ctl_file = path( "t/tapes", "$basename.ctl" );

    # round-trip without .ctl file
    run_ok("zx81_decompile -o $basename.bas $input_p_file");
    run_ok("zx81_compile -o $basename.p $basename.bas");
    compare_p( $input_p_file, "$basename.p" );

    unlink "$basename.bas", "$basename.p" if Test::More->builder->is_passing;

    next;

    my $exp_p_file   = path( "t/asm/expected", "$basename.p" );
    my $exp_map_file = path( "t/asm/expected", "$basename.map" );
    my $exp_txt_file = path( "t/asm/expected", "$basename.txt" );
    my $exp_bas_file = path( "t/asm/expected", "$basename.bas" );

    # make .p file and .map file
    run_ok("zx81_compile -o $exp_p_file -m $basename.map $input_p_file");
    check_text_file( $exp_map_file, "$basename.map" );

    # make .txt file
    dump_p( $exp_p_file, "$basename.txt" );
    check_text_file( $exp_txt_file, "$basename.txt" );

    # make .bas file
    run_ok("zx81_decompile -o $basename.bas $exp_p_file");
    check_text_file( $exp_bas_file, "$basename.bas" );

    # round-trip
    run_ok("zx81_compile -o $basename.p $basename.bas");
    compare_p( $exp_p_file, "$basename.p" );
}

unlink_testfiles;
done_testing;
