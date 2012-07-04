#!/usr/bin/perl

my $inputfilename=$ARGV[0];
my $outputfilename=$ARGV[1];

if ($inputfilename eq undef || $outputfilename eq undef) {
	die "usage: colorize.pl inputfilename outputfilename"
}

open(INPUTFILE,$inputfilename)
	or die "failed to open $inputfilename for input";

open(OUTPUTFILE,">$outputfilename")
	or die "failed to open $outputfilename for ouptut";

while (<INPUTFILE>) {

	my $line=$_;
	chomp($line);

	if ($line eq "{{{") {
		my $code="";

		while (<INPUTFILE>) {

			$line=$_;
			chomp($line);

			if ($line eq "}}}") {
				# FIXME: colorize here
				print OUTPUTFILE "$code";
				last;
			}

			$code=$code.$line."\n";
		}
	} else {
		print OUTPUTFILE "$line\n";
	}
}
