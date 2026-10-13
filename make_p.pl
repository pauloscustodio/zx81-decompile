#!/usr/bin/env perl

# make a .P file from a mix of BASIC and ASSEMBLY
# code between #ASM and #ENDASM is assembled and converted to a REM line
# labels can be referenced in BASIC by &NAME
# BASIC can use 0xhhh and $hhh for hex numbers

use CPU::Z80::Assembler;
use CPU::Z80::Assembler::Program;
use Modern::Perl;
use Path::Tiny;
use Getopt::Std;

getopts( "do:", \my %opts );
@ARGV == 1 or die "Usage: $0 [-d] [-o file.p] file.bas\n";
my $input_bas = shift;
my $output_p  = $opts{'o'} ? $opts{'o'} : $input_bas =~ s/\.\w+$/.p/r;

# read code
my @asm = (" ORG 16514");
my @basic;
my $fh;
open( $fh, "<", $input_bas ) or die "open $input_bas: $!\n";
my $in_basic = 1;
while (<$fh>) {
    if (/#ASM/) {
        $in_basic = 0;
    }
    elsif (/#ENDASM/) {
        $in_basic = 1;
    }
    elsif ($in_basic) {
        push @basic, $_;
    }
    else {
        push @asm, $_;
    }
}

# assemble
my $program      = CPU::Z80::Assembler::Program->new();
my $token_stream = z80lexer(@asm);
$program->parse($token_stream);
my $bin     = $program->bytes();
my %symbols = %{ $program->symbols };

# insert binary output as REM in basic
my $rem = "1 REM ";
for ( split( //, $bin ) ) {
    $rem .= "\\" . sprintf( "%02X", ord($_) );
}
$rem .= "\n";
unshift @basic, $rem;

# translate labels in basic and hex numbers
for (@basic) {
    s/(&(\w+))/ exists $symbols{$2} ? $symbols{$2}->evaluate : $1 /gie;
    s/\$([0-9a-f]+)\b/ hex($1) /gie;
    s/\b0x([0-9a-f]+)\b/ hex($1) /gie;
}

# output basic
my $output_b81 = path($input_bas)->basename(".bas") . ".b81";
path($output_b81)->spew_raw(@basic);

# convert to .p
run("t2p -o $output_p $output_b81");
unlink $output_b81 unless $opts{'d'};

# rename line 1 REM to 0
my @bytes = map { ord } split //, path($output_p)->slurp_raw;
$bytes[ 0x407d - 0x4009 ] = 0;
$bytes[ 0x407e - 0x4009 ] = 0;

# remove last byte (t2p writes the first byte of E_LINE)
pop @bytes;

path($output_p)->spew_raw( join "", map { chr } @bytes );

sub run {
    my ($cmd) = @_;
    print "% $cmd\n";
    system($cmd) == 0 or die "command failed\n";
}
