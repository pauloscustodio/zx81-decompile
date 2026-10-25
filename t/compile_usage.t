#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

capture_nok( "zx81_compile", "t/expected/compile_usage.txt" );

unlink_testfiles;
done_testing;
