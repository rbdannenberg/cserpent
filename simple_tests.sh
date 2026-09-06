#!/usr/bin/env bash
#
# run_tests.sh - run serpent test programs and check for a literal
# substring match in their output.
#
# Usage:
#   ./run_tests.sh              # run all tests
#   ./run_tests.sh <name>       # run only the named test (e.g. forloop)
#
# To add a new test:
#   1. Put the program at tests/serpent/simple/<name>.srp
#   2. Put the expected output snippet at tests/serpent/simple/<name>.expected
#   3. Add "<name>" to the TESTS array below.

set -u

SERPENT_DIR="tests/serpent/simple"
BUILD_OUT="tsttmp"

# ---- list of tests: just the base name (no .srp) ----
TESTS=(
    oneplustwo
    forloop
    forintloop
    helloworld
    easyglobal
    easyglobaltypes
    array
    callreturn
)

# ---------------------------------------------------------

# Determine which tests to run.
if [[ $# -eq 0 ]]; then
    tests_to_run=("${TESTS[@]}")
elif [[ $# -eq 1 ]]; then
    requested="$1"
    found=0
    for t in "${TESTS[@]}"; do
        if [[ "$t" == "$requested" ]]; then
            found=1
            break
        fi
    done
    if [[ $found -eq 0 ]]; then
        echo "Error: no such test '$requested'"
        echo "Available tests: ${TESTS[*]}"
        exit 1
    fi
    tests_to_run=("$requested")
else
    echo "Usage: $0 [test-name]"
    exit 1
fi

pass_count=0
fail_count=0

for name in "${tests_to_run[@]}"; do
    srp_file="${SERPENT_DIR}/${name}.srp"
    expected_file="${SERPENT_DIR}/${name}.expected"

    echo "=== Running test: ${name} ==="

    if [[ ! -f "$srp_file" ]]; then
        echo "  FAIL: source file not found: $srp_file"
        fail_count=$((fail_count + 1))
        continue
    fi

    if [[ ! -f "$expected_file" ]]; then
        echo "  FAIL: expected-output file not found: $expected_file"
        fail_count=$((fail_count + 1))
        continue
    fi

    # Run the compiler/test command, capturing stdout+stderr together.
    actual_output=$(serpent64 compiler.srp "$srp_file" -o "$BUILD_OUT" -c -d 2>&1)

    expected_output=$(cat "$expected_file")

    # Literal substring match (fixed string, not a pattern).
    if grep -F -q -- "$expected_output" <<< "$actual_output"; then
        echo "  PASS"
        pass_count=$((pass_count + 1))
    else
        echo "  FAIL: expected output not found in actual output"
        echo "  ---- expected snippet ----"
        echo "$expected_output"
        echo "  ---- actual output ----"
        echo "$actual_output"
        echo "  ---------------------------"
        fail_count=$((fail_count + 1))
    fi
done

echo
echo "===================================="
echo "Passed: $pass_count   Failed: $fail_count"
echo "===================================="

[[ $fail_count -eq 0 ]]
