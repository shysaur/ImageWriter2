#!/usr/bin/perl
# 
# BuildIDGen
# Tool to generate a condensed machine ID to use for version numbers
# (c) 2012-2013 Daniele Cattaneo
#
# Usage: ./buildidgen.pl $HOSTNAME


@encoding = (
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
  0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
 25,26,27,28,29,30,31,32,33,34,35, 0, 0, 0, 0, 0,
  0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
 25,26,27,28,29,30,31,32,33,34,35, 0, 0, 0, 0, 0);
 
@outenc = split(//,
  'ABCDEFGHIJKLMNOPQRSTUVWXYZ:&%#0123456789!$?.abcdefghijklmnopqrstuvwxyz');
  
@instr = split(//, '?'.$ARGV[0]);

 
for ($i=0; $i<3; $i++) {  
  print $outenc[
    abs($encoding[ord($instr[$i*3+2])] +
        $encoding[ord($instr[$i*3+1])] -
        $encoding[ord($instr[$i*3  ])]
       )
    ];
}
print "\n";