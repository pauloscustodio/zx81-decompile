#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

capture_nok( "zx81_decompile", "t/expected/decompile-usage.txt" );

unlink_testfiles;
done_testing;
