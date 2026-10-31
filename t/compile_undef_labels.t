#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

capture_nok( "zx81_compile -o $test.p t/input/undef_labels.bas",
    "t/expected/undef_labels.txt" );

unlink_testfiles;
done_testing;
