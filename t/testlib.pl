#!/usr/bin/env perl

#------------------------------------------------------------------------------
# zx81bas
# Copyright (C) Paulo Custodio, 2023-2026
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

use Modern::Perl;
use Test::More;
use Test::HexDifferences;
use Config;
use Path::Tiny;
use Text::Diff;
use File::Spec;
use Text::ParseWords qw(shellwords);

$ENV{PATH} = join( $Config{path_sep}, "build/Debug", $ENV{PATH} );

use vars '$test', '$null';
$test = "test_" . ( $0 =~ s/\W/_/gr );
$null = ( $^O eq 'MSWin32' ) ? 'nul' : '/dev/null';

unlink_testfiles();

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
sub check_text_file {
    my ( $got_file, $exp_file ) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    ( my $got_text = path($got_file)->slurp ) =~ s/\r\n/\n/g;
    ( my $exp_text = path($exp_file)->slurp ) =~ s/\r\n/\n/g;

    # remove version
    $got_text =~ s/Version: .*//;
    path($got_file)->spew_raw($got_text);

    note "text diff expected ($exp_file) got ($got_file)";
    my $diff = diff( \$exp_text, \$got_text, { STYLE => 'Unified' } );
    if ( $diff ne "" ) {
        diag $diff;
        if ( $ENV{DEBUG} ) {
            system( "start \"\" WinMergeU.exe " . "$exp_file $got_file" );
        }
    }
    ok $diff eq "", "text files are equal";

    ( Test::More->builder->is_passing ) or die;
}

sub run_ok {
    my ($cmd) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    $cmd = normalize_command($cmd);
    ok 1,                 "Running: $cmd";
    ok 0 == system($cmd), $cmd;

    ( Test::More->builder->is_passing ) or die;
}

#------------------------------------------------------------------------------
sub unlink_testfiles {
    my (@additional) = @_;
    unlink( <${test}*>, @additional )
        if Test::More->builder->is_passing;
}

#------------------------------------------------------------------------------
# need to normalize slashes in the command only, not in the arguments,
# so that Strawberry Perl is able to start build/Debug/z88dk-z80asm.exe
sub normalize_command {
    my ($cmd) = @_;

    # Split command into tokens safely
    my @tokens = shellwords($cmd);

    # Normalize only the executable path
    if ( $^O eq 'MSWin32' ) {
        my @parts = split( '/', $tokens[0] );
        $tokens[0] = File::Spec->catfile(@parts);
    }

    # Reassemble command
    my $fixed_cmd = join( ' ', @tokens );
    return $fixed_cmd;
}

1;
