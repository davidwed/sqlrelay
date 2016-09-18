#!/usr/bin/perl

use FileHandle;


my $guide=new FileHandle;
$guide->open($ARGV[0]);

foreach $guideline(<$guide>) {
	if ($guideline=~m/^@.*@$/) {
		my $partfilename=$guideline;
		$partfilename=~s/@//g;
		my $partfile=new FileHandle;
		if ($partfile->open("parts/$partfilename")) {
			foreach $partfileline(<$partfile>) {
				print $partfileline;
			}
			$partfile->close();
		} else {
			print $guideline;
		}
	} else {
		print $guideline;
	}
}

$guide->close();
exit;
