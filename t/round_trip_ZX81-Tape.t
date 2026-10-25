#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(make_p);

for my $input_bas_file ( path("t/input/ZX81-Tape")->children(qr/\.bas$/) ) {
    my $basename     = $input_bas_file->basename('.bas');
    my $build_p_file = path("t/expected/ZX81-Tape")->child("$basename.p");

    # make benchmark .p file
    make_p( $input_bas_file, $build_p_file );

    # compile .bas file and compare with benchmark
    #run_ok("zx81_compile -o $test.p $input_bas_file");
    #compare_p( "$test.p", $build_p_file );

    # decompile .p file and compare with benchmark
    run_ok("zx81_decompile -o $test.bas $build_p_file");
    check_text_file( "$test.bas", "t/expected/ZX81-Tape/$basename.bas" );
}

unlink_testfiles;
done_testing;
