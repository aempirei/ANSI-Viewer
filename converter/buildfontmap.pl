#!/usr/bin/perl

use strict;
use warnings;

my $fontfile = 'fontmap.ppm';

my $line = join(' ', @ARGV);

my @byte = split(/\s+/, $line);

print pack('s', scalar(@byte));

for my $ch (@byte) {

	system("./defont $ch < $fontfile");
}
