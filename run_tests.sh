# For every file $TESTFILE in tests/serpent/full_pipeline,
# runs the following two commands:
# serpent64 compiler.srp $TESTFILE -o test_temp -c
# serpent64 $TESTFILE
# If the output of the second command is the same as the output of the first command,
# the test passes. Otherwise, the test fails.

# The script will print the results of the tests to the console.

# The script will also print the number of tests that passed and the number of tests that failed.

# The script will return 0 if all tests pass, and 1 if any test fails.


# Get the directory of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Get the directory of the tests
TESTDIR=$DIR/tests/serpent/full_pipeline

# Get the directory of the serpent64 executable
SERPENTDIR=$DIR/serpent64

# Get the directory of the serpent64 compiler
COMPILERDIR=$DIR/compiler.srp

# Get the directory of the temporary test file
OUTPUT_FILE1=output_1.txt
OUTPUT_FILE2=output_2.txt

BLUE='\033[0;34m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Initialize the number of tests that passed and the number of tests that failed
PASSED=0
FAILED=0
TIMEOUT_DURATION=5s

if [ -d autotest_temp_dir ]
then
    rm -r autotest_temp_dir
fi
# For every file $TESTFILE in tests/serpent/full_pipeline
# Try for TESTIFLE = $TESTDIR/arrays.srp
for TESTFILE in $TESTDIR/*.srp
do
    echo "---------------------------------------------------------"
    echo -e "${BLUE}Running test ${TESTFILE}...${NC}"
    # Run the following two commands:
    # serpent64 compiler.srp $TESTFILE -o test_temp -c
    # serpent64 $TESTFILE
    timeout $TIMEOUT_DURATION serpent64 $COMPILERDIR $TESTFILE -o autotest_temp_dir -c > $OUTPUT_FILE1 2>/dev/null
    EXIT_STATUS1=$?
    timeout $TIMEOUT_DURATION serpent64 $TESTFILE > $OUTPUT_FILE2 2>/dev/null
    EXIT_STATUS2=$?

    # If the output of the second command is the same as the output of the first command
    if [ $EXIT_STATUS1 -eq 124 ] || [ $EXIT_STATUS2 -eq 124 ]
    then
        echo -e "${RED}Test $TESTFILE timed out${NC}"
        FAILED=$((FAILED+1))
        continue
    fi
    if [ "$(diff $OUTPUT_FILE1 $OUTPUT_FILE2)" == "" ]
    then
        # The test passes
        PASSED=$((PASSED+1))
        echo -e "${GREEN}Test $TESTFILE passed${NC}"
    else
        # The test fails
        FAILED=$((FAILED+1))
        echo -e "${RED}Test $TESTFILE failed${NC}"
        diff $OUTPUT_FILE1 $OUTPUT_FILE2
    fi
    # Remove the temporary test directory
    rm -r autotest_temp_dir
    #rm $OUTPUT_FILE1
    #rm $OUTPUT_FILE2
done

# Print the number of tests that passed and the number of tests that failed
echo "Number of tests that passed: $PASSED"
echo "Number of tests that failed: $FAILED"

# Return 0 if all tests pass, and 1 if any test fails
if [ $FAILED -eq 0 ]
then
    exit 0
else
    exit 1
fi




