#!/usr/bin/perl -w
use strict;
use warnings;

# stop cpuspeed service
#my $msg;
#$msg = `chkconfig --level 0123456 cpuspeed off`;
#$msg = `/etc/init.d/cpuspeed stop`;

# find how many cpus on the system
my $ncpus = `grep -c processor /proc/cpuinfo`;

# set scaling governor
my $idx;
my $file;
for($idx = 0; $idx<$ncpus; $idx++) {
   $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/scaling_governor";
   open(FH, ">$file");
   print FH "userspace";
   close FH;
}

# fix permissions
for($idx = 0; $idx<$ncpus; $idx++) {
   $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/scaling_setspeed";
   chmod 0666, $file;
   $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/cpuinfo_cur_freq";
   chmod 0444, $file;
}

# get min_frequency
$file = "/sys/devices/system/cpu/cpu" . 0 . "/cpufreq/scaling_min_freq";
open FH, "<$file";
my $min_freq = <FH>;
chomp $min_freq;
close FH;

# set all cpus at lowest speed
for($idx = 0; $idx<$ncpus; $idx++) {
   $file = "/sys/devices/system/cpu/cpu" . $idx . "/cpufreq/scaling_setspeed";
   open FH, ">$file";
   print FH "$min_freq";
   close FH;
}


