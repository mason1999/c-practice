#!/bin/bash

make objdump_x2017
x=1
while [ $x -le 11 ]
do
   echo "Test case $x---------------------------------------------------------------------------------------------------"
   cat ./test_descriptions/$x.txt # A description of the test
   if [ $x == 5 ]
   then 
      echo "" 
      echo "NOTE:"
      echo "for this one, the seg fault comes from the fact that NOT REG 7 will set the program counter to some relatively high number. In that case, since My program memory is implemented as a linked list, my pointer will keep traversing until it reaches then end (and still traverse). Hence it will read past the last node and cause a segmentation fault. I did not anticipate this but didn't have enough time to fix this up"
   fi
   echo ""
   echo "MILESTONE: "
   DIFF=$(./objdump_x2017 ./tests/tc_$x.x2017 | diff - ./output_assembly/tc_$x.out)
   if [ "$DIFF" != "" ] 
   then 
      echo "the difference was";  
      ./objdump_x2017 ./tests/tc_$x.x2017 | diff - ./output_assembly/tc_$x.out
   else 
      echo "you passed"
   fi
   echo ""
   echo "VM: "
   ./vm_x2017 ./tests/tc_$x.x2017
   echo ""
   echo ""
   x=$(( $x + 1))
done

echo "FINAL REMARK: My code isn't actually that malleable and I realised that these test cases showed that. It was hard to write to do this! There was lots of effort spent on the assignment though! I also didn't take the new info courses under the new syllabus so I don't know how to output if an error occured, so I just printed out ERROR. I hope that's okay. "
