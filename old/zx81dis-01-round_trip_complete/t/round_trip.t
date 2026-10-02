BEGIN { use lib 't'; require 'testlib.pl'; }

for my $p_file (<t/input/*.p>) {
    my $source = $p_file =~ s/\.\w+$//r;
    my $base   = $test . "-" . path($p_file)->basename(".p");

    # decompile
    copy( "$source.p", "$base.p" );
    run_ok("build/Debug/z88dk-zx81dis $base.p");
    copy( "$base.bas", "$source.bas" );

    # compile
    run_ok("build/Debug/z88dk-zx81dis -r $base.bas");
    copy( "$base.asm", "$source.asm" );

    # compare
    my $exp = path("$source.p")->slurp_raw;
    my $got = path("$base.p")->slurp_raw;
    ok( $exp eq $got ), "round trip test";

    if ( $exp ne $got ) {

        # dump both
        my $dump_tool = "perl dump_p.pl";

        my $exp_file = "$base-exp.dump";
        run_ok("$dump_tool $source.p > $exp_file");

        my $got_file = "$base-got.dump";
        run_ok("$dump_tool $base.p > $got_file");
        if ( $ENV{DEBUG} ) {
            system("start \"\" WinMergeU.exe $exp_file $got_file");
        }
    }
}

unlink_testfiles;
done_testing;
