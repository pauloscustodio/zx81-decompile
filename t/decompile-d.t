BEGIN { use lib 't'; require 'testlib.pl'; }

for my $p_file ( "tapes/slow.p", "tapes/fast.p", "tapes/test_vars.p" ) {
    my $expected =
          "t/expected/"
        . path($0)->basename(".t") . "-"
        . path($p_file)->basename(".p") . ".txt";
    run_ok("zx81_decompile -d -o $test.bas $p_file");
    check_text_file( "$test.bas", $expected );
}

unlink_testfiles;
done_testing;
