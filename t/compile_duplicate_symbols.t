#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

capture_nok( "zx81_compile -o $test.p t/input/duplicate_symbols.bas",
    "t/expected/duplicate_symbols.txt" );

unlink_testfiles;
done_testing;
