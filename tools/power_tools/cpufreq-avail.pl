#!/usr/bin/perl -w
use strict;
use warnings;

# get all available frequencies
my $file3 = "/sys/devices/system/cpu/cpu" . 0 . "/cpufreq/scaling_available_frequencies";
open FH, "<$file3";
my $freqss = <FH>;
chomp $freqss;
my @freqsa = split(/ /, $freqss);
foreach my $freq (@freqsa) {
   print " " . $freq;
}
print "\n";


