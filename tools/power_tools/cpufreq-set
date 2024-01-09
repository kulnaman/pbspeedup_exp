#!/usr/bin/perl -w
use strict;
use warnings;

# find how many cpus on the system
my $ncpus = `grep -c processor /proc/cpuinfo`;

my ($max_freq, $min_freq, %hash);

# get minimum frequency
my $file1 = "/sys/devices/system/cpu/cpu" . 0 . "/cpufreq/scaling_min_freq";
open FH, "<$file1";
$min_freq = <FH>;
chomp $min_freq;
close FH;

# get maximum frequency
my $file2 = "/sys/devices/system/cpu/cpu" . 0 . "/cpufreq/scaling_max_freq";
open FH, "<$file2";
$max_freq = <FH>;
chomp $max_freq;

# get all available frequencies
my $file3 = "/sys/devices/system/cpu/cpu" . 0 . "/cpufreq/scaling_available_frequencies";
open FH, "<$file3";
my $freqss = <FH>;
chomp $freqss;
my @freqsa = split(/ /, $freqss);
foreach my $freq (@freqsa) {
   $hash{$freq} = 1;
}

my @set_freqs;
my $idx;
my $set_freq = shift @ARGV;
for($idx=0; $idx<$ncpus; $idx++) {
    if ( defined($set_freq) ) {
       if ( $set_freq eq "max") {
           $set_freq = $max_freq;
       } elsif ( $set_freq eq "min") {
           $set_freq =$min_freq;
       } 

       if ( ! exists $hash{$set_freq} ) {
           print "unsupported frequency--" . $set_freq . "\n";
           exit -2;
       }
       push @set_freqs, $set_freq;
    }
}

# set all cpus at lowest speed
for($idx = 0; $idx<$ncpus; $idx++) {
   if ( defined($set_freqs[$idx]) ) {
       my $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/scaling_setspeed";
       open FH, ">$file";
       print FH $set_freqs[$idx];
       close FH;
   }
}


