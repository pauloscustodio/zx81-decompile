BEGIN { use lib 't'; require 'testlib.pl'; }

for my $ctl_file (<t/input/*.ctl>) {
    my $p_file = "tapes/" . path($ctl_file)->basename(".ctl") . ".p";
    ok -f $p_file, "$p_file exists";

    my $expected =
          "t/expected/"
        . path($0)->basename(".t") . "-"
        . path($p_file)->basename(".p") . ".txt";
    run_ok("zx81_decompile -o $test.bas -c $ctl_file $p_file");
    check_text_file( "$test.bas", $expected );
}

unlink_testfiles;
done_testing;
