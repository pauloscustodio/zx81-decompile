BEGIN { use lib 't'; require 'testlib.pl'; }

for my $p_file (<t/input/*.p>) {
    my $source = $p_file =~ s/\.\w+$//r;
    my $temp   = $test . "-" . path($p_file)->basename(".p");

    # decompile
    run_ok("build/Debug/z88dk-zx81dis $source.p");
    ( Test::More->builder->is_passing ) or die;
    copy( "$source.bas", "$temp.bas" );

    # compile
    run_ok("build/Debug/z88dk-zx81dis -r $temp.bas");
    ( Test::More->builder->is_passing ) or die;
    copy( "$temp.asm", "$source.asm" );

    # compare
    my $exp = path("$source.p")->slurp_raw;
    my $got = path("$temp.p")->slurp_raw;
    ok( $exp eq $got ), "round trip test";

    if ( $exp ne $got ) {

        # dump both
        my $dump_tool = "perl dump_p.pl";

        my $exp_file = "$temp-exp.dump";
        run_ok("$dump_tool $source.p > $exp_file");

        my $got_file = "$temp-got.dump";
        run_ok("$dump_tool $temp.p > $got_file");
        if ( $ENV{DEBUG} ) {
            system("start \"\" WinMergeU.exe $exp_file $got_file");
        }
    }
}

unlink_testfiles;
done_testing;
