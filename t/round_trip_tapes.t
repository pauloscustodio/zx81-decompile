#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(dump_p);

for my $input_p_file ( path("t/tapes/input")->children(qr/\.p$/) ) {
    note "$input_p_file";
    my $basename       = $input_p_file->basename('.p');
    my $input_ctl_file = path( "t/tapes/input",    "$basename.ctl" );
    my $exp_bas_file   = path( "t/tapes/expected", "$basename.bas" );

    # round-trip without .ctl file
    run_ok("zx81_decompile -o $basename.bas $input_p_file");
    run_ok("zx81_compile -o $basename.p $basename.bas");
    compare_p( $input_p_file, "$basename.p" );

    unlink "$basename.bas", "$basename.p" if Test::More->builder->is_passing;

    # decompile with .ctl file
    run_ok("zx81_decompile -c $input_ctl_file -o $basename.bas $input_p_file");
    check_text_file( $exp_bas_file, "$basename.bas" );

    # round-trip
    run_ok("zx81_compile -o $basename.p $basename.bas");
    compare_p( $input_p_file, "$basename.p" );

    unlink "$basename.bas", "$basename.p" if Test::More->builder->is_passing;
}

unlink_testfiles;
done_testing;
