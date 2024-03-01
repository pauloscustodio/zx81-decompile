#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

use Modern::Perl;

my @progs = qw( 
	test_vars 
	show_float
	slow 
	fast 
	FortressOfZorlac
	GrimmsFairyTrails
);

for my $prog (@progs) {
	my $ctl_opt = (-f "t/$prog.ctl") ? "-c t/$prog.ctl" : "";
	run_ok("zx81_decompile -o t/$prog.b81 $ctl_opt t/$prog.p");
	run_ok("dos2unix -q t/$prog.b81");
	run_ok("zx81_compile -o $prog.p t/$prog.b81");
	eq_or_dump_diff(
		path("t/$prog.p")->slurp_raw,
		path("$prog.p")->slurp_raw,
		{ address => 0x4009 },
		"compare t/$prog.p $prog.p",
	);

	unlink_testfiles("$prog.p");
}

done_testing;
