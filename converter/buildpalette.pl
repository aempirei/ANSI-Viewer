#!/usr/bin/perl

use strict;
use warnings;

my $line = join(' ', @ARGV);

my @colors = split(/\s+/, $line);

print pack('s', scalar(@colors));

for my $color (@colors) {
	print pack('L', hex $color);
}
