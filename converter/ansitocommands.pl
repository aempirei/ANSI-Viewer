#!/usr/bin/perl
#
#

use strict;
use warnings;

my $data = join('', <STDIN>);

my $bold = 0;
my $fg = 7;
my $bg = 0;
my $xpos = 1;
my $ypos = 1;

my $xold = 1;
my $yold = 1;

while(defined $data and $data ne '') {

	my $right;

	if($data =~ /^\33\[((?:\d+;){0,5}\d+)([A-Za-z])(.*)/s) {

		my @values = split(/;/, $1);
		my $code = $2;
		$right = $3;

		# print "code = $code   values = ".join(' ', @values)."\n";

		if($code eq 'm') {

			foreach my $value (@values) {
				if($value == 0) {
					$bold = 0;
					$fg = 7;
					$bg = 0;
				} elsif($value == 1) {
					$bold = 1;
				} elsif($value >= 30 and $value <= 37) {
					$fg = $value - 30;
				} elsif($value == 39) {
					$fg = 7;
				} elsif($value >= 40 and $value <= 47) {
					$bg = $value - 40;
				} elsif($value >= 49) {
					$bg = 0;
				} else {
					die "unknown code=$code value=$value";
				}

				print "color bold=$bold fg=$fg bg=$bg\n";
			}

		} elsif($code eq 'J' and @values == 1 and $values[0] eq '2') {

			print "clear\n";

		} elsif($code eq 'C' and @values == 1) {

			$xpos += $values[0];


			print "move $ypos $xpos\n";

		} elsif($code eq 'H' and @values >= 1 and @values <= 2) {

			my ( $line, $col ) = @values;

			$ypos = $line if(defined $line);
			$xpos = $col if(defined $col);

			print "move $ypos $xpos\n";

		}
		elsif($code eq 'A' and @values == 1) { $ypos -= $values[0]; print "move $ypos $xpos\n"; }
		elsif($code eq 'B' and @values == 1) { $ypos += $values[0]; print "move $ypos $xpos\n"; }
		elsif($code eq 'C' and @values == 1) { $xpos += $values[0]; print "move $ypos $xpos\n"; }
		elsif($code eq 'D' and @values == 1) { $xpos -= $values[0]; print "move $ypos $xpos\n"; }

		else {

			die "unknown code=$code values=".join(';',@values)."\n";
		}	

	} elsif($data =~ /^\33\[([A-Za-z])(.*)/s) {

		my $code = $1;
		$right = $2;

		if($code eq 's') {

			$yold = $ypos;
			$xold = $xpos;

			print "save $ypos $xpos\n";

		} elsif($code eq 'u') {

			$xpos = $xold;
			$ypos = $yold;

			print "move $ypos $xpos\n";

		}
		elsif($code eq 'A') { $ypos--; print "move $ypos $xpos\n"; }
		elsif($code eq 'B') { $ypos++; print "move $ypos $xpos\n"; }
		elsif($code eq 'C') { $xpos++; print "move $ypos $xpos\n"; }
		elsif($code eq 'D') { $xpos--; print "move $ypos $xpos\n"; }
		else {

			die "unknown code=$code";
		}

		# print "code = $code\n";

	} elsif($data =~ /^\x1a/) {

		# quit when we receive a ^Z so we drop before the SAUCE line	

		last;


	} elsif($data =~ /^([^\33])(.*)/s) {

		my $char = $1;
		$right = $2;

		if($char eq "\r") {
		} elsif($char eq "\n") {
			$xpos = 1;
			$ypos++;
		} else {
			$xpos++;
			print "print ".unpack('C',$char)."\n";
		}

		if($xpos > 80) { $ypos++; $xpos = 1 }

		print "move $ypos $xpos\n";

	} elsif($data =~ /^\33\[(?:\?7|=|=0|=\d;7)h(.*)/s) {

		$right = $1;

	} elsif($data =~ /^\33/s) {

		die "unknown escape sequence";

	} else {

		die "unknown reason";
	}

	$data = $right;
}
