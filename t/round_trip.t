#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

for my $p_file ( path("tapes")->children(qr/\.p$/) ) {
    note $p_file;
    my $bas_file     = "$test.bas";
    my $exp_bas_file = path( "t/expected", $p_file->basename(".p") . ".bas" );
    my $out_p_file   = "$test.p";

    # decompile
    run_ok("zx81_decompile -o $bas_file $p_file");
    run_ok("dos2unix -q $bas_file");
    check_text_file( $exp_bas_file, $bas_file );

    # compile
    run_ok("zx81_compile -o $out_p_file $bas_file");
    compare_p( $p_file, $out_p_file );

    unlink "$p_file.txt" if Test::More->builder->is_passing;
}

unlink_testfiles;
done_testing;
