#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

run_ok("perl make_p.pl -o $test.p t/input/test_make_p.bas");

unlink_testfiles;
done_testing;
