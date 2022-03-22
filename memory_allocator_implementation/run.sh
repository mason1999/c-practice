#!/bin/bash

# We assume we have executables called test1, test2, test3,...

# This bash script must be able to build each test and run it. 
x=1
while [ $x -le 13 ]
do
      echo "----------------------------------------------------------------"
      cat ./test_descriptions/"testing"$x"_description"
      ./"testing$x" > "t$x"
      echo ""
      echo "Program output is:"
      echo ""
      ./"testing$x"
      echo ""
      echo "Actual output should be:"
      echo ""
      cat ./test_out_files/"testing$x.out"
      echo ""
      DIFF=$(cat "t$x" | diff - ./test_out_files/"testing$x.out")
      
      if [ "$DIFF" != "" ]
      then 
	 echo "The difference was:"
	 diff ./test_out_files/testing"$x".out t$x
      else  
	 echo "You passed!"
      fi
      rm "testing$x"
      rm "t$x"

x=$(( $x + 1))
done
