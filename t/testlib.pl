#!/usr/bin/env perl

use Modern::Perl;
use Test::More;
use Test::HexDifferences;
use Path::Tiny;
use Config;

$ENV{PATH} = join($Config{path_sep}, ".", $ENV{PATH});

sub unlink_testfiles {
	my(@additional) = @_;
    unlink(<*.p *.p.txt *.b81 *.bak t/*.p.txt t/*.bak>, @additional) 
        if !$ENV{DEBUG} && Test::More->builder->is_passing;
}

sub run_ok {
    my($cmd) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
	
    ok 0==system($cmd), $cmd;
	
	(Test::More->builder->is_passing) or die;
}
