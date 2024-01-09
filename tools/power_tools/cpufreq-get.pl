#!/usr/bin/perl -w
use strict;
use warnings;

# set all cpus at lowest speed
my $idx;
my @freqs= ();
my $ncpus = `grep -c processor /proc/cpuinfo`;
for($idx = 0; $idx<$ncpus; $idx++) {
   my $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/scaling_cur_freq";
   open FH, "<$file";
   my $freq = <FH>;
   chomp $freq;
   push @freqs, $freq;
   close FH;
}
print "@freqs\n";


