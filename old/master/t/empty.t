#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

# compile empty source file
path("t/empty.b81")->spew();
run_ok("zx81_compile t/empty.b81");

# decompile it
run_ok("zx81_decompile t/empty.p");

unlink_testfiles();
done_testing;
