#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;
use ZX81::Tape qw(make_p);

my $exp_empty_p = "t/expected/empty.p";

# make benchmark empty file
path("$test.bas")->spew("");
make_p( "$test.bas", $exp_empty_p );

# compile empty file
run_ok("zx81_compile -o $test.p $test.bas");

# compare
compare_p( "$test.p", $exp_empty_p );

unlink_testfiles;
done_testing;
