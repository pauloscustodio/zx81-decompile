#!/usr/bin/env perl

# disassemble Fortress of Zorlac assembly routines located in $407d - $4a3e

use Modern::Perl;
use CPU::Z80::Disassembler;

my $dis = CPU::Z80::Disassembler->new;
$dis->load_control_file("FortressOfZorlac.ctl");
$dis->write_asm("FortressOfZorlac.asm");
