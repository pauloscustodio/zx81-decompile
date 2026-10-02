BEGIN { use lib 't'; require 'testlib.pl'; }

# build z80ops.p, needs z88dk
if ( !-f "t/input/z80ops.p" ) {

    # build .bas with z80pack z80ops.asm
    my @basic = "#ASM\n";
    push @basic, grep { !/TITLE/ } path("t/input/z80ops.asm")->lines;
    push @basic, "\tRET\n", "#ENDASM\n", "10 RAND USR &Z80OPS\n";
    path("$test.bas")->spew_raw(@basic);

    # build .p
    run_ok("perl make_p.pl -l $test.bas");
    copy( "$test.p", "t/input/z80ops.p" );
}

# build control file to restart disassembly after all control flow ends
path("$test.ctl")->spew_raw(<<END);
asm 0x4082 C z80ops
asm 0x421C C
asm 0x421E C
asm 0x4220 C
asm 0x4223 C
asm 0x422C C
asm 0x423D C
asm 0x4449 C
asm 0x4453 C
asm 0x4455 C
END

# decompile .p
run_ok(
    "build/Debug/zx81_decompile -o $test.2.bas -c $test.ctl t/input/z80ops.p");

# check disassembly
check_text_file( "$test.2.bas", "t/expected/z80ops.txt" );

unlink_testfiles;
done_testing;
