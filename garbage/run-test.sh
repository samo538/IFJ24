#!/bin/sh

NUM_OF_FILES=$(ls tests/ | wc -l)
echo "num of test files: $NUM_OF_FILES"
echo " "

TEST_FILES=$(ls tests/)
echo "list of test files:"
echo $TEST_FILES
echo " "

make > /dev/null

GREEN='\033[0;32m'
RESET='\e[0m'
RED="\e[0;31m"

echo -e "${GREEN}=============START==============${RESET}"

for i in $TEST_FILES
do

    read -p "Press any key to continue..." -n1 -s
    echo " "
    echo "test file: $i"
    echo " "

    echo "running compiler..."
    ./compiler < tests/$i > asm
    echo "done"
    echo " "

    echo "runing ic24int..."
    echo " "
    echo -e "${RED}output:${RESET}"
    echo " "
    ./ic24int asm
    echo " "
    echo -e "${GREEN}======================================${RESET}"
    echo " "
    
    

done



